/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/localbroker.h>
#include <yarp/conf/string.h>

#include <csignal>
#include <cstring>

#define RUN_TIMEOUT             10.0        //seconds
#define STOP_TIMEOUT            15.0
#define KILL_TIMEOUT            10.0
#define CONNECTION_TIMEOUT      2.0

#define WRITE_TO_PIPE           1
#define READ_FROM_PIPE          0

#if defined(_WIN32)
    #include<Windows.h>
    #define SIGKILL 9
#else
    #include <cstdlib>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <cerrno>
    #include <unistd.h>
    #include <cstring>

    #define PIPE_TIMEOUT        0
    #define PIPE_EVENT          1
    #define PIPE_SIGNALED       2
    #define C_MAXARGS           128         // max number of the command parameters
#endif

using namespace yarp::os;
using namespace yarp::manager;
using namespace std;


#if defined(_WIN32)
class LocalTerminateParams
{
public:
    LocalTerminateParams(DWORD id) {
        nWin = 0;
        dwID = id;
    }

    ~LocalTerminateParams(){}
    int nWin;
    DWORD dwID;
};

BOOL CALLBACK LocalTerminateAppEnum(HWND hwnd, LPARAM lParam)
{
    LocalTerminateParams* params=(LocalTerminateParams*)lParam;
    DWORD dwID;
    GetWindowThreadProcessId(hwnd, &dwID);
    if (dwID==params->dwID)
    {
        params->nWin++;
        PostMessage(hwnd,WM_CLOSE,0,0);
    }
    return TRUE ;
}
#if defined(_WIN64)
volatile LONGLONG uniquePipeNumber = 0;
#else
volatile LONG uniquePipeNumber = 0;
#endif

/*
*  TODO: check deeply for asyn PIPE
*/
BOOL CreatePipeAsync(
    OUT LPHANDLE lpReadPipe,
    OUT LPHANDLE lpWritePipe,
    IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
    IN DWORD nSize)
{
    HANDLE ReadPipeHandle, WritePipeHandle;
    DWORD dwError;
    char PipeNameBuffer[MAX_PATH];
    nSize = (nSize ==0) ? 100*8096: nSize;

#if defined(_WIN64)
    InterlockedIncrement64(&uniquePipeNumber);
#else
    InterlockedIncrement(&uniquePipeNumber);
#endif

    sprintf( PipeNameBuffer,
             "\\\\.\\Pipe\\RemoteExeAnon.%08x.%08x",
             GetCurrentProcessId(),
             uniquePipeNumber
           );

    ReadPipeHandle = CreateNamedPipeA(
                         (LPSTR)PipeNameBuffer,
                         PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
                         PIPE_TYPE_BYTE | PIPE_WAIT, //PIPE_NOWAIT,
                         1,             // Number of pipes
                         nSize,         // Out buffer size
                         nSize,         // In buffer size
                         120 * 1000,    // Timeout in ms
                         lpPipeAttributes
                         );

    if (! ReadPipeHandle) {
        return FALSE;
    }

    WritePipeHandle = CreateFileA(
                        (LPSTR)PipeNameBuffer,
                        GENERIC_WRITE,
                        0,                         // No sharing
                        lpPipeAttributes,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                        nullptr                       // Template file
                      );

    if (INVALID_HANDLE_VALUE == WritePipeHandle)
    {
        dwError = GetLastError();
        CloseHandle( ReadPipeHandle );
        SetLastError(dwError);
        return FALSE;
    }

    *lpReadPipe = ReadPipeHandle;
    *lpWritePipe = WritePipeHandle;
    return( TRUE );
}

#endif

LocalBroker::LocalBroker()
{
    bOnlyConnector = bInitialized = false;
    ID = 0;
    fd_stdout = nullptr;
    setWindowMode(WINDOW_HIDDEN);
}


LocalBroker::~LocalBroker()
{
    fini();
}

void LocalBroker::fini()
{
    if(Thread::isRunning())
        Thread::stop();
}

bool LocalBroker::init()
{
    /*
    if(!NetworkBase::checkNetwork(5.0))
    {
        strError = "YARP network server is not up.";
        return false;
    }
    */
    bInitialized = true;
    bOnlyConnector = true;
    return true;
}

bool LocalBroker::init(const char* szcmd, const char* szparam,
            const char* szhost, const char* szstdio,
            const char* szworkdir, const char* szenv )
{

    strCmd.clear();
    strParam.clear();
    strHost.clear();
    strStdio.clear();
    strWorkdir.clear();
    strTag.clear();
    strEnv.clear();

    if(!szcmd)
    {
        strError = "command is not specified.";
        return false;
    }
    strCmd = szcmd;
    if(szparam && strlen(szparam))
        strParam = szparam;

    if(szhost && strlen(szhost))
        strHost = szhost;
    if(szworkdir && strlen(szworkdir))
        strWorkdir = szworkdir;

    if(szstdio && strlen(szstdio))
    {
        if(szstdio[0] != '/')
            strStdio = string("/") + string(szstdio);
        else
            strStdio = szstdio;
    }

    if(szenv && strlen(szenv))
        strEnv = szenv;

    /*
    OSTRINGSTREAM sstrID;
    sstrID<<ID;
    strTag = strHost + strCmd + sstrID.str();

    if(!NetworkBase::checkNetwork(5.0))
    {
        strError = "YARP network server is not up.";
        semParam.post();
        return false;
    }
    */

#if defined(_WIN32)
    // do nothing
    bInitialized = true;
    return true;
#else
    /* avoiding zombie */
    struct sigaction new_action;
    new_action.sa_handler = SIG_IGN;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction (SIGCHLD, &new_action, nullptr);
    bInitialized = true;
    return true;
#endif

}


bool LocalBroker::start()
{
    if(!bInitialized) return false;
    if(bOnlyConnector) return false;

    if(running())
        return true;

    strError.clear();
    ID = ExecuteCmd();
    if(!ID)
        return false;

   if(running())
   {
        return true;
   }
   return false;
}

bool LocalBroker::stop()
{
    if(!bInitialized) return true;
    if(bOnlyConnector) return false;

    strError.clear();
#if defined(_WIN32)
    stopCmd(ID);
    stopStdout();
#else
    stopStdout();
    stopCmd(ID);
#endif

    double base = SystemClock::nowSystem();
    while(!timeout(base, STOP_TIMEOUT))
    {
        if(!running())
            return true;
    }

    strError = "Timeout! cannot stop ";
    strError += strCmd;
    strError += " on ";
    strError += strHost;
    return false;
}

bool LocalBroker::kill()
{
    if(!bInitialized) return true;
    if(bOnlyConnector) return false;

    strError.clear();

#if defined(_WIN32)
    killCmd(ID);
    stopStdout();
#else
    stopStdout();
    stopCmd(ID);
#endif

    double base = SystemClock::nowSystem();
    while(!timeout(base, KILL_TIMEOUT))
    {
        if(!running())
            return true;
    }

    strError = "Timeout! cannot kill ";
    strError += strCmd;
    strError += " on ";
    strError += strHost;
    return false;
}


int LocalBroker::running()
{
    if(!bInitialized) return 0;
    if(bOnlyConnector) return 0;
    return (psCmd(ID))?1:0;
}


/**
 *  connection broker
 */
bool LocalBroker::connect(const char* from, const char* to,
            const char* carrier, bool persist)
{

    if(!from)
    {
        strError = "no source port is introduced.";
        return false;
    }

    if(!to)
    {
        strError = "no destination port is introduced.";
        return false;
    }

    if(!exists(from))
    {
        strError = from;
        strError += " does not exist.";
        return false;
    }

    if(!exists(to))
    {
        strError = to;
        strError += " does not exist.";
        return false;
    }

    if(!NetworkBase::connect(from, to, carrier) || !connected(from, to, carrier))
    {
        strError = "cannot connect ";
        strError +=from;
        strError += " to " + string(to);
        return false;
    }
    return true;
}

bool LocalBroker::disconnect(const char* from, const char* to, const char *carrier)
{

    if(!from)
    {
        strError = "no source port is introduced.";
        return false;
    }

    if(!to)
    {
        strError = "no destination port is introduced.";
        return false;
    }

    if(!exists(from))
    {
        strError = from;
        strError += " does not exist.";
        return true;
    }

    if(!exists(to))
    {
        strError = to;
        strError += " does not exist.";
        return true;
    }

    if(!connected(from, to, carrier))
        return true;

    if(!NetworkBase::disconnect(from, to))
    {
        strError = "cannot disconnect ";
        strError +=from;
        strError += " from " + string(to);
        return false;
    }
    return true;

}

bool LocalBroker::exists(const char* port)
{
    return NetworkBase::exists(port);
}


const char* LocalBroker::requestRpc(const char* szport, const char* request, double timeout)
{
    if((szport==nullptr) || (request==nullptr))
        return nullptr;

    if(!exists(szport))
        return nullptr;

    // opening the port
    yarp::os::Port port;
    port.setTimeout((float)((timeout>0.0) ? timeout : CONNECTION_TIMEOUT));
    if(!port.open("..."))
        return nullptr;

    ContactStyle style;
    style.quiet = true;
    style.timeout = (timeout>0.0) ? timeout : CONNECTION_TIMEOUT;
    bool ret;
    for(int i=0; i<10; i++) {
        ret = NetworkBase::connect(port.getName(), szport, style);
        if(ret) break;
        SystemClock::delaySystem(1.0);
    }

    if(!ret) {
        port.close();
        return nullptr;
    }

    Bottle msg, response;
    msg.fromString(request);
    ret = port.write(msg, response);
    NetworkBase::disconnect(port.getName(), szport);
    if(!response.size() || !ret) {
        port.close();
        return nullptr;
    }

    port.close();
    return response.toString().c_str();
}

bool LocalBroker::connected(const char* from, const char* to, const char* carrier)
{
    if(!exists(from) || !exists(to))
        return false;
    return NetworkBase::isConnected(from, to);
}


const char* LocalBroker::error()
{
    return strError.c_str();
}

bool LocalBroker::attachStdout()
{
    if(Thread::isRunning())
        return true;
    if(!running())
    {
        strError = "Module is not running";
        return false;
    }
    return startStdout();
}

void LocalBroker::detachStdout()
{
   stopStdout();
}


bool LocalBroker::timeout(double base, double timeout)
{
    SystemClock::delaySystem(1.0);
    if((SystemClock::nowSystem()-base) > timeout)
        return true;
    return false;
}

bool LocalBroker::threadInit()
{
   return true;
}


void LocalBroker::run()
{

#if defined(_WIN32)
    //windows implementation
    DWORD dwRead;
    CHAR buff[1024];
    while(!Thread::isStopping())
    {
        BOOL bRet = ReadFile(read_from_pipe_cmd_to_stdout,
                             buff, 1023, &dwRead, nullptr);
        if(!bRet)
            break;
        buff[dwRead] = (CHAR)0;
        if(eventSink && strlen(buff))
            eventSink->onBrokerStdout(buff);
        yarp::os::SystemClock::delaySystem(0.5); // this prevents event flooding
    }
#else
    while(!Thread::isStopping())
    {
        if(waitPipeSignal(pipe_to_stdout[READ_FROM_PIPE]) == PIPE_EVENT)
        {
           if(fd_stdout)
           {
                string strmsg;
                char buff[1024];
                while(fgets(buff, 1024, fd_stdout))
                    strmsg += string(buff);
                if(eventSink && strmsg.size())
                    eventSink->onBrokerStdout(strmsg.c_str());
                yarp::os::SystemClock::delaySystem(0.5); // this prevents event flooding
           }
        }
    }
#endif
}


void LocalBroker::threadRelease()
{
}


void LocalBroker::setWindowMode(WindowMode m)
{
    windowMode=m;
}


#if defined(_WIN32)

string LocalBroker::lastError2String()
{
    int error=GetLastError();
    char buff[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,nullptr,error,0,buff,1024,nullptr);
    return string(buff);
}

bool LocalBroker::startStdout()
{
    if (!CloseHandle(write_to_pipe_cmd_to_stdout))
        return false;
    Thread::start();
    return true;
}

void LocalBroker::stopStdout()
{
    Thread::stop();
}

int LocalBroker::ExecuteCmd()
{
    string strCmdLine = strCmd + string(" ") + strParam;

    PROCESS_INFORMATION cmd_process_info;
    STARTUPINFO cmd_startup_info;
    ZeroMemory(&cmd_process_info,sizeof(PROCESS_INFORMATION));
    ZeroMemory(&cmd_startup_info,sizeof(STARTUPINFO));
    cmd_startup_info.cb = sizeof(STARTUPINFO);


    string strDisplay=getDisplay();

    DWORD dwCreationFlags;

    //these come from xml
    /*
    // These are not supported until we find a way to send break signals to
    // consoles that are not inherited
    if (strDisplay=="--visible_na")
        windowMode=WINDOW_VISIBLE;
    if (strDisplay=="--hidden")
        windowMode=WINDOW_HIDDEN;
    */

    // this is for "attach to stoud only"
    if (windowMode==WINDOW_VISIBLE)
    {
        //this is common to all processes
        cmd_startup_info.dwFlags |= STARTF_USESHOWWINDOW;
        cmd_startup_info.wShowWindow = SW_SHOWNA;
        dwCreationFlags=CREATE_NEW_CONSOLE;
    }
    if (windowMode==WINDOW_HIDDEN)
    {
        // Setting up child process and pipe for stdout (useful for attaching stdout)
        SECURITY_ATTRIBUTES pipe_sec_attr;
        pipe_sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
        pipe_sec_attr.bInheritHandle = TRUE;
        pipe_sec_attr.lpSecurityDescriptor = nullptr;
        CreatePipeAsync(&read_from_pipe_cmd_to_stdout,
                   &write_to_pipe_cmd_to_stdout,
                   &pipe_sec_attr, 0);

        cmd_startup_info.hStdError = write_to_pipe_cmd_to_stdout;
        cmd_startup_info.hStdOutput = write_to_pipe_cmd_to_stdout;

        cmd_startup_info.dwFlags |= STARTF_USESTDHANDLES;

        dwCreationFlags=CREATE_NEW_PROCESS_GROUP; //CREATE_NEW_CONSOLE|CREATE_NEW_PROCESS_GROUP,
    }



    /*
     * setting environment variable for child process
     */
    TCHAR chNewEnv[32767];

    // Get a pointer to the env block.
    LPTCH chOldEnv = GetEnvironmentStrings();

    // copying parent env variables
    LPTSTR lpOld = (LPTSTR) chOldEnv;
    LPTSTR lpNew = (LPTSTR) chNewEnv;
    while (*lpOld)
    {
        lstrcpy(lpNew, lpOld);
        lpOld += lstrlen(lpOld) + 1;
        lpNew += lstrlen(lpNew) + 1;
    }

    // adding new env variables
    std::string cstrEnvName;
    if(strEnv.size())
    {
        auto ss = yarp::conf::string::split(strEnv, ';');
        for (const auto& s : ss) {
            lstrcpy(lpNew, (LPTCH) s.c_str());
            lpNew += lstrlen(lpNew) + 1;
        }
    }

    // closing env block
    *lpNew = (TCHAR)0;

    bool bWorkdir=(strWorkdir.size()) ? true : false;
    string strWorkdirOk = bWorkdir ? strWorkdir+string("\\") : "";

    BOOL bSuccess=CreateProcess(nullptr,   // command name
                                (char*)(strWorkdirOk+strCmdLine).c_str(), // command line
                                nullptr,          // process security attributes
                                nullptr,          // primary thread security attributes
                                TRUE,          // handles are inherited
                                dwCreationFlags,
                                (LPVOID) chNewEnv, // use new environment
                                bWorkdir?strWorkdirOk.c_str():nullptr, // working directory
                                &cmd_startup_info,   // STARTUPINFO pointer
                                &cmd_process_info);  // receives PROCESS_INFORMATION

    if (!bSuccess && bWorkdir)
    {
            bSuccess=CreateProcess(nullptr,    // command name
                                    (char*)(strCmdLine.c_str()), // command line
                                    nullptr,          // process security attributes
                                    nullptr,          // primary thread security attributes
                                    TRUE,          // handles are inherited
                                    dwCreationFlags,
                                    (LPVOID) chNewEnv, // use new environment
                                    strWorkdirOk.c_str(), // working directory
                                    &cmd_startup_info,   // STARTUPINFO pointer
                                    &cmd_process_info);  // receives PROCESS_INFORMATION
    }

    // deleting old environment variable
    FreeEnvironmentStrings(chOldEnv);

    CloseHandle(cmd_process_info.hProcess);
    CloseHandle(cmd_process_info.hThread);

    if (!bSuccess)
    {
        strError = string("Can't execute command because ") + lastError2String();
        return 0;
    }

    return cmd_process_info.dwProcessId;
}

bool LocalBroker::psCmd(int pid)
{
    HANDLE hProc=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProc==nullptr)
        return false;

    DWORD status;
    GetExitCodeProcess(hProc , &status);
    CloseHandle(hProc);
    return (status==STILL_ACTIVE);
}

bool LocalBroker::killCmd(int pid)
{
    HANDLE hProc=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, pid);
    if (hProc==nullptr)
        return false;

    BOOL bRet = TerminateProcess(hProc, 0);
    CloseHandle(hProc);
    return bRet ? true : false;
}

bool LocalBroker::stopCmd(int pid)
{
    HANDLE hProc=OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, pid);
    if (hProc==nullptr)
        return false;

    LocalTerminateParams params(pid);
    EnumWindows((WNDENUMPROC)LocalTerminateAppEnum,(LPARAM)&params);

    // I believe we do not need this. It is ignored by console applications created with CREATE_NEW_PROCESS_GROUP
    GenerateConsoleCtrlEvent(CTRL_C_EVENT, pid);

    //send BREAK_EVENT because we created the process with CREATE_NEW_PROCESS_GROUP
    GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pid);

    CloseHandle(hProc);
    return true;
}

#else   //for UNIX

bool LocalBroker::psCmd(int pid)
{
    if(!pid)
        return false;
    return !::kill(pid, 0);
}


bool LocalBroker::killCmd(int pid)
{
    if(!pid)
        return false;
    return !::kill(pid, SIGKILL);
}


bool LocalBroker::stopCmd(int pid)
{
    if(!pid)
        return false;
   return !::kill(pid, SIGTERM);
}

int LocalBroker::waitPipe(int pipe_fd)
{
    struct timeval timeout;
    int rc;
    fd_set fd;

    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;

    FD_ZERO(&fd);
    FD_SET(pipe_fd, &fd);
    rc = select(pipe_fd + 1, &fd, nullptr, nullptr, &timeout);
    return rc;
}


int LocalBroker::waitPipeSignal(int pipe_fd)
{
    struct timespec timeout;
    fd_set fd;

    timeout.tv_sec = 2;
    timeout.tv_nsec = 0;
    FD_ZERO(&fd);
    FD_SET(pipe_fd, &fd);

    /*
#if (_POSIX_C_SOURCE >= 200112L) || (_XOPEN_SOURCE >= 600)
    struct sigaction new_action;
    new_action.sa_handler = SIG_IGN;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction (SIGUSR1, &new_action, nullptr);
    sigset_t sset, orgmask;
    sigemptyset(&sset);
    sigaddset(&sset, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &sset, &orgmask);
    if(pselect(pipe_fd + 1, &fd, nullptr, nullptr, &timeout, &orgmask))
        return PIPE_EVENT;
#endif
*/
    if(pselect(pipe_fd + 1, &fd, nullptr, nullptr, &timeout, nullptr))
        return PIPE_EVENT;
    return PIPE_TIMEOUT;
}


bool LocalBroker::startStdout()
{
    fd_stdout = fdopen(pipe_to_stdout[READ_FROM_PIPE], "r");
    if(!fd_stdout)
    {
        strError = "cannot open pipe. " + string(strerror(errno));
        //close(pipe_to_stdout[READ_FROM_PIPE]);
        return false;
    }

    int oflags = fcntl(pipe_to_stdout[READ_FROM_PIPE], F_GETFL);
    if(fcntl(pipe_to_stdout[READ_FROM_PIPE], F_SETFL, oflags|O_NONBLOCK) == -1)
    {
        strError = "cannot set flag on pipe: " + string(strerror(errno));
        //close(pipe_to_stdout[READ_FROM_PIPE]);
        return false;
    }
    Thread::start();
    return true;
}

void LocalBroker::stopStdout()
{
    Thread::stop();
    if(fd_stdout)
        fclose(fd_stdout);
    fd_stdout = nullptr;
}



int LocalBroker::ExecuteCmd()
{
    int  pipe_child_to_parent[2];
    int ret = pipe(pipe_child_to_parent);
    if (ret!=0)
    {
        strError = string("Can't create child pipe because") + string(strerror(errno));
        return 0;
    }

    ret = pipe(pipe_to_stdout);
    if (ret!=0)
    {
        strError = string("Can't create stdout pipe because") + string(strerror(errno));
        return 0;
    }

    int pid_cmd = fork();

    if(IS_INVALID(pid_cmd))
    {
        strError = string("Can't fork command because ") + string(strerror(errno));
        return 0;
    }

    if (IS_NEW_PROCESS(pid_cmd)) // RUN COMMAND HERE
    {
        close(pipe_child_to_parent[READ_FROM_PIPE]);
        //int saved_stderr = dup(STDERR_FILENO);
        dup2(pipe_to_stdout[WRITE_TO_PIPE], STDOUT_FILENO);
        dup2(pipe_to_stdout[WRITE_TO_PIPE], STDERR_FILENO);
        if (fcntl(STDOUT_FILENO, F_SETFL, fcntl(STDOUT_FILENO, F_GETFL) | O_NONBLOCK) == -1) {
            strError = string("Can't set flag on stdout: ") + string(strerror(errno));
            return 0;
        }
        if (fcntl(STDERR_FILENO, F_SETFL, fcntl(STDERR_FILENO, F_GETFL) | O_NONBLOCK) == -1) {
            strError = string("Can't set flag on stderr: ") + string(strerror(errno));
            return 0;
        }

        close(pipe_to_stdout[WRITE_TO_PIPE]);
        close(pipe_to_stdout[READ_FROM_PIPE]);

        strCmd = strCmd + string(" ") + strParam;
        char *szcmd = new char[strCmd.size()+1];
        strcpy(szcmd,strCmd.c_str());
        int nargs = 0;
        char **szarg = new char*[C_MAXARGS + 1];
        parseArguments(szcmd, &nargs, szarg);
        szarg[nargs]=nullptr;
        if(strEnv.size())
        {
            auto ss = yarp::conf::string::split(strEnv, ';');
            for (const auto& s : ss) {
                char* szenv = new char[s.size()+1];
                strcpy(szenv, s.c_str());
                putenv(szenv); // putenv doesn't make copy of the string
            }
            //delete szenv;
        }

        if(strWorkdir.size())
        {
            int ret = chdir(strWorkdir.c_str());
            if (ret!=0)
            {
                strError = string("Can't set working directory because ") + string(strerror(errno));
                FILE* out_to_parent = fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
                fprintf(out_to_parent,"%s", strError.c_str());
                fflush(out_to_parent);
                fclose(out_to_parent);
                close(pipe_child_to_parent[WRITE_TO_PIPE]);
                delete [] szcmd;
                delete [] szarg;
                std::exit(ret);
            }
        }

        char currWorkDirBuff[1024];
        char *currWorkDir = getcwd(currWorkDirBuff,1024);

        ret = 0;
        if (currWorkDir)
        {
            char **cwd_szarg=new char*[nargs+1];
            for (int i=1; i<nargs; ++i) cwd_szarg[i]=szarg[i];
            cwd_szarg[nargs]=nullptr;
            cwd_szarg[0]=new char[strlen(currWorkDir)+strlen(szarg[0])+16];

            strcpy(cwd_szarg[0],currWorkDir);
            strcat(cwd_szarg[0],"/");
            strcat(cwd_szarg[0],szarg[0]);
            ret=execvp(cwd_szarg[0],cwd_szarg);
            delete [] cwd_szarg[0];
            delete [] cwd_szarg;
        }

        if (ret==-1)
        {
            ret=execvp(szarg[0],szarg);
        }

        if (ret==-1)
        {
            strError = string("Can't execute command because ") + string(strerror(errno));
            FILE* out_to_parent = fdopen(pipe_child_to_parent[WRITE_TO_PIPE],"w");
            fprintf(out_to_parent,"%s", strError.c_str());
            fflush(out_to_parent);
            fclose(out_to_parent);
        }
        close(pipe_child_to_parent[WRITE_TO_PIPE]);
        delete [] szcmd;
        delete [] szarg;
        ::exit(ret);
    }

    if (IS_PARENT_OF(pid_cmd))
    {
        close(pipe_child_to_parent[WRITE_TO_PIPE]);
        FILE* in_from_child = fdopen(pipe_child_to_parent[READ_FROM_PIPE],"r");
        int flags=fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_GETFL,0);
        if (fcntl(pipe_child_to_parent[READ_FROM_PIPE],F_SETFL,flags|O_NONBLOCK) == -1)
        {
            strError = string("Can't set flag on pipe: ") + string(strerror(errno));
            fclose(in_from_child);
            return 0;
        }

        string retError;
        waitPipe(pipe_child_to_parent[READ_FROM_PIPE]);

        for (char buff[1024]; fgets(buff,1024,in_from_child);)
            retError += string(buff);
        fclose(in_from_child);

        if(retError.size())
        {
            strError = retError;
            close(pipe_child_to_parent[READ_FROM_PIPE]);
            return 0;
        }

        close(pipe_to_stdout[WRITE_TO_PIPE]);
        close(pipe_child_to_parent[READ_FROM_PIPE]);
        return pid_cmd;
    }

    return 0;
}

/**
 * Split a line into separate words.
 */
void LocalBroker::splitLine(char *pLine, char **pArgs)
{
     char *pTmp = strchr(pLine, ' ');

    if (pTmp) {
        *pTmp = '\0';
        pTmp++;
        while ((*pTmp) && (*pTmp == ' ')) {
            pTmp++;
        }
        if (*pTmp == '\0') {
            pTmp = nullptr;
        }
    }
    *pArgs = pTmp;
}



/**
 * Breaks up a line into multiple arguments.
 */
void LocalBroker::parseArguments(char *io_pLine, int *o_pArgc, char **o_pArgv)
{
    char *pNext = io_pLine;
    size_t i;
    int j;
    int quoted = 0;
    size_t len = strlen(io_pLine);

    // Protect spaces inside quotes, but lose the quotes
    for(i = 0; i < len; i++) {
        if ((!quoted) && ('"' == io_pLine[i])) {
            quoted = 1;
            io_pLine[i] = ' ';
        } else if ((quoted) && ('"' == io_pLine[i])) {
            quoted = 0;
            io_pLine[i] = ' ';
        } else if ((quoted) && (' ' == io_pLine[i])) {
            io_pLine[i] = '\1';
        }
    }

    // init
    memset(o_pArgv, 0x00, sizeof(char*) * C_MAXARGS);
    *o_pArgc = 1;
    o_pArgv[0] = io_pLine;

    while ((nullptr != pNext) && (*o_pArgc < C_MAXARGS)) {
        splitLine(pNext, &(o_pArgv[*o_pArgc]));
        pNext = o_pArgv[*o_pArgc];

        if (nullptr != o_pArgv[*o_pArgc]) {
            *o_pArgc += 1;
        }
    }

    for(j = 0; j < *o_pArgc; j++) {
        len = strlen(o_pArgv[j]);
        for(i = 0; i < len; i++) {
            if('\1' == o_pArgv[j][i]) {
                o_pArgv[j][i] = ' ';
            }
        }
    }
}
#endif
