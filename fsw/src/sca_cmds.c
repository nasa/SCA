 /*************************************************************************
 ** File:
 **   $Id: sca_cmds.c 1.4 2015/02/20 17:04:22EST czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the functions to handle processing of ground 
 **   command requests and housekeeping requests
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_cmds.c  $ 
 **   Revision 1.4 2015/02/20 17:04:22EST czogby  
 **   Move sca_cmds.c function prototype into sca_cmds.h 
 **   Revision 1.3 2014/11/17 12:14:41EST czogby  
 **   --- Added comments ---  czogby [2014/11/17 20:06:39Z] 
 **   No changes. 
 **   Revision 1.2 2014/09/24 16:25:35EDT czogby  
 **   Updates from code inspection. 
 **   Revision 1.1 2014/08/26 15:38:24EDT czogby  
 **   Initial revision 
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj 
 *************************************************************************/


/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include "cfe.h"
#include "sca_app.h"
#include "sca_cmds.h"
#include "sca_child.h"
#include "sca_atsrq.h"
#include "sca_utils.h"
#include "sca_msgids.h"
#include "sca_events.h"
#include "sca_version.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Sends Housekeeping Data                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_SendHkPacket (void)
{
    /* send the status packet */
    CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &SCA_OperData.HkPacket);
    CFE_SB_SendMsg((CFE_SB_MsgPtr_t)&SCA_OperData.HkPacket);
    
} /* end SCA_SendHkPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset Counters Command                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void SCA_ResetCountersCmd (CFE_SB_MsgPtr_t CmdPacket)
{
    uint8 i = 0;

    SCA_ResetCntrsCmd_t *ResetCntrsCmd_Ptr = NULL;

    if (SCA_VerifyCmdLength(CmdPacket, sizeof(SCA_ResetCntrsCmd_t)))
    {
        ResetCntrsCmd_Ptr = ((SCA_ResetCntrsCmd_t *) CmdPacket);

        if(ResetCntrsCmd_Ptr->ResetFlag == SCA_ALL_COUNTERS)
        {
            SCA_OperData.HkPacket.CmdCtr = 0;
            SCA_OperData.HkPacket.CmdErrCtr = 0;

            for (i = SCA_CHILD_1; i < SCA_MAX_NUM_ATS; i++)
            {
                /* Protect data used by both the parent and children */
                OS_MutSemTake(SCA_OperData.HkMutex);

                SCA_OperData.HkPacket.AtsCmdCtr[i]    = 0;
                SCA_OperData.HkPacket.AtsCmdErrCtr[i] = 0;

                OS_MutSemGive(SCA_OperData.HkMutex);
            }
            CFE_EVS_SendEvent (SCA_RESET_ALL_COUNTERS_DBG_EID, CFE_EVS_DEBUG,
                               "Reset counters command: reset all counters");

        }
        else if(ResetCntrsCmd_Ptr->ResetFlag == SCA_CMD_COUNTERS_ONLY)
        {
            /* Protect data used by both the parent and children */
            OS_MutSemTake(SCA_OperData.HkMutex);

            SCA_OperData.HkPacket.CmdCtr = 0;
            SCA_OperData.HkPacket.CmdErrCtr = 0;

            OS_MutSemGive(SCA_OperData.HkMutex);

            CFE_EVS_SendEvent (SCA_RESET_CMD_COUNTERS_DBG_EID, CFE_EVS_DEBUG,
                               "Reset counters command: reset command counters");
        }
        else if(ResetCntrsCmd_Ptr->ResetFlag == SCA_ATS_COUNTERS_ONLY)
        {
            for (i = SCA_CHILD_1; i < SCA_MAX_NUM_ATS; i++)
            {
                /* Protect data used by both the parent and children */
                OS_MutSemTake(SCA_OperData.HkMutex);

                SCA_OperData.HkPacket.AtsCmdCtr[i]    = 0;
                SCA_OperData.HkPacket.AtsCmdErrCtr[i] = 0;

                OS_MutSemGive(SCA_OperData.HkMutex);
            }
            CFE_EVS_SendEvent (SCA_RESET_ATS_COUNTERS_DBG_EID, CFE_EVS_DEBUG,
                               "Reset counters command: reset ATS counters");
        }
        /* IMPROVEME: send an error message if ResetFlag has any other value. */
    }
} /* end SCA_ResetCountersCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* No Op Command                                                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_NoOpCmd(CFE_SB_MsgPtr_t CmdPacket)
{
    if (SCA_VerifyCmdLength(CmdPacket, sizeof(SCA_NoArgsCmd_t)))
    {
        SCA_OperData.HkPacket.CmdCtr++;
        CFE_EVS_SendEvent(SCA_NOOP_INFO_EID,
                          CFE_EVS_INFORMATION,
                          "No-op command. Version %d.%d.%d.%d",
                          SCA_MAJOR_VERSION,
                          SCA_MINOR_VERSION,
                          SCA_REVISION,
                          SCA_MISSION_REV);
    }     
}/* End SCA_NoOpCmd */
       

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Process Requests                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_ProcessRequest (CFE_SB_MsgPtr_t CmdPacket)
{      
    CFE_SB_MsgId_t  MessageID;

    MessageID = CFE_SB_GetMsgId (CmdPacket);

    switch (MessageID)
    {        
        case SCA_CMD_MID:
            /* request from the ground */
            SCA_ProcessCommand (CmdPacket);
            break;
            
        case SCA_SEND_HK_MID:
                         
            /* request from health and safety for housekeeping status */
            SCA_SendHkPacket();
            break;

        default:
            CFE_EVS_SendEvent (SCA_MID_ERR_EID,
                               CFE_EVS_ERROR,
                               "Invalid software bus pipe message ID: 0x%08X",
                               MessageID);
            break;
    } /* end switch */
} /* end SCA_ProcessRequest */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Process a command                                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_ProcessCommand (CFE_SB_MsgPtr_t CmdPacket)
{
    uint16              CommandCode;

    CommandCode = CFE_SB_GetCmdCode(CmdPacket);
        
    switch (CommandCode)
    {
        case SCA_NOOP_CC:
            SCA_NoOpCmd(CmdPacket);
            break;
            
        case SCA_RESET_COUNTERS_CC:
            SCA_ResetCountersCmd(CmdPacket);
            break;
            
        case SCA_START_ATS_CC:
            SCA_StartAtsCmd(CmdPacket);
            break;
            
        case SCA_STOP_ATS_CC:
            SCA_StopAtsCmd(CmdPacket);
            break;

        case SCA_GET_ATS_INFO_CC:
            SCA_GetATSInfo(CmdPacket);
            break;

        default:
            CFE_EVS_SendEvent (SCA_INVLD_CMD_ERR_EID,
                               CFE_EVS_ERROR,
                               "Invalid Command Code: MID =  0x%04X CC =  %d",
                                CFE_SB_GetMsgId (CmdPacket),
                               CommandCode);
            SCA_OperData.HkPacket.CmdErrCtr++;
            break;     
    } /* end switch */
} /* end SCA_ProcessCommand */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* SCA Verify the length of the command                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
boolean SCA_VerifyCmdLength(CFE_SB_MsgPtr_t msg, 
                            uint16          ExpectedLength)
{
    CFE_SB_MsgId_t MessageID;
    uint16  CommandCode;
    boolean Result = TRUE;
    uint16  ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /* Verify the command packet length */
    if (ExpectedLength !=  ActualLength)
    {
        CommandCode = CFE_SB_GetCmdCode(msg);
        MessageID =  CFE_SB_GetMsgId(msg);
     
        CFE_EVS_SendEvent(SCA_LEN_ERR_EID,
                          CFE_EVS_ERROR,
                          "Invalid msg length: ID = 0x%04X, CC = %d, Len = %d, Expected = %d",
                          MessageID,
                          CommandCode,
                          ActualLength,
                          ExpectedLength);
        Result = FALSE;
        SCA_OperData.HkPacket.CmdErrCtr++;
    }    
    return(Result);
} /* End of SCA_VerifyCmdLength */

/************************/
/*  End of File Comment */
/************************/

