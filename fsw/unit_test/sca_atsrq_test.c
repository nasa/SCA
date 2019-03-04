 /*************************************************************************
 ** File:
 **   $Id: sca_atsrq_test.c 1.1 2015/03/10 13:08:57EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file sca_atsrq.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_atsrq_test.c  $
 **   Revision 1.1 2015/03/10 13:08:57EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "sca_atsrq_test.h"
#include "sca_events.h"
#include "sca_msgids.h"
#include "sca_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include <time.h>

#define UT_CFE_SB_MSGLENGTH  72

extern SCA_OperData_t SCA_OperData;

/*
 * Function Definitions
 */

void SCA_StartAtsCmd_Test_Nominal(void)
{
    char              Filename1[OS_MAX_PATH_LEN];
    char              Filename2[OS_MAX_PATH_LEN];
    char              Filename3[OS_MAX_PATH_LEN];
    char              Filename4[OS_MAX_PATH_LEN];
    char              Filename5[OS_MAX_PATH_LEN];
    SCA_StartAtsCmd_t CmdPacket;

    strncpy (Filename1, "/ram/ats1.txt", OS_MAX_PATH_LEN);
    strncpy (Filename2, "/ram/ats2.txt", OS_MAX_PATH_LEN);
    strncpy (Filename3, "/ram/ats3.txt", OS_MAX_PATH_LEN);
    strncpy (Filename4, "/ram/ats4.txt", OS_MAX_PATH_LEN);
    strncpy (Filename5, "/ram/ats5.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_2].ChildName, SCA_CHILD_2_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_3].ChildName, SCA_CHILD_3_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_4].ChildName, SCA_CHILD_4_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_5].ChildName, SCA_CHILD_5_NAME, SCA_CHILD_NAME_SIZE);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);

    /* Start SCA_CHILD_1 */

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_START_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename1, OS_MAX_PATH_LEN);

    /* Execute the function being tested (Start ATS ats1.txt) */
    SCA_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_MemCmp(SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename1, OS_MAX_PATH_LEN, "ChildInfo[SCA_CHILD_1].ChildAtsFile == Filename1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "CommandStatus == SCA_NON_COMMAND");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandSequenceNumber == 1, "CommandSequenceNumber == 1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus == SCA_CHILD_EXECUTING, "RunStatus == SCA_CHILD_EXECUTING");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_CREAT_CHILD_INFO_EID, CFE_EVS_DEBUG, "Started ATS Child Task: SCA_CHILD1"),
        "Started ATS Child Task: SCA_CHILD1");

    /* Start SCA_CHILD_2 */

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_START_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename2, OS_MAX_PATH_LEN);

    /* Execute the function being tested (Start ATS ats2.txt) */
    SCA_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_MemCmp(SCA_OperData.ChildInfo[SCA_CHILD_2].ChildAtsFile, Filename2, OS_MAX_PATH_LEN, "ChildInfo[SCA_CHILD_2].ChildAtsFile == Filename2");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].CommandStatus == SCA_NON_COMMAND, "CommandStatus == SCA_NON_COMMAND");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].CommandSequenceNumber == 1, "CommandSequenceNumber == 1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].RunStatus == SCA_CHILD_EXECUTING, "RunStatus == SCA_CHILD_EXECUTING");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_CREAT_CHILD_INFO_EID, CFE_EVS_DEBUG, "Started ATS Child Task: SCA_CHILD2"),
        "Started ATS Child Task: SCA_CHILD2");

    /* Start SCA_CHILD_3 */

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_START_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename3, OS_MAX_PATH_LEN);

    /* Execute the function being tested (Start ATS ats3.txt) */
    SCA_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_MemCmp(SCA_OperData.ChildInfo[SCA_CHILD_3].ChildAtsFile, Filename3, OS_MAX_PATH_LEN, "ChildInfo[SCA_CHILD_3].ChildAtsFile == Filename3");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].CommandStatus == SCA_NON_COMMAND, "CommandStatus == SCA_NON_COMMAND");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].CommandSequenceNumber == 1, "CommandSequenceNumber == 1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].RunStatus == SCA_CHILD_EXECUTING, "RunStatus == SCA_CHILD_EXECUTING");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_CREAT_CHILD_INFO_EID, CFE_EVS_DEBUG, "Started ATS Child Task: SCA_CHILD3"),
        "Started ATS Child Task: SCA_CHILD3");

    /* Start SCA_CHILD_4 */

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_START_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename4, OS_MAX_PATH_LEN);

    /* Execute the function being tested (Start ATS ats4.txt) */
    SCA_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_MemCmp(SCA_OperData.ChildInfo[SCA_CHILD_4].ChildAtsFile, Filename4, OS_MAX_PATH_LEN, "ChildInfo[SCA_CHILD_4].ChildAtsFile == Filename4");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].CommandStatus == SCA_NON_COMMAND, "CommandStatus == SCA_NON_COMMAND");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].CommandSequenceNumber == 1, "CommandSequenceNumber == 1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].RunStatus == SCA_CHILD_EXECUTING, "RunStatus == SCA_CHILD_EXECUTING");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_CREAT_CHILD_INFO_EID, CFE_EVS_DEBUG, "Started ATS Child Task: SCA_CHILD4"),
        "Started ATS Child Task: SCA_CHILD4");

    /* Start SCA_CHILD_5 */

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_START_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename5, OS_MAX_PATH_LEN);

    /* Execute the function being tested (Start ATS ats5.txt) */
    SCA_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_MemCmp(SCA_OperData.ChildInfo[SCA_CHILD_5].ChildAtsFile, Filename5, OS_MAX_PATH_LEN, "ChildInfo[SCA_CHILD_5].ChildAtsFile == Filename5");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].CommandStatus == SCA_NON_COMMAND, "CommandStatus == SCA_NON_COMMAND");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].CommandSequenceNumber == 1, "CommandSequenceNumber == 1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].RunStatus == SCA_CHILD_EXECUTING, "RunStatus == SCA_CHILD_EXECUTING");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_CREAT_CHILD_INFO_EID, CFE_EVS_DEBUG, "Started ATS Child Task: SCA_CHILD5"),
        "Started ATS Child Task: SCA_CHILD5");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 5, "Ut_CFE_EVS_GetEventQueueDepth() == 5");

} /* end SCA_StartAtsCmd_Test_Nominal */

void SCA_StartAtsCmd_Test_InUseError(void)
{
    /**
     **  Test case: SCA_ATS_START_ATS_IN_USE_ERR_EID
     **/

    char              Filename[OS_MAX_PATH_LEN];
    SCA_StartAtsCmd_t CmdPacket;

    strncpy (Filename, "/ram/ats1.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_START_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);

    /* Set Child 2 ATS file to same file that we're trying to open right now, and mark it as running - will generate error */
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_2].ChildAtsFile, Filename, OS_MAX_PATH_LEN);
    SCA_OperData.ChildInfo[SCA_CHILD_2].RunStatus = SCA_CHILD_EXECUTING;

    /* Execute the function being tested */
    SCA_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_START_ATS_IN_USE_ERR_EID, CFE_EVS_ERROR, "ATS /ram/ats1.txt already in use"),
        "ATS /ram/ats1.txt already in use");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_StartAtsCmd_Test_InUseError */

void SCA_StartAtsCmd_Test_CmdCreateChildError(void)
{
    /**
     **  Test case: SCA_STARTATS_CMD_CREAT_CHILD_ERR_EID
     **/

    char              Filename[OS_MAX_PATH_LEN];
    SCA_StartAtsCmd_t CmdPacket;

    strncpy (Filename, "/ram/ats1.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_START_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_CREATECHILDTASK_INDEX, OS_ERROR, 1);

    /* Execute the function being tested */
    SCA_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_CREAT_CHILD_ERR_EID, CFE_EVS_ERROR, "Start ATS Rejected: Child Task Creation Failure: 0xffffffff, SCA_CHILD1"),
        "Start ATS Rejected: Child Task Creation Failure: 0xffffffff, SCA_CHILD1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_StartAtsCmd_Test_CmdCreateChildError */

void SCA_StartAtsCmd_Test_CmdMaxAtsError(void)
{
    /**
     **  Test case: SCA_STARTATS_CMD_MAX_ATS_ERR_EID
     **/

    char              Filename[OS_MAX_PATH_LEN];
    SCA_StartAtsCmd_t CmdPacket;

    strncpy (Filename, "/ram/ats1.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_START_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);

    /* Set all child processes to SCA_CHILD_EXECUTING - will generate error */
    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_2].RunStatus = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_3].RunStatus = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_4].RunStatus = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_5].RunStatus = SCA_CHILD_EXECUTING;

    /* Execute the function being tested */
    SCA_StartAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_MAX_ATS_ERR_EID, CFE_EVS_ERROR, "Start ATS Rejected: Max number of ATS executing"),
        "Start ATS Rejected: Max number of ATS executing");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_StartAtsCmd_Test_CmdMaxAtsError */

void SCA_StopAtsCmd_Test_Nominal(void)
{
    char              Filename[OS_MAX_PATH_LEN];
    SCA_StopAtsCmd_t  CmdPacket;

    strncpy (Filename, "/ram/ats1.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StopAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_STOP_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);

    /* Execute the function being tested */
    SCA_StopAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS == TRUE, "SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS == TRUE");
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 1, "CmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_StopAtsCmd_Test_Nominal */

void SCA_StopAtsCmd_Test_NotRunningError(void)
{
    /**
     **  Test case: SCA_STOPATS_NOTRUNNING_INFO_EID (not really an error)
     **/

    char              Filename[OS_MAX_PATH_LEN];
    SCA_StopAtsCmd_t  CmdPacket;

    strncpy (Filename, "/ram/ats1.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, SCA_CHILD_NAME_SIZE);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StopAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_STOP_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);

    /* Execute the function being tested */
    SCA_StopAtsCmd((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS == FALSE, "SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS == FALSE");
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 1, "CmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STOPATS_NOTRUNNING_INFO_EID, CFE_EVS_INFORMATION, "ATS /ram/ats1.txt is not running"),
        "ATS /ram/ats1.txt is not running");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_StopAtsCmd_Test_NotRunningError */

void SCA_GetATSInfo_Test_Nominal(void)
{
    char                 Filename[OS_MAX_PATH_LEN];
    SCA_GetATSInfoCmd_t  CmdPacket;

    strncpy (Filename, "/ram/atsinfo.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;
    SCA_CopyChildInformation(SCA_CHILD_1);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_GetATSInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_GET_ATS_INFO_CC);
    strncpy (CmdPacket.OutputFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);

    /* Execute the function being tested */
    SCA_GetATSInfo((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 1, "CmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATSINFO_WRITTEN_EID, CFE_EVS_INFORMATION, "Get ATS Info: 108 Bytes written to /ram/atsinfo.txt"),
        "Get ATS Info: 108 Bytes written to /ram/atsinfo.txt");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_GetATSInfo_Test_Nominal */

void SCA_GetATSInfo_Test_CreateFileError(void)
{
    /**
     **  Test case: SCA_ATSINFO_CREATFILE_ERR_EID
     **/

    char                 Filename[OS_MAX_PATH_LEN];
    SCA_GetATSInfoCmd_t  CmdPacket;

    strncpy (Filename, "/ram/atsinfo.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;
    SCA_CopyChildInformation(SCA_CHILD_1);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_GetATSInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_GET_ATS_INFO_CC);
    strncpy (CmdPacket.OutputFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CREAT_INDEX, OS_FS_ERROR, 1);

    /* Execute the function being tested */
    SCA_GetATSInfo((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATSINFO_CREATFILE_ERR_EID, CFE_EVS_ERROR, "Get ATS Info: Failed to create file /ram/atsinfo.txt: err = 0xffffffff"),
        "Get ATS Info: Failed to create file /ram/atsinfo.txt: err = 0xffffffff");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_GetATSInfo_Test_CreateFileError */

void SCA_GetATSInfo_Test_WriteFileError(void)
{
    /**
     **  Test case: SCA_ATSINFO_WRITEFILE_ERR_EID
     **/

    char                 Filename[OS_MAX_PATH_LEN];
    SCA_GetATSInfoCmd_t  CmdPacket;

    strncpy (Filename, "/ram/atsinfo.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;
    SCA_CopyChildInformation(SCA_CHILD_1);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_GetATSInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_GET_ATS_INFO_CC);
    strncpy (CmdPacket.OutputFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_WRITE_INDEX, OS_FS_ERROR, 1);

    /* Execute the function being tested */
    SCA_GetATSInfo((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATSINFO_WRITEFILE_ERR_EID, CFE_EVS_ERROR, "Get ATS Info Rejected: Write File Error: 0xffffffff, SCA_CHILD1, /ram/atsinfo.txt"),
        "Get ATS Info Rejected: Write File Error: 0xffffffff, SCA_CHILD1, /ram/atsinfo.txt");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_GetATSInfo_Test_WriteFileError */

void SCA_GetATSInfo_Test_CloseFileError(void)
{
    /**
     **  Test case: SCA_ATSINFO_CLOSE_FILE_CRITICAL_EID
     **/

    char                 Filename[OS_MAX_PATH_LEN];
    SCA_GetATSInfoCmd_t  CmdPacket;

    strncpy (Filename, "/ram/atsinfo.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;
    SCA_CopyChildInformation(SCA_CHILD_1);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_GetATSInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_GET_ATS_INFO_CC);
    strncpy (CmdPacket.OutputFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CLOSE_INDEX, OS_FS_ERROR, 1);

    /* Execute the function being tested */
    SCA_GetATSInfo((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATSINFO_CLOSE_FILE_CRITICAL_EID, CFE_EVS_CRITICAL, "Get ATS Info: Error closing file /ram/atsinfo.txt, err = 0xffffffff"),
        "Get ATS Info: Error closing file /ram/atsinfo.txt, err = 0xffffffff");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SCA_GetATSInfo_Test_CloseFileError */

void SCA_GetATSInfo_Test_NoRunningAtsError(void)
{
    /**
     **  Test case: "Get ATS Info: No Running ATS, data file not created"
     **/

    char                 Filename[OS_MAX_PATH_LEN];
    SCA_GetATSInfoCmd_t  CmdPacket;

    strncpy (Filename, "/ram/atsinfo.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_NOT_USED;
    SCA_OperData.ChildInfo[SCA_CHILD_2].RunStatus = SCA_CHILD_NOT_USED;
    SCA_OperData.ChildInfo[SCA_CHILD_3].RunStatus = SCA_CHILD_NOT_USED;
    SCA_OperData.ChildInfo[SCA_CHILD_4].RunStatus = SCA_CHILD_NOT_USED;
    SCA_OperData.ChildInfo[SCA_CHILD_5].RunStatus = SCA_CHILD_NOT_USED;
    SCA_CopyChildInformation(SCA_CHILD_1);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_GetATSInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_GET_ATS_INFO_CC);
    strncpy (CmdPacket.OutputFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, UT_CFE_SB_MSGLENGTH, 1);

    /* Execute the function being tested */
    SCA_GetATSInfo((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATSINFO_NOFILE_EID, CFE_EVS_INFORMATION, "Get ATS Info: No Running ATS, data file not created"),
        "Get ATS Info: No Running ATS, data file not created");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_GetATSInfo_Test_NoRunningAtsError */

void SCA_GetATSInfo_Test_IncorrectMsgLengthError(void)
{
    /**
     **  Test case: Incorrect message length from SB_GetTotalMsgLength (not really an error)
     **/

    char                 Filename[OS_MAX_PATH_LEN];
    SCA_GetATSInfoCmd_t  CmdPacket;

    strncpy (Filename, "/ram/atsinfo.txt", OS_MAX_PATH_LEN);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;
    SCA_CopyChildInformation(SCA_CHILD_1);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_GetATSInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_GET_ATS_INFO_CC);
    strncpy (CmdPacket.OutputFilename, Filename, OS_MAX_PATH_LEN);

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETTOTALMSGLENGTH_INDEX, 5, 1);

    /* Execute the function being tested */
    SCA_GetATSInfo((CFE_SB_MsgPtr_t)(&CmdPacket));

    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATSINFO_WRITTEN_EID, CFE_EVS_INFORMATION, "Get ATS Info: 108 Bytes written to /ram/sca_atsinfo.txt"),
        "Get ATS Info: 108 Bytes written to /ram/sca_atsinfo.txt");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_GetATSInfo_Test_IncorrectMsgLengthError */

/* SCA_CreateChild_Test_Nominal not needed - already tested elsewhere */

void SCA_CreateChild_Test_AtsIndexError(void)
{
    /**
     **  Test case: AtsIndex > SCA_CHILD_5
     **/

    int32 Result;

    /* Execute the function being tested */
    Result = SCA_CreateChild(99);
    
    /* Verify results */
    UtAssert_True(Result == CFE_ES_ERR_CHILD_TASK_CREATE, "Result == CFE_ES_ERR_CHILD_TASK_CREATE");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_StopAtsCmd_Test_AtsIndexError */

void SCA_AtsRq_Test_AddTestCases(void)
{
    UtTest_Add(SCA_StartAtsCmd_Test_Nominal, SCA_Test_Setup, SCA_Test_TearDown, "SCA_StartAtsCmd_Test_Nominal");
    UtTest_Add(SCA_StartAtsCmd_Test_InUseError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_StartAtsCmd_Test_InUseError");
    UtTest_Add(SCA_StartAtsCmd_Test_CmdCreateChildError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_StartAtsCmd_Test_CmdCreateChildError");
    UtTest_Add(SCA_StartAtsCmd_Test_CmdMaxAtsError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_StartAtsCmd_Test_CmdMaxAtsError");
    UtTest_Add(SCA_StopAtsCmd_Test_Nominal, SCA_Test_Setup, SCA_Test_TearDown, "SCA_StopAtsCmd_Test_Nominal");
    UtTest_Add(SCA_StopAtsCmd_Test_NotRunningError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_StopAtsCmd_Test_NotRunningError");
    UtTest_Add(SCA_GetATSInfo_Test_Nominal, SCA_Test_Setup, SCA_Test_TearDown, "SCA_GetATSInfo_Test_Nominal");
    UtTest_Add(SCA_GetATSInfo_Test_CreateFileError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_GetATSInfo_Test_CreateFileError");
    UtTest_Add(SCA_GetATSInfo_Test_WriteFileError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_GetATSInfo_Test_WriteFileError");
    UtTest_Add(SCA_GetATSInfo_Test_CloseFileError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_GetATSInfo_Test_CloseFileError");
    UtTest_Add(SCA_GetATSInfo_Test_NoRunningAtsError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_GetATSInfo_Test_NoRunningAtsError");
    UtTest_Add(SCA_GetATSInfo_Test_IncorrectMsgLengthError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_GetATSInfo_Test_IncorrectMsgLengthError");
    UtTest_Add(SCA_CreateChild_Test_AtsIndexError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_CreateChild_Test_AtsIndexError");
}
