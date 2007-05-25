/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGSTask.h
 *  Extents ACE_Task template
 * @version 1.0
 * @date 13-Jul-06 12:09:23 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 * */

/*
 * RCS-ID:$Id: DGSTask.h,v 1.1 2007-05-25 10:12:50 beltran Exp $
 */

#ifndef __DGSTASKH__
#define __DGSTASKH__

#include <ace/Task.h>

/** 
 * @class DGSTask
 *  Extents ACE_Task and implements some usefull commands for the
 * Dexterous Grasping System enviroment
 */
class DGSTask: public ACE_Task<ACE_MT_SYNCH>
{
public:

    /** 
     *  DGSTask Constructor
     */
    DGSTask (){};   

    /** 
     *  IsAlive Prints a Message signaling that is alive. 
     * As virtual function can be overwrited by descendant classes to
     * personalize their alive messages
     * @return 
     */
    virtual int IsAlive(){ACE_DEBUG((LM_INFO, ACE_TEXT("DGSTask: I am Alive!\n"))); return 0;};

protected:

private:

}; 

#endif /*__DGSTASKH__*/
