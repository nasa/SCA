/*************************************************************************
 ** File:
 **   $Id: sca_app.c 1.6 2015/03/10 12:46:25EDT czogby Exp  $
 **
 ** Purpose: 
 **     This file contains the Stored Command Absolute main event loop function. It also
 **     contains the initialization function. The SCA app handles the scheduling
 **     of stored commands with absolute times for the fsw.  The main event loop handles the Software
 **     Bus interface.  It is an alternate version of the SC app, created with the following differences.
 **     1.) Removed relative command timing (only use absolute)
 **     2.) Removed tables (reads commands from a text file)
 **     3.) Allows the use of more than two independent command sequences.
 **     4.) Added a flag to force a command to be executed even if filesystem delays cause it to miss its time, if within a given timeout period.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 ** History:
 **   $Log: sca_app.c  $
 **   Revision 1.6 2015/03/10 12:46:25EDT czogby 
 **   In function SCA_AppInit, 4th parameter of call to function CFE_SB_InitMsg changed to TRUE
 **   Revision 1.5 2015/02/20 19:11:05EST czogby 
 **   Move sca_app.c function prototypes into sca_app.h
 **   Revision 1.4 2015/02/20 19:03:36EST czogby 
 **   Add newline character to all system log entries.
 **   Revision 1.3 2014/11/17 12:14:05EST czogby 
 **   --- Added comments ---  czogby [2014/11/17 20:04:44Z]
 **   Split SCA_CreateMutexes into two prodecures - SCA_CreateMutexes and SCA_CreateSemaphores.
 **   Revision 1.2 2014/09/24 16:25:35EDT czogby 
 **   Updates from code inspection.
 **   Revision 1.1 2014/08/26 15:38:22EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
 *************************************************************************/

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include <string.h>
#include "cfe.h"
#include "sca_app.h"
#include "sca_cmds.h"
#include "sca_child.h"
#include "sca_events.h"
#include "sca_msgids.h"
#include "sca_perfids.h"
#include "sca_version.h"
#include "sca_verify.h"

/**************************************************************************
 **
 ** Global variables
 **
 **************************************************************************/

SCA_OperData_t SCA_OperData;        /* SCA Operational Data */

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  SCA main process loop (task entry point)                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SCA_AppMain(void)
{   
    uint32 RunStatus = CFE_ES_APP_RUN;
    int32 Result;

    /* Register application with cFE */
    Result = CFE_ES_RegisterApp();

    /* Performance Log (start time counter) */
    CFE_ES_PerfLogEntry(SCA_APPMAIN_PERF_ID);

    /* Startup initialization */
    if (Result == CFE_SUCCESS)
    {
        Result = SCA_AppInit();    
    }

    /* Check for start-up error */
    if (Result != CFE_SUCCESS)
    {
        /* Set request to terminate main loop */
        RunStatus = CFE_ES_APP_ERROR;
    }

    /* Main process loop */
    while (CFE_ES_RunLoop(&RunStatus))
    {
        /* Performance Log (stop time counter) */
        CFE_ES_PerfLogExit(SCA_APPMAIN_PERF_ID);

        /* Pend on Software Bus for message */
        Result = CFE_SB_RcvMsg(&SCA_OperData.MsgPtr, SCA_OperData.CmdPipe, CFE_SB_PEND_FOREVER);

        /* Performance Log (start time counter) */
        CFE_ES_PerfLogEntry(SCA_APPMAIN_PERF_ID);

        /* Check for Software Bus error */
        if (Result == CFE_SUCCESS)
        {
            /* Invoke command handlers */
            SCA_ProcessRequest(SCA_OperData.MsgPtr);
        }
        else
        {
            /* Exit main process loop */
            RunStatus = CFE_ES_APP_ERROR;
        }
    }
    
    /* Check for "fatal" process error */
    if (Result !=  CFE_SUCCESS)
    {
        /* Send event describing reason for termination */
        CFE_EVS_SendEvent(SCA_APP_EXIT_ERR_EID, CFE_EVS_ERROR,
           "App terminating, Result = 0x%08X", Result);

        /* In case cFE Event Services is not working */
        CFE_ES_WriteToSysLog("SCA App terminating, Result = 0x%08X\n", Result);
    }

    /* Performance Log (stop time counter) */
    CFE_ES_PerfLogExit(SCA_APPMAIN_PERF_ID);
    
    /* Let cFE kill the app */
    CFE_ES_ExitApp(RunStatus);
    
} /* end SCA_AppMain() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Application startup initialization                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 SCA_AppInit(void)
{
    uint8 i      = 0;
    int32 Result = CFE_SUCCESS;

    /* Must be able to register for events */
    Result = CFE_EVS_Register(NULL,0,CFE_EVS_NO_FILTER);
    if (Result !=  CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Event Services Register returned: 0x%08X\n", Result);
        return(Result);
    }

    /* Clear global data structures */
    CFE_PSP_MemSet(&SCA_OperData, 0, sizeof(SCA_OperData_t));

    /* Initialize the SCA housekeeping packet */
    CFE_SB_InitMsg(&SCA_OperData.HkPacket, SCA_HK_TLM_MID, sizeof(SCA_HkTlm_t), TRUE);
    SCA_OperData.HkPacket.LastAtsCmdErrFile[0] = '\0';

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_2].ChildName, SCA_CHILD_2_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_3].ChildName, SCA_CHILD_3_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_4].ChildName, SCA_CHILD_4_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_5].ChildName, SCA_CHILD_5_NAME, SCA_CHILD_NAME_SIZE);

    for (i = SCA_CHILD_1; i < SCA_MAX_NUM_ATS; i++)
    {
        SCA_OperData.ChildInfo[i].StopATS = FALSE;
    }

    Result = SCA_CreateMutexes();

    if (Result != OS_SUCCESS)
    {
        return(Result);
    }

    Result = SCA_CreateSemaphores();

    if (Result != OS_SUCCESS)
    {
        return(Result);
    }

    /* Must be able to create Software Bus message pipe */
    Result = CFE_SB_CreatePipe(&SCA_OperData.CmdPipe, SCA_PIPE_DEPTH, SCA_CMD_PIPE_NAME);
    if (Result !=  CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SCA_INIT_SB_CREATE_ERR_EID, CFE_EVS_ERROR,
                         "Software Bus Create Pipe returned: 0x%08X", Result);
        return(Result);
    }
    
    /* Must be able to subscribe to HK request command */
    Result = CFE_SB_Subscribe(SCA_SEND_HK_MID, SCA_OperData.CmdPipe);
    if (Result !=  CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SCA_INIT_SB_SUBSCRIBE_HK_ERR_EID, CFE_EVS_ERROR,
                         "Software Bus subscribe to housekeeping returned: 0x%08X", Result);
        return(Result);
    }

    /* Must be able to subscribe to SCA commands */
    Result = CFE_SB_Subscribe(SCA_CMD_MID, SCA_OperData.CmdPipe);
    if (Result !=  CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(SCA_INIT_SB_SUBSCRIBE_CMD_ERR_EID, CFE_EVS_ERROR,
                         "Software Bus subscribe to command returned: 0x%08X", Result);
        return(Result);
    }

    /* Send application startup event */
    CFE_EVS_SendEvent(SCA_INIT_INFO_EID, CFE_EVS_INFORMATION, 
                     "SCA Initialized. Version %d.%d.%d.%d",
                      SCA_MAJOR_VERSION, SCA_MINOR_VERSION, SCA_REVISION, SCA_MISSION_REV);

    return(CFE_SUCCESS);

} /* end SCA_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Create SCA child data mutexes                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 SCA_CreateMutexes (void)
{
    int32 Result = OS_SUCCESS;
    uint8 i      = 0;
    char ChildMutexName[SCA_CHILD_MUTEX_NAME_LENGTH + 1];

    /* 
    ** Create mutex to protect HK data used by both the parent and children: 
    ** CmdErrCtr, CmdCtr, AtsCmdCtr, AtsCmdErrCtr, LastAtsCmdErrSeqNum, and LastAtsCmdErrFile
    */
    Result = OS_MutSemCreate(&SCA_OperData.HkMutex, SCA_HK_MUTEX_NAME, SCA_HK_MUTEX_OPTIONS);

    if(Result != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("SCA Create HK Mutex error: returned 0x%08X\n", Result);
        return(Result);
    }

    CFE_PSP_MemSet (ChildMutexName, '\0', SCA_CHILD_MUTEX_NAME_LENGTH + 1);

    /*
    ** Create mutexes to protect child data used by both the parent and the children: StopATS and RunStatus.
    ** Also creating mutexes for items in the *output copy* of the child data used by both the parent and the children:
    ** ChildAtsFile, ChildName, ChildTaskID, CommandStatus, CommandSequenceNumber, ForceFlag, TimeoutPeriod, TimeNextCmd, 
    ** CurrentTime_Last, and RunStatus.
    **
    */
    for (i = SCA_CHILD_1; i < SCA_MAX_NUM_ATS; i++)
    {
        strncpy (ChildMutexName, SCA_CHILD_MUTEX_BASE_NAME, SCA_CHILD_MUTEX_NAME_LENGTH - 1);
        sprintf (ChildMutexName, "%s%d", ChildMutexName, i+1);

        Result = OS_MutSemCreate(&SCA_OperData.ChildInfo[i].DataMutex, ChildMutexName, SCA_CHILD_MUTEX_OPTIONS);
    
        if(Result != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("SCA Create Child Mutex %d error: returned 0x%08X\n", i+1, Result);
            return(Result);
        }

        CFE_PSP_MemSet (ChildMutexName, '\0', SCA_CHILD_MUTEX_NAME_LENGTH + 1);

        strncpy (ChildMutexName, SCA_CHILD_COPY_MUTEX_BASE_NAME, SCA_CHILD_MUTEX_NAME_LENGTH - 1);
        sprintf (ChildMutexName, "%s%d", ChildMutexName, i+1);

        Result = OS_MutSemCreate(&SCA_OperData.ChildInfo[i].OutputDataMutex, ChildMutexName, SCA_CHILD_MUTEX_OPTIONS);

        if(Result != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("SCA Create Child Info Copy Mutex %d error: returned 0x%08X\n", i+1, Result);
            return(Result);
        }

        CFE_PSP_MemSet (ChildMutexName, '\0', SCA_CHILD_MUTEX_NAME_LENGTH + 1);
    }

    return OS_SUCCESS;
} /* end SCA_CreateMutexes() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Create and take SCA child semaphores used to create             */
/* delays when waiting for the right time to send a command        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 SCA_CreateSemaphores (void)
{
    int32 Result = OS_SUCCESS;
    uint8 i      = 0;
    char ChildPendingCmdSemaphoreName[SCA_CHILD_PENDING_CMD_SEMAPHORE_NAME_LENGTH + 1];

    for (i = SCA_CHILD_1; i < SCA_MAX_NUM_ATS; i++)
    {
        strncpy (ChildPendingCmdSemaphoreName, SCA_CHILD_PENDING_CMD_SEMAPHORE_BASE_NAME, SCA_CHILD_PENDING_CMD_SEMAPHORE_NAME_LENGTH - 1);
        sprintf (ChildPendingCmdSemaphoreName, "%s%d", ChildPendingCmdSemaphoreName, i+1);

        Result = OS_BinSemCreate(&SCA_OperData.ChildInfo[i].PendingCmdSemaphore, ChildPendingCmdSemaphoreName, 1, SCA_CHILD_MUTEX_OPTIONS);

        if(Result != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("SCA Create Child Pending Command Semaphore %d error: returned 0x%08X\n", i+1, Result);
            return(Result);
        }

        OS_BinSemTake(SCA_OperData.ChildInfo[i].PendingCmdSemaphore);
    }

    return OS_SUCCESS;
} /* end SCA_CreateSemaphores() */

/************************/
/*  End of File Comment */
/************************/
