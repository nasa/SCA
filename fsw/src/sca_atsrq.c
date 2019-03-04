 /*************************************************************************
 ** File:
 **   $Id: sca_atsrq.c 1.3 2014/11/17 12:14:18EST czogby Exp  $
 **
 ** Purpose: 
 **     This file contains functions to handle all of the ATS
 **     executive requests and internal reuqests to control
 **     the ATP and ATSs.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_atsrq.c  $
 **   Revision 1.3 2014/11/17 12:14:18EST czogby 
 **   --- Added comments ---  czogby [2014/11/17 20:03:00Z]
 **   Added call to OS_BinSemFlush as part of algorithm to compensate for time drift when waiting to send a command.
 **   Revision 1.2 2014/09/24 16:25:33EDT czogby 
 **   Updates from code inspection.
 **   Revision 1.1 2014/08/26 15:38:23EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
 
 *************************************************************************/
 
 

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include <stdio.h>
#include <string.h>
#include "cfe.h"
#include "sca_atsrq.h"
#include "sca_utils.h"
#include "sca_events.h"
#include "sca_app.h"
#include "sca_child.h"
#include "sca_cmds.h"
#include "sca_msg.h"

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Start an ATS                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_StartAtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint8              i                = 0;
    int32              OS_Status        = CFE_SUCCESS;
    SCA_StartAtsCmd_t  *StartAtsCmd_Ptr = NULL;
    uint8              AtsIndex         = 0;

    if (SCA_VerifyCmdLength(CmdPacket, sizeof(SCA_StartAtsCmd_t)))
    {
	StartAtsCmd_Ptr = ((SCA_StartAtsCmd_t *) CmdPacket);

	AtsIndex = SCA_FindFreeChild();

	if (AtsIndex != SCA_NO_FREE_CHILD)
        {
            /* Verify that the specified ATS file is not already running. */
            for (i = SCA_CHILD_1; i < SCA_MAX_NUM_ATS; i++)
            {
                /* Protect data used by both the parent and children */
                OS_MutSemTake(SCA_OperData.ChildInfo[i].DataMutex);

                if (strncmp(SCA_OperData.ChildInfo[i].ChildAtsFile, StartAtsCmd_Ptr->AtsFilename, OS_MAX_PATH_LEN) == 0)
                {
                    if (SCA_OperData.ChildInfo[i].RunStatus == SCA_CHILD_EXECUTING)
                    {
                        SCA_OperData.HkPacket.CmdErrCtr++;

                        CFE_EVS_SendEvent( SCA_ATS_START_ATS_IN_USE_ERR_EID, CFE_EVS_ERROR,
                                           "ATS %s already in use",
                                            StartAtsCmd_Ptr->AtsFilename);

                        OS_MutSemGive(SCA_OperData.ChildInfo[i].DataMutex);
                        return;
                    }
                }

                OS_MutSemGive(SCA_OperData.ChildInfo[i].DataMutex);
            }

            /*
            ** This line must be before SCA_CreateChild.  If it were instead set later, there would be a race condition 
            ** between that and when it's read in SCA_ProcessFile.
            */
            strncpy (SCA_OperData.ChildInfo[AtsIndex].ChildAtsFile, StartAtsCmd_Ptr->AtsFilename, OS_MAX_PATH_LEN);

            /* 
            ** These need to be set here - if they aren't set until ProcessFile 
            ** then trying to start 2 ATSs in the same second from an ATS fails due to a race condition.
            */
            SCA_OperData.ChildInfo[AtsIndex].CommandStatus         = SCA_NON_COMMAND;
            SCA_OperData.ChildInfo[AtsIndex].CommandSequenceNumber = 1;
            SCA_OperData.ChildInfo[AtsIndex].RunStatus             = SCA_CHILD_EXECUTING;

            OS_Status = SCA_CreateChild(AtsIndex);

            if( OS_Status == CFE_SUCCESS )
            {
                CFE_EVS_SendEvent( SCA_STARTATS_CMD_CREAT_CHILD_INFO_EID, CFE_EVS_DEBUG,
                                   "Started ATS Child Task: %s",
                                   SCA_OperData.ChildInfo[AtsIndex].ChildName );
            }
            else
            {
                SCA_OperData.HkPacket.CmdErrCtr++;

                CFE_EVS_SendEvent( SCA_STARTATS_CMD_CREAT_CHILD_ERR_EID, CFE_EVS_ERROR,
                                   "Start ATS Rejected: Child Task Creation Failure: 0x%x, %s",
                                   OS_Status, SCA_OperData.ChildInfo[AtsIndex].ChildName );

                return;
            }
        }
        else
        {
            SCA_OperData.HkPacket.CmdErrCtr++;

            CFE_EVS_SendEvent( SCA_STARTATS_CMD_MAX_ATS_ERR_EID, CFE_EVS_ERROR,
                               "Start ATS Rejected: Max number of ATS executing");

            return;
        }
    }
} /* end SCA_StartAtsCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*   Stop the currently executing ATS                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_StopAtsCmd (CFE_SB_MsgPtr_t CmdPacket)
{   
    uint8   i                         = 0;
    SCA_StopAtsCmd_t  *StopAtsCmd_Ptr = NULL;
    boolean ATS_Found                 = FALSE;

    if (SCA_VerifyCmdLength(CmdPacket, sizeof(SCA_StopAtsCmd_t)))
    {
        StopAtsCmd_Ptr = ((SCA_StopAtsCmd_t *) CmdPacket);

        for (i = SCA_CHILD_1; i < SCA_MAX_NUM_ATS; i++)
        {
            /* Protect data used by both the parent and children */
            OS_MutSemTake(SCA_OperData.ChildInfo[i].DataMutex);

            if (strncmp(SCA_OperData.ChildInfo[i].ChildAtsFile, StopAtsCmd_Ptr->AtsFilename, OS_MAX_PATH_LEN) == 0)
            {
                if (SCA_OperData.ChildInfo[i].RunStatus == SCA_CHILD_EXECUTING)
                {
                    SCA_OperData.ChildInfo[i].StopATS = TRUE;
                    ATS_Found                         = TRUE;
                    OS_BinSemFlush(SCA_OperData.ChildInfo[i].PendingCmdSemaphore);
                }
            }

            OS_MutSemGive(SCA_OperData.ChildInfo[i].DataMutex);
        }

        if (ATS_Found == FALSE)
        {
            CFE_EVS_SendEvent( SCA_STOPATS_NOTRUNNING_INFO_EID, CFE_EVS_INFORMATION,
                               "ATS %s is not running",
                               StopAtsCmd_Ptr->AtsFilename);
        }
        /* ATS_Found == TRUE is handled in the ATS child functions, after the ATS is successfully stopped. */

        SCA_OperData.HkPacket.CmdCtr++;
    } /* end if */

} /* end SCA_StopAtsCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get ATS Info                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_GetATSInfo(CFE_SB_MsgPtr_t CmdPacket)
{
    SCA_GetATSInfoCmd_t    *GetATSInfo_Ptr     = NULL;
    int32   FileDescriptor                     = 0;
    uint8   i                                  = 0;
    int32   WriteStatus                        = 0;
    int32   CloseStatus                        = OS_SUCCESS;
    int32   TotalBytesWritten                  = 0;
    boolean FileCreated                        = FALSE;
    char    OutputFilename[OS_MAX_PATH_LEN];

    GetATSInfo_Ptr = ((SCA_GetATSInfoCmd_t *) CmdPacket);

    for (i = SCA_CHILD_1; i < SCA_MAX_NUM_ATS; i++)
    {
        if (SCA_OperData.ChildInfoOutput[i].RunStatus == SCA_CHILD_EXECUTING)
        {
            if (FileCreated == FALSE)
            {
                if (CFE_SB_GetTotalMsgLength(CmdPacket) == sizeof(SCA_GetATSInfoCmd_t))
                {
                    strncpy (OutputFilename, GetATSInfo_Ptr->OutputFilename, OS_MAX_PATH_LEN);
                }
                else
                {
                    /* If the packet length is incorrect it means the filename parameter is missing, so a default filename is used instead. */
                    strncpy (OutputFilename, SCA_DEFAULT_ATSINFO_FILENAME, OS_MAX_PATH_LEN);
                }

                FileDescriptor = OS_creat(OutputFilename, OS_WRITE_ONLY);

                if ( FileDescriptor < OS_FS_SUCCESS )
                {
                    CFE_EVS_SendEvent( SCA_ATSINFO_CREATFILE_ERR_EID, CFE_EVS_ERROR,
                                       "Get ATS Info: Failed to create file %s: err = 0x%x",
                                       OutputFilename,
                                       FileDescriptor);

                    SCA_OperData.HkPacket.CmdErrCtr++;
                    return;
                }
                else
                {
                    FileCreated = TRUE;
                }      
            } /* end if */

            /* Protect data used by both the parent and children */
            OS_MutSemTake(SCA_OperData.ChildInfo[i].OutputDataMutex);

            WriteStatus = OS_write(FileDescriptor, &SCA_OperData.ChildInfoOutput[i], sizeof (SCA_ChildInfoOutput_t));

            if ( WriteStatus < OS_FS_SUCCESS )
            {
                CFE_EVS_SendEvent( SCA_ATSINFO_WRITEFILE_ERR_EID, CFE_EVS_ERROR,
                                   "Get ATS Info Rejected: Write File Error: 0x%x, %s, %s",
                                   WriteStatus,
                                   SCA_OperData.ChildInfoOutput[i].ChildName,
                                   OutputFilename );

                SCA_OperData.HkPacket.CmdErrCtr++;

                OS_MutSemGive(SCA_OperData.ChildInfo[i].OutputDataMutex);
                return;
            }
            else
            {
                TotalBytesWritten = TotalBytesWritten + WriteStatus;
            }
        } /* end if */
        OS_MutSemGive(SCA_OperData.ChildInfo[i].OutputDataMutex);
    } /* end for loop */

    if (FileCreated)
    {
        CFE_EVS_SendEvent( SCA_ATSINFO_WRITTEN_EID, CFE_EVS_INFORMATION,
                           "Get ATS Info: %i Bytes written to %s",
                           TotalBytesWritten,
                           OutputFilename);

        if ((CloseStatus = OS_close(FileDescriptor)) != OS_FS_SUCCESS)
        {
            CFE_EVS_SendEvent( SCA_ATSINFO_CLOSE_FILE_CRITICAL_EID, CFE_EVS_CRITICAL,
                               "Get ATS Info: Error closing file %s, err = 0x%x",
                               OutputFilename,
                               CloseStatus);

            SCA_OperData.HkPacket.CmdErrCtr++;
            return;
        }

    }
    else
    {
        CFE_EVS_SendEvent( SCA_ATSINFO_NOFILE_EID, CFE_EVS_INFORMATION,
                           "Get ATS Info: No Running ATS, data file not created");
    }

    SCA_OperData.HkPacket.CmdCtr++;
} /* End SCA_GetATSInfo */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Create child process for an ATS                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 SCA_CreateChild(uint8 AtsIndex)
{
    int32 OS_Status = CFE_SUCCESS;

    switch (AtsIndex)
    {
        case SCA_CHILD_1:
            OS_Status = CFE_ES_CreateChildTask( &SCA_OperData.ChildInfo[SCA_CHILD_1].ChildTaskID,
                                                SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName,
                                                SCA_Child1Main,
                                                SCA_CHILD_STACK_PTR,
                                                SCA_ATS_CHILD_STACK_SIZE,
                                                SCA_ATS_CHILD_TASK_PRIORITY,
                                                SCA_CHILD_FLAGS );
            break;

        case SCA_CHILD_2:
            OS_Status = CFE_ES_CreateChildTask( &SCA_OperData.ChildInfo[SCA_CHILD_2].ChildTaskID,
                                                SCA_OperData.ChildInfo[SCA_CHILD_2].ChildName,
                                                SCA_Child2Main,
                                                SCA_CHILD_STACK_PTR,
                                                SCA_ATS_CHILD_STACK_SIZE,
                                                SCA_ATS_CHILD_TASK_PRIORITY,
                                                SCA_CHILD_FLAGS );
            break;

        case SCA_CHILD_3:
            OS_Status = CFE_ES_CreateChildTask( &SCA_OperData.ChildInfo[SCA_CHILD_3].ChildTaskID,
                                                SCA_OperData.ChildInfo[SCA_CHILD_3].ChildName,
                                                SCA_Child3Main,
                                                SCA_CHILD_STACK_PTR,
                                                SCA_ATS_CHILD_STACK_SIZE,
                                                SCA_ATS_CHILD_TASK_PRIORITY,
                                                SCA_CHILD_FLAGS );
            break;

        case SCA_CHILD_4:
            OS_Status = CFE_ES_CreateChildTask( &SCA_OperData.ChildInfo[SCA_CHILD_4].ChildTaskID,
                                                SCA_OperData.ChildInfo[SCA_CHILD_4].ChildName,
                                                SCA_Child4Main,
                                                SCA_CHILD_STACK_PTR,
                                                SCA_ATS_CHILD_STACK_SIZE,
                                                SCA_ATS_CHILD_TASK_PRIORITY,
                                                SCA_CHILD_FLAGS );
            break;

        case SCA_CHILD_5:
            OS_Status = CFE_ES_CreateChildTask( &SCA_OperData.ChildInfo[SCA_CHILD_5].ChildTaskID,
                                                SCA_OperData.ChildInfo[SCA_CHILD_5].ChildName,
                                                SCA_Child5Main,
                                                SCA_CHILD_STACK_PTR,
                                                SCA_ATS_CHILD_STACK_SIZE,
                                                SCA_ATS_CHILD_TASK_PRIORITY,
                                                SCA_CHILD_FLAGS );
            break;

        default:
            /* This case can't be reached. Handling it anyway, just in case. Error event message is sent from the calling function. */
            OS_Status = CFE_ES_ERR_CHILD_TASK_CREATE;
            break;

    }  /* end switch */

    return OS_Status;

} /* End SCA_CreateChild */

/************************/
/*  End of File Comment */
/************************/
