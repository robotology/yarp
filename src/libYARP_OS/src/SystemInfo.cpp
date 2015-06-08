/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2010 RobotCub Consortium
 *              Italian Institute of Technology (IIT)
 *              Via Morego 30, 16163, 
 *              Genova, Italy
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/SystemInfoSerializer.h>
using namespace yarp::os;

#if defined(__linux__)
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <sys/statvfs.h>
#include <pwd.h>

extern char **environ;

#endif

#if defined(WIN32)
#include <windows.h>
#include <shlobj.h>
#include <Lmcons.h>
#include <comdef.h>     // for using bstr_t class

#if (defined(WINVER)) && (WINVER>=0x0502)
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")
#endif 

#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>
//#include <yarp/os/impl/PlatformVector.h>
#include <vector>

static void enableCpuLoadCollector(void);
static void disableCpuLoadCollector(void);

#endif

#if defined(__linux__)
SystemInfo::capacity_t getMemEntry(const char *tag, const char *bufptr)
{
    char *tail;
    SystemInfo::capacity_t retval;
    size_t len = strlen(tag);
    while (bufptr) 
    {
        if (*bufptr == '\n') bufptr++;
        if (!strncmp(tag, bufptr, len)) 
        {
            retval = strtol(bufptr + len, &tail, 10);
            if (tail == bufptr + len)
                return -1;
            else
                return retval;      
        }
        bufptr = strchr( bufptr, '\n' );
    }
    return -1;
}


bool getCpuEntry(const char* tag, const char *buff, yarp::os::ConstString& value)
{
    if(strlen(buff) <= strlen(tag))
        return false;
        
    if(strncmp(buff, tag, strlen(tag)) != 0)
        return false;

    const char *pos1 = strchr(buff, ':');
    if(!pos1)
        return false;

    while((*pos1 != '\0') && ((*pos1 == ' ')||(*pos1 == ':')||(*pos1 == '\t'))) pos1++;
    const char* pos2 = buff + strlen(buff)-1;
    while((*pos2 != ':') && ((*pos2 == ' ')||(*pos2 == '\n'))) pos2--;
    if(pos2 < pos1)
        return false;
    value = yarp::os::ConstString(pos1, pos2-pos1+1);
    return true;
}
#endif


#if defined(WIN32)
class CpuLoadCollector: public yarp::os::RateThread 
{
public:
    CpuLoadCollector():RateThread(5000) 
    {
       firstRun = true;
       load.cpuLoad1 = 0.0;
       load.cpuLoad5 = 0.0;
       load.cpuLoad15 = 0.0;
       load.cpuLoadInstant = (int)0;
    }
    
    ~CpuLoadCollector() 
    {
    }

    void run() 
    {
        sem.wait();
        load.cpuLoadInstant = (int) phdCpuLoad();
        samples.push_back(load.cpuLoadInstant);
        if(samples.size() > 180)
            samples.erase(samples.begin());

        std::vector<int>::reverse_iterator rti;
        int sum = 0;
        int n = 0;
        for(rti=samples.rbegin(); rti<samples.rend(); ++rti)
        {
            sum += (*rti);
            n++;
            // every 1min
            if(n<12) 
                load.cpuLoad1 = (double)(sum/n)/100.0;
            // every 5min
            if(n<60) 
                load.cpuLoad5 = (double)(sum/n)/100.0;
            // every 15min
            load.cpuLoad15 = (double)(sum/n)/100.0;          
        } 
        sem.post();
        
    }

    LoadInfo getCpuLoad(void) 
    {
        sem.wait();
        LoadInfo ld = load;
        sem.post();
        return ld; 
    }

    //bool threadInit()
    //void threadRelease()
private: 
#if (defined(WINVER)) && (WINVER>=0x0502)    
    double phdCpuLoad()
    {        
        DWORD ret;
        if(firstRun)
        {
            phdStatus = PdhOpenQuery(NULL, 0, &hPhdQuery);
            if(phdStatus != ERROR_SUCCESS)
                return 0;

            PdhAddCounter(hPhdQuery, TEXT("\\Processor(_Total)\\% Processor Time"),0,&phdCounter);             
            PdhCollectQueryData(hPhdQuery);
            firstRun = false;
            return 0;
        }

        phdStatus = PdhCollectQueryData(hPhdQuery);
        if(phdStatus != ERROR_SUCCESS)        
            return 0;
        phdStatus = PdhGetFormattedCounterValue(phdCounter, 
                                                PDH_FMT_DOUBLE|PDH_FMT_NOCAP100,
                                                &ret, &phdFmtValue);
        if(phdStatus != ERROR_SUCCESS)
            return 0;
        return phdFmtValue.doubleValue;
    }
#else
    double phdCpuLoad() { return 0.0; }
#endif

private:
#if (defined(WINVER)) && (WINVER>=0x0502)    
    PDH_STATUS            phdStatus;    
    HQUERY                hPhdQuery;
    PDH_FMT_COUNTERVALUE  phdFmtValue;
    HCOUNTER              phdCounter;
#endif
    bool firstRun;
    LoadInfo load;
    std::vector<int> samples; 
    yarp::os::Semaphore sem;
};

static CpuLoadCollector* globalLoadCollector = NULL;

void enableCpuLoadCollector(void)
{
    if(globalLoadCollector == NULL)
    {
        globalLoadCollector = new CpuLoadCollector();
        globalLoadCollector->start();
    }
}

void disableCpuLoadCollector(void)
{
    if(globalLoadCollector)
    {        
        globalLoadCollector->stop();
        delete globalLoadCollector;
        globalLoadCollector = NULL;
    }
}

#endif



SystemInfo::MemoryInfo SystemInfo::getMemoryInfo()
{
    MemoryInfo memory;
    memory.totalSpace = 0;
    memory.freeSpace = 0;

#if defined(WIN32)
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    if(GlobalMemoryStatusEx (&statex))
    {
        memory.totalSpace = (capacity_t)(statex.ullTotalPhys/1048576);  //in Mb
        memory.freeSpace = (capacity_t)(statex.ullAvailPhys/1048576);   //in Mb
    }
#endif

#if defined(__linux__)
    char buffer[128];
    FILE* procmem = fopen("/proc/meminfo", "r");
    if(procmem)
    {
        while(fgets(buffer, 128, procmem))
        {
            capacity_t ret;
            if((ret=getMemEntry("MemTotal:", buffer)) > 0)   
                memory.totalSpace = ret/1024;
            
            if((ret=getMemEntry("MemFree:", buffer)) > 0)
                memory.freeSpace = ret/1024;
        }
        fclose(procmem);
    }
#endif
    return memory;
}


SystemInfo::StorageInfo SystemInfo::getStorageInfo()
{
    StorageInfo storage;
    storage.totalSpace = 0;
    storage.freeSpace = 0;

#if defined(WIN32)
        
        DWORD dwSectorsPerCluster=0, dwBytesPerSector=0;
        DWORD dwFreeClusters=0, dwTotalClusters=0;
        yarp::os::ConstString strHome = getUserInfo().homeDir; 
        if(!strHome.length())
            strHome = "C:\\";
        if(GetDiskFreeSpaceA(strHome.c_str(), &dwSectorsPerCluster, 
            &dwBytesPerSector, &dwFreeClusters, &dwTotalClusters)) 
        {
            storage.freeSpace = (capacity_t)((dwFreeClusters/1048576.0)* dwSectorsPerCluster * dwBytesPerSector);
            storage.totalSpace = (capacity_t)((dwTotalClusters)/1048576.0 * dwSectorsPerCluster * dwBytesPerSector); 
        } 
#endif

#if defined(__linux__)
    yarp::os::ConstString strHome = getUserInfo().homeDir; 
    if(!strHome.length())
        strHome = "/home";

    struct statvfs vfs;
    if(statvfs(strHome.c_str(), &vfs) == 0)
    {
        storage.totalSpace = (int)(vfs.f_blocks*vfs.f_bsize/(1048576)); // in MB
        storage.freeSpace = (int)(vfs.f_bavail*vfs.f_bsize/(1048576));  // in MB
    }

#endif
    return storage;
}

/*
SystemInfo::NetworkInfo SystemInfo::getNetworkInfo()
{
    NetworkInfo network;

#if defined(__linux__)

    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa ->ifa_addr->sa_family == AF_INET) 
        { 
            // is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            const char* ret = inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if(ret && (strcmp(ifa->ifa_name, "eth0") == 0))
                network.ip4 = addressBuffer;
        } 
        else if (ifa->ifa_addr->sa_family == AF_INET6) 
        {
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            const char* ret = inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            if(ret && (strcmp(ifa->ifa_name, "eth0") == 0))
                network.ip6 = addressBuffer;
        } 
    }

    if(ifAddrStruct) 
        freeifaddrs(ifAddrStruct);

    // getting mac addrress
    struct ifreq ifr;
    struct ifreq *IFR;
    struct ifconf ifc;
    char buf[1024];
    int s;
    bool found = false;

    if( (s = socket(AF_INET, SOCK_DGRAM, 0)) == -1 )
        return network;

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    ioctl(s, SIOCGIFCONF, &ifc);
    IFR = ifc.ifc_req;
    for (int i = ifc.ifc_len/sizeof(struct ifreq); --i >= 0; IFR++) 
    {
        strcpy(ifr.ifr_name, IFR->ifr_name);
        if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0) 
        {
            if (!(ifr.ifr_flags & IFF_LOOPBACK)) 
            {
                if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) 
                {
                    found = true;
                    break;
                }
            }
        }
    }
    close(s);

    if(found)
    {
        unsigned char addr[6]; 
        char mac[32];
        bcopy(ifr.ifr_hwaddr.sa_data, addr, 6);
        sprintf(mac, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", 
            addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
        network.mac = mac; 
    }

#endif
    return network;
}
*/


SystemInfo::ProcessorInfo SystemInfo::getProcessorInfo()
{
    ProcessorInfo processor;
    processor.cores = 0;
    processor.frequency = 0.0;
    processor.family = 0;
    processor.modelNumber = 0; 
    processor.siblings = 0;

#if defined(WIN32)
    SYSTEM_INFO sysinf;
    GetSystemInfo(&sysinf);
    switch (sysinf.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64: { processor.architecture = "X64"; break; }
        case PROCESSOR_ARCHITECTURE_IA64: { processor.architecture = "Intel Itanium-based"; break; }
        case PROCESSOR_ARCHITECTURE_INTEL: { processor.architecture = "X86"; break; }
        default: processor.architecture = "Unknown";
    };
    processor.siblings = sysinf.dwNumberOfProcessors;
    processor.modelNumber = sysinf.dwProcessorType;
    switch(sysinf.dwProcessorType) {
        case PROCESSOR_INTEL_386: { processor.model = "PROCESSOR_INTEL_386"; break; }
        case PROCESSOR_INTEL_486: { processor.model = "PROCESSOR_INTEL_486"; break; }
        case PROCESSOR_INTEL_PENTIUM: { processor.model = "PROCESSOR_INTEL_PENTIUM"; break; }
        case PROCESSOR_INTEL_IA64: { processor.model = "PROCESSOR_INTEL_IA64"; break; }
        case PROCESSOR_AMD_X8664: { processor.model = "PROCESSOR_AMD_X8664"; break; }
    };
    
    DWORD dwMHz;
    DWORD BufSize = sizeof(DWORD);
    HKEY hKey;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                        "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    { 
        RegQueryValueEx(hKey, "~MHz", NULL, NULL, (LPBYTE) &dwMHz, &BufSize);
        processor.frequency = (double) dwMHz;
        RegCloseKey(hKey);
    }

    // TODO: this should be fixed to obtain correct number of physical cores
    processor.cores = 1;

#endif

#if defined(__linux__)
    char buffer[128];
    FILE* proccpu = fopen("/proc/cpuinfo", "r");
    if(proccpu)
    {
        while(fgets(buffer, 128, proccpu))
        {
            yarp::os::ConstString value;
            if(getCpuEntry("model", buffer, value) && 
               !getCpuEntry("model name", buffer, value))
                processor.modelNumber = atoi(value.c_str());
            if(getCpuEntry("model name", buffer, value))
                processor.model = value;            
            if(getCpuEntry("vendor_id", buffer, value))
                processor.vendor = value;            
            if(getCpuEntry("cpu family", buffer, value))
                processor.family = atoi(value.c_str());            
            if(getCpuEntry("cpu cores", buffer, value))
                processor.cores = atoi(value.c_str());
            if(getCpuEntry("siblings", buffer, value))
                processor.siblings = atoi(value.c_str());
            if(getCpuEntry("cpu MHz", buffer, value))
                processor.frequency = atof(value.c_str());
        }
        fclose(proccpu);
    }

    struct utsname uts;
    if(uname(&uts) == 0)
    {
      processor.architecture = uts.machine;  
    }
#endif
    return processor;
}


SystemInfo::PlatformInfo SystemInfo::getPlatformInfo()
{
    PlatformInfo platform;

#if defined(WIN32)
    platform.name = "Windows";
    OSVERSIONINFOEX osver;
    osver.dwOSVersionInfoSize = sizeof(osver);
    if(GetVersionEx((LPOSVERSIONINFO)&osver))
    {
        char buff[64];
        sprintf(buff,"%d.%d", osver.dwMajorVersion, osver.dwMinorVersion);
        platform.release = buff;
        platform.codename = buff;
        if((osver.dwMajorVersion == 6) && (osver.dwMinorVersion == 1) &&
            (osver.wProductType == VER_NT_WORKSTATION))
           platform.distribution = "7"; 
        else if((osver.dwMajorVersion == 6) && (osver.dwMinorVersion == 1) &&
            (osver.wProductType != VER_NT_WORKSTATION))
           platform.distribution = "Server 2008 R2"; 
        else if((osver.dwMajorVersion == 6) && (osver.dwMinorVersion == 0) &&
            (osver.wProductType == VER_NT_WORKSTATION))
           platform.distribution = "Vista"; 
        else if((osver.dwMajorVersion == 6) && (osver.dwMinorVersion == 0) &&
            (osver.wProductType != VER_NT_WORKSTATION))
           platform.distribution = "Server 2008"; 
        else if((osver.dwMajorVersion == 5) && (osver.dwMinorVersion == 2))
           platform.distribution = "Server 2003"; 
    //    else if((osver.dwMajorVersion == 5) && (osver.dwMinorVersion == 2) &&
    //        (osver.wSuiteMask & VER_SUITE_WH_SERVER))
    //       platform.distribution = "Home Server";         
    //    else if((osver.dwMajorVersion == 5) && (osver.dwMinorVersion == 2) &&
    //            (GetSystemMetrics(SM_SERVERR2) != 0))
    //       platform.distribution = "Server 2003 R2"; 
        else if((osver.dwMajorVersion == 5) && (osver.dwMinorVersion == 2) &&
                (osver.wProductType == VER_NT_WORKSTATION))
                /* &&(osver.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)*/
           platform.distribution = "XP Professional x64 Edition";
        else if((osver.dwMajorVersion == 5) && (osver.dwMinorVersion == 1))
           platform.distribution = "XP";         
        else if((osver.dwMajorVersion == 5) && (osver.dwMinorVersion == 2))
            platform.distribution = "2000";
    }

    const char* a = GetEnvironmentStrings();
    size_t prev = 0;
    for(size_t i = 0; ; i++) {
    if (a[i] == '\0') {
        std::string tmpVariable(a + prev, a + i);
        size_t equalsSign=tmpVariable.find("=");
        if(equalsSign!=std::string::npos && equalsSign!=0) // among enviroment variables there are DOS-related ones that start with a =
        {
            platform.environmentVars.put(tmpVariable.substr(0, equalsSign), tmpVariable.substr(equalsSign+1));
        }
        prev = i+1;
        if (a[i + 1] == '\0') {
            break;
        }
    }
}

#endif 

#if defined(__linux__)
    struct utsname uts;
    if(uname(&uts) == 0)
    {
      platform.name = uts.sysname;
      platform.kernel = uts.release;
    }
    else
        platform.name = "Linux";

    char buffer[128];
    FILE* release = popen("lsb_release -ric", "r");
    if (release) 
    {
        while(fgets(buffer, 128, release))
        {
            yarp::os::ConstString value;
            if(getCpuEntry("Distributor ID", buffer, value))
                platform.distribution = value;
            if(getCpuEntry("Release", buffer, value))
                platform.release = value;
            if(getCpuEntry("Codename", buffer, value))
                platform.codename = value;
        }
        pclose(release);
    }            

    char *varChar = *environ;

    for (int i = 0; varChar; i++) {
        std::string tmpVariable(varChar);
        size_t equalsSign=tmpVariable.find("=");
        if(equalsSign!=std::string::npos)
        {
            platform.environmentVars.put(tmpVariable.substr(0, equalsSign), tmpVariable.substr(equalsSign+1));
        }
        varChar = *(environ+i);

    }
#endif
        return platform;
}


SystemInfo::UserInfo SystemInfo::getUserInfo()
{
    UserInfo user;
    user.userID = 0;

#if defined(WIN32)    
    char path[MAX_PATH+1];
    if(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path ) == S_OK)
        user.homeDir = path;

    char username[UNLEN+1];
    DWORD nsize = UNLEN+1;
    if(GetUserName(username, &nsize))
    {
        user.userName = username;
        user.realName = username;
    }
#endif

#if defined(__linux__)
    struct passwd* pwd = getpwuid(getuid());
    user.userID = getuid();
    if(pwd)
    {
        user.userName = pwd->pw_name;
        user.realName = pwd->pw_gecos;
        user.homeDir = pwd->pw_dir;
    }
#endif
        return user;
}


SystemInfo::LoadInfo SystemInfo::getLoadInfo()
{
    LoadInfo load;
    load.cpuLoad1 = 0.0;
    load.cpuLoad5 = 0.0;
    load.cpuLoad15 = 0.0;
    load.cpuLoadInstant = 0;

#if defined(WIN32)
    if(globalLoadCollector)
    {
        load = globalLoadCollector->getCpuLoad();
        int siblings = getProcessorInfo().siblings;        
        if( siblings > 1)
        {
            load.cpuLoad1 *= siblings;
            load.cpuLoad5 *= siblings;
            load.cpuLoad15 *= siblings;
        }
    }
#endif

#if defined(__linux__)
    FILE* procload = fopen("/proc/loadavg", "r");
    if(procload)
    {
        char buff[128];
        int ret = fscanf(procload, "%lf %lf %lf %s", 
                         &(load.cpuLoad1), &(load.cpuLoad5), 
                         &(load.cpuLoad15), buff);
        if(ret>0)
        {
            char* tail  = strchr(buff, '/');
            if(tail && (tail != buff))
                load.cpuLoadInstant = (int)(strtol(buff, &tail, 0) - 1);
        }
        fclose(procload);
    }
#endif
    return load;
}

