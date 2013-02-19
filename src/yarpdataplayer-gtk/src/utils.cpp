/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Vadim Tikhanoff
 * email:  vadim.tikhanoff@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#if defined(WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#if defined(WIN32)
    #include "iCub/dirent.h"
    #undef max                  /*conflict with pango lib coverage.h*/
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #include <dirent.h>
    #include <cerrno>
    #define GetCurrentDir getcwd
#endif

#include <yarp/os/Time.h>
#include <stdio.h>              /* defines FILENAME_MAX */
#include "iCub/utils.h"
#include "iCub/main_window.h"
#include <iostream>

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

/**********************************************************/
Utilities::~Utilities()
{

}
/**********************************************************/
Utilities::Utilities(string name, bool _add_prefix)
{
    this->speed = 1.0;
    this->moduleName = name;
    add_prefix=_add_prefix;
    dir_count = 0;
    repeat = false;
    sendStrict = false;
    recursiveIterations = 0;
}
/**********************************************************/
string Utilities::getCurrentPath()
{
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath) ))// sizeof(TCHAR)))
        fprintf(stdout,"ERROR GETTING THE CURRENT DIR\n");
    //cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
    string currentPath = cCurrentPath;

    return currentPath;
}
/**********************************************************/
int Utilities::getRecSubDirList(string dir, vector<string> &names, vector<string> &info,
                                vector<string> &logs, vector<string> &paths, int recursive)
{
    struct dirent *direntp = NULL;
    DIR *dirp = NULL;
    size_t path_len;
    const char *path = dir.c_str();

    /* Check validity of directory */
    if (!path)
        return errno;
    /* Get length of the directory */
    path_len = strlen(path);
    /* Check length of the directory */
    if (!path || !path_len || (path_len > FILENAME_MAX))
        return errno;
    /* Check if file is opened */
    if((dirp  = opendir(path)) == NULL)
    {
        fprintf(stdout,"Error( %d opening %s\n",errno, dir.c_str());
        return errno;
    }
    /* read through */
    while ((direntp = readdir(dirp)) != NULL)
    {
        struct stat fstat;
        char full_name[FILENAME_MAX + 1];

        /* Calculate full name, check we are in file length limts */
        if ((path_len + strlen(direntp->d_name) + 1) > FILENAME_MAX)
            continue;

        strcpy(full_name, path);
        if (full_name[path_len - 1] != '/')
            strcat(full_name, "/");
        strcat(full_name, direntp->d_name);

        /* Ignore special directories. */
        if ((strcmp(direntp->d_name, ".") == 0) ||
            (strcmp(direntp->d_name, "..") == 0))
            continue;

        /*Ignore if not really a directory. */
        if (stat(full_name, &fstat) < 0)
            continue;
        if (S_ISDIR(fstat.st_mode))
        {
            string recDir = full_name;
            struct stat st;
            string fullName = string(dir + "/" + direntp->d_name + "/info.log");
            const char * filename = fullName.c_str();
            if(stat(filename,&st) == 0)
            {
                string dataFileName = string(dir + "/" + direntp->d_name + "/data.log");

                //check log file validity before proceeding
                if ( checkLogValidity( filename ) && (stat(dataFileName.c_str(),&st) == 0))
                {
                    fprintf(stdout," %s IS present adding it to the gui\n",filename);
                    
                    if (recursiveName.empty())
                        names.push_back( string( direntp->d_name) );//pass also previous subDir name
                    else
                        names.push_back( string(recursiveName + "_" + direntp->d_name) );//pass only subDir name

                    info.push_back( string(dir + "/" + direntp->d_name + "/info.log") );
                    logs.push_back( string(dir + "/" + direntp->d_name + "/data.log") );
                    paths.push_back( string(dir + "/" + direntp->d_name + "/") ); //pass full path
                    dir_count++;
                }else
                {
                    fprintf(stdout," %s IS present BUT corrupted not using file\n",filename);
                }
            }
            if (recursive)
            {
                recursiveIterations ++;
                if (recursiveIterations > 1)
                {
                    if (recursiveName.empty())
                        recursiveName = string( direntp->d_name );
                }
                else
                    recursiveName = string( direntp->d_name );
                
                getRecSubDirList(recDir, names, info, logs, paths, 1);
            }
            if (recursiveIterations < 2 || recursiveIterations > 2)
            {
                recursiveName.erase();
                recursiveIterations = 0;
            }
        }
    }
    /* close the dir */
    (void)closedir(dirp);
    //avoid this for alphabetical order in linux
    sort(names.begin(), names.end());
    sort(info.begin(), info.end());
    sort(logs.begin(), logs.end());
    sort(paths.begin(), paths.end());

    return dir_count;
}
/**********************************************************/
bool Utilities::checkLogValidity(const char *filename)
{
    bool check = false;
    fstream str; 
    str.open (filename);//, ios::binary);
    
    if (str.is_open())
    {   
        string line;
        int itr = 0;
        while( getline( str, line ) && itr < 3)
        {
            Bottle b( line.c_str() );
            if (itr >= 1)
            {
                if ( b.size() < 1)
                    check = false;                
                else
                    check = true;
            } 
            itr++;
        }    
        str.close();            
    }   
    return check;
}
/**********************************************************/
bool Utilities::setupDataFromParts(partsData &part)
{
    fstream str;

    // info part
    fprintf(stdout,"opening file %s\n", part.infoFile.c_str() );

    str.open (part.infoFile.c_str());//, ios::binary);
    if (str.is_open())
    {
        string line;
        int itr = 0;
        while( getline( str, line ) && (itr <= 1) )
        {
            Bottle b( line.c_str() );
            if (itr == 0)
            {
                part.type = b.get(1).toString().c_str();
                part.type.erase(part.type.size() -1 );      // remove the ";" character
            }
            if (itr == 1)
            {
                part.portName = b.get(1).toString().c_str();
                fprintf(stdout, "the name of the port is %s\n",part.portName.c_str());
            }

            itr++;
        }
        str.close();
    }
    else
        return false;

    // data part
    fprintf(stdout,"opening file %s\n", part.logFile.c_str() );
    str.open (part.logFile.c_str());//, ios::binary);

    //read throughout
    if (str.is_open())
    {
        string line;
        int itr = 0;
        while( getline( str, line ) )
        {
            Bottle b( line.c_str() );
            part.bot.addList() = b;
            part.timestamp.push_back( b.get(1).asDouble() );
            itr++;
        }
        allTimeStamps.push_back( part.timestamp[1] );   //save all first timeStamps dumped for later ease of use
        part.maxFrame= itr-1;                           //set max frame to the total iteration minus first line type;
        part.currFrame = 1;                             //initialize current frame to 0
        str.close();                                    //close the file
    }
    else
        return false;

    return true;
}
/**********************************************************/
void Utilities::getMaxTimeStamp()
{
    maxTimeStamp = 0.0;
    int index = 0;
    for (int i=0; i< (int)allTimeStamps.size(); i++ ){
        fprintf(stdout,"%lf\n",allTimeStamps[i]);
        if ( maxTimeStamp < allTimeStamps[i] )
        {
            maxTimeStamp = allTimeStamps[i];
            index = i;
        }
    }
    fprintf(stdout,"the biggest timestamp is: index %d with value %lf\n",index, allTimeStamps[index] );
}
/**********************************************************/
int Utilities::amendPartFrames(partsData &part)
{
    while (part.timestamp[part.currFrame] < maxTimeStamp)
        part.currFrame++;
    fprintf(stdout,"the first frame of part %s is %d\n",part.name.c_str(), part.currFrame);
    return part.currFrame;
}
/**********************************************************/
void Utilities::resetMaxTimeStamp()
{
    allTimeStamps.clear();
    maxTimeStamp = 0.0;
}
/**********************************************************/
void Utilities::resetDirCount()
{
    dir_count = 0;
}
/**********************************************************/
bool Utilities::configurePorts(partsData &part)
{
    string tmp_port_name=part.portName;
    if(add_prefix)
        tmp_port_name="/"+moduleName+tmp_port_name;

    if (strcmp (part.type.c_str(),"Bottle") == 0)
    {
        if ( !yarp::os::Network::exists(tmp_port_name.c_str()) )
        {
            fprintf(stdout,"need to create new port %s for %s\n",part.type.c_str(), part.name.c_str());
            part.bottlePort.close();
            fprintf(stdout,"creating and opening %s port for part %s\n",part.type.c_str(), part.name.c_str());
            part.bottlePort.open(tmp_port_name.c_str());
        }
    }
    else if (strcmp (part.type.c_str(),"Image:ppm") == 0)
    {
        if ( !yarp::os::Network::exists(tmp_port_name.c_str()) )
        {
            fprintf(stdout,"need to create new port %s for %s\n",part.type.c_str(), part.name.c_str());
            part.imagePort.close();
            fprintf(stdout,"creating and opening image port for part %s\n",part.name.c_str());
            part.imagePort.open(tmp_port_name.c_str());
        }
    }
    else
    {
        fprintf(stdout,"Something is wrong with the data...%s\nIt seems its missing a type \n",part.name.c_str());
        return false;
    }
    return true;
}
/**********************************************************/
bool Utilities::interruptPorts(partsData &part)
{
    if (strcmp (part.type.c_str(),"Bottle") == 0)
        part.bottlePort.interrupt();
    else if (strcmp (part.type.c_str(),"Image:ppm") == 0)
        part.imagePort.interrupt();
    else
        return false;
    return true;
}
/**********************************************************/
bool Utilities::closePorts(partsData &part)
{
    if (strcmp (part.type.c_str(),"Bottle") == 0)
        part.bottlePort.close();
    else if (strcmp (part.type.c_str(),"Image:ppm") == 0)
        part.imagePort.close();
    else
        return false;
    return true;
}

/**********************************************************/
void Utilities::stopAtEnd()
{
    fprintf(stdout, "asking utils to stop the thread \n");
    masterThread->askToStop();
    for (int i=0; i < totalThreads; i++)
        partDetails[i].currFrame = (int)initialFrame[i];

    masterThread->wnd->resetButtonOnStop();
    fprintf(stdout, "ok................ \n");
}
/**********************************************************/
void Utilities::stepThread()
{
    masterThread->stepfromCmd = true;
    fprintf(stdout, "asking utils to step the thread \n");
    if ( masterThread->isRunning() )
    {
        masterThread->resume();
    }
    else
    {
        for (int i=0; i < totalThreads; i++)
            partDetails[i].worker->init();
        masterThread->start();
    }
    fprintf(stdout, "ok................ \n");
}
/**********************************************************/
void Utilities::pauseThread()
{   fprintf(stdout,"asking the thread to pause\n");
    masterThread->pause();
    fprintf(stdout, "ok................ \n");
    masterThread->stepfromCmd = false;
}



