 /*************************************************************************
 ** File:
 **   $Id: sca_cmds_test.c 1.1 2015/03/10 13:08:58EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file sca_cmds.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_cmds_test.c  $
 **   Revision 1.1 2015/03/10 13:08:58EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "sca_cmds.h"
#include "sca_cmds_test.h"
#include "sca_events.h"
#include "sca_msgids.h"
#include "sca_test_utils.h"
#include "sca_version.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include <time.h>

/* STRINGIFY(x) is a preprocessor macro to convert preprocessor constants into strings */
#define STR(x) #x
#define STRINGIFY(x) STR(x)

extern SCA_OperData_t SCA_OperData;

/*
 * Function Definitions
 */

void SCA_ProcessCommand_Test_NoOp(void)
{
    /**
     **  Test case: SCA_NOOP_CC
     **/

    SCA_NoArgsCmd_t     CmdPacket;

    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename.txt", OS_MAX_PATH_LEN);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_NOOP_CC);

    /* Execute the function being tested */
    SCA_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 1, "CmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SCA_NOOP_INFO_EID,
            CFE_EVS_INFORMATION,
            "No-op command. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV)),
        "No-op command. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV));

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessCommand_Test_NoOp */

void SCA_ProcessCommand_Test_ResetAllCounters(void)
{
    /**
     **  Test case: SCA_RESET_COUNTERS_CC (SCA_ALL_COUNTERS)
     **/

    SCA_ResetCntrsCmd_t CmdPacket;

    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename.txt", OS_MAX_PATH_LEN);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_ResetCntrsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_RESET_COUNTERS_CC);
    CmdPacket.ResetFlag = SCA_ALL_COUNTERS;

    /* Execute the function being tested */
    SCA_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "AtsCmdCtr[SCA_CHILD_1] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_2] == 0, "AtsCmdCtr[SCA_CHILD_2] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_3] == 0, "AtsCmdCtr[SCA_CHILD_3] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_4] == 0, "AtsCmdCtr[SCA_CHILD_4] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_5] == 0, "AtsCmdCtr[SCA_CHILD_5] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "AtsCmdErrCtr[SCA_CHILD_1] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_2] == 0, "AtsCmdErrCtr[SCA_CHILD_2] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_3] == 0, "AtsCmdErrCtr[SCA_CHILD_3] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_4] == 0, "AtsCmdErrCtr[SCA_CHILD_4] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_5] == 0, "AtsCmdErrCtr[SCA_CHILD_5] == 0");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_RESET_ALL_COUNTERS_DBG_EID, CFE_EVS_DEBUG, "Reset counters command: reset all counters"),
        "Reset counters command: reset all counters");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessCommand_Test_ResetAllCounters */

void SCA_ProcessCommand_Test_ResetCmdCounters(void)
{
    /**
     **  Test case: SCA_RESET_COUNTERS_CC (SCA_CMD_COUNTERS_ONLY)
     **/

    SCA_ResetCntrsCmd_t CmdPacket;

    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename.txt", OS_MAX_PATH_LEN);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_ResetCntrsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_RESET_COUNTERS_CC);
    CmdPacket.ResetFlag = SCA_CMD_COUNTERS_ONLY;

    /* Execute the function being tested */
    SCA_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_RESET_CMD_COUNTERS_DBG_EID, CFE_EVS_DEBUG, "Reset counters command: reset command counters"),
        "Reset counters command: reset command counters");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessCommand_Test_ResetCmdCounters */

void SCA_ProcessCommand_Test_ResetAtsCounters(void)
{
    /**
     **  Test case: SCA_RESET_COUNTERS_CC (SCA_ATS_COUNTERS_ONLY)
     **/

    SCA_ResetCntrsCmd_t CmdPacket;

    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename.txt", OS_MAX_PATH_LEN);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_ResetCntrsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_RESET_COUNTERS_CC);
    CmdPacket.ResetFlag = SCA_ATS_COUNTERS_ONLY;

    /* Execute the function being tested */
    SCA_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "AtsCmdCtr[SCA_CHILD_1] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_2] == 0, "AtsCmdCtr[SCA_CHILD_2] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_3] == 0, "AtsCmdCtr[SCA_CHILD_3] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_4] == 0, "AtsCmdCtr[SCA_CHILD_4] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_5] == 0, "AtsCmdCtr[SCA_CHILD_5] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "AtsCmdErrCtr[SCA_CHILD_1] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_2] == 0, "AtsCmdErrCtr[SCA_CHILD_2] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_3] == 0, "AtsCmdErrCtr[SCA_CHILD_3] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_4] == 0, "AtsCmdErrCtr[SCA_CHILD_4] == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_5] == 0, "AtsCmdErrCtr[SCA_CHILD_5] == 0");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_RESET_ATS_COUNTERS_DBG_EID, CFE_EVS_DEBUG, "Reset counters command: reset ATS counters"),
        "Reset counters command: reset ATS counters");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessCommand_Test_ResetAtsCounters */

void SCA_ProcessCommand_Test_StartAts(void)
{
    /**
     **  Test case: SCA_START_ATS_CC
     **/

    SCA_StartAtsCmd_t   CmdPacket;

    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename.txt", OS_MAX_PATH_LEN);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_START_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename, OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    SCA_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* This function is already verified to work correctly in sca_atsrq_test.c, so no verifications here. */

} /* end SCA_ProcessCommand_Test_StartAts */

void SCA_ProcessCommand_Test_StopAts(void)
{
    /**
     **  Test case: SCA_STOP_ATS_CC		
     **/

    SCA_StopAtsCmd_t    CmdPacket;

    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename.txt", OS_MAX_PATH_LEN);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StopAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_STOP_ATS_CC);
    strncpy (CmdPacket.AtsFilename, Filename, OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    SCA_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* This function is already verified to work correctly in sca_atsrq_test.c, so no verifications here. */

} /* end SCA_ProcessCommand_Test_StopAts */

void SCA_ProcessCommand_Test_GetAtsInfo(void)
{
    /**
     **  Test case: SCA_GET_ATS_INFO_CC		
     **/

    SCA_GetATSInfoCmd_t CmdPacket;

    char Filename[OS_MAX_PATH_LEN];

    strncpy (Filename, "filename.txt", OS_MAX_PATH_LEN);

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_GetATSInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_GET_ATS_INFO_CC);
    strncpy (CmdPacket.OutputFilename, Filename, OS_MAX_PATH_LEN);

    /* Execute the function being tested */
    SCA_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* This function is already verified to work correctly in sca_atsrq_test.c, so no verifications here. */

} /* end SCA_ProcessCommand_Test_GetAtsInfo */

void SCA_ProcessCommand_Test_InvalidCmdError(void)
{
    /**
     **  Test case: SCA_INVLD_CMD_ERR_EID
     **/

    SCA_StartAtsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_StartAtsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, 99);

    /* Execute the function being tested */
    SCA_ProcessCommand((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_INVLD_CMD_ERR_EID, CFE_EVS_ERROR, "Invalid Command Code: MID =  0x18C0 CC =  99"),
        "Invalid Command Code: MID =  0x18C0 CC =  99");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessCommand_Test_InvalidCmdError */

void SCA_ProcessRequest_Test_CmdMID(void)
{
    /**
     **  Test case: SCA_CMD_MID
     **/

    SCA_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_NOOP_CC);

    /* Execute the function being tested */
    SCA_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 1, "CmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SCA_NOOP_INFO_EID,
            CFE_EVS_INFORMATION,
            "No-op command. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV)),
        "No-op command. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV));

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessRequest_Test_CmdMID */

void SCA_ProcessRequest_Test_HkMID(void)
{
    /**
     **  Test case: SCA_SEND_HK_MID
     **/

    SCA_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, SCA_SEND_HK_MID, sizeof(SCA_NoArgsCmd_t), TRUE);

    /* Execute the function being tested */
    SCA_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_ProcessRequest_Test_HkMID */

void SCA_ProcessRequest_Test_MIDError(void)
{
    /**
     **  Test case: SCA_MID_ERR_EID
     **/

    SCA_NoArgsCmd_t   CmdPacket;

    CFE_SB_InitMsg (&CmdPacket, 255, sizeof(SCA_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_NOOP_CC);

    /* Execute the function being tested */
    SCA_ProcessRequest((CFE_SB_MsgPtr_t)(&CmdPacket));
    
    /* Verify results */
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_MID_ERR_EID, CFE_EVS_ERROR, "Invalid software bus pipe message ID: 0x000000FF"),
        "Invalid software bus pipe message ID: 0x000000FF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessRequest_Test_MIDError */

/* SCA_VerifyCmdLength_Test_Nominal not needed - nominal case already tested in other functions. */

void SCA_VerifyCmdLength_Test_LenError(void)
{
    /**
     **  Test case: SCA_LEN_ERR_EID
     **/

    SCA_NoArgsCmd_t CmdPacket;
    boolean Result;

    CFE_SB_InitMsg (&CmdPacket, SCA_CMD_MID, sizeof(SCA_NoArgsCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&CmdPacket, SCA_NOOP_CC);

    /* Execute the function being tested */
    Result = SCA_VerifyCmdLength((CFE_SB_MsgPtr_t)(&CmdPacket), 999);
    
    /* Verify results */
    UtAssert_True(Result == FALSE, "Result == FALSE");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_LEN_ERR_EID, CFE_EVS_ERROR, "Invalid msg length: ID = 0x18C0, CC = 0, Len = 8, Expected = 999"),
        "Invalid msg length: ID = 0x18C0, CC = 0, Len = 8, Expected = 999");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_VerifyCmdLength_Test_LenError */

void SCA_Cmds_Test_AddTestCases(void)
{
    UtTest_Add(SCA_ProcessCommand_Test_NoOp, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessCommand_Test_NoOp");
    UtTest_Add(SCA_ProcessCommand_Test_ResetAllCounters, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessCommand_Test_ResetAllCounters");
    UtTest_Add(SCA_ProcessCommand_Test_ResetCmdCounters, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessCommand_Test_ResetCmdCounters");
    UtTest_Add(SCA_ProcessCommand_Test_ResetAtsCounters, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessCommand_Test_ResetAtsCounters");
    UtTest_Add(SCA_ProcessCommand_Test_StartAts, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessCommand_Test_StartAts");
    UtTest_Add(SCA_ProcessCommand_Test_StopAts, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessCommand_Test_StopAts");
    UtTest_Add(SCA_ProcessCommand_Test_GetAtsInfo, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessCommand_Test_GetAtsInfo");
    UtTest_Add(SCA_ProcessCommand_Test_InvalidCmdError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessCommand_Test_InvalidCmdError");
    UtTest_Add(SCA_ProcessRequest_Test_CmdMID, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessRequest_Test_CmdMID");
    UtTest_Add(SCA_ProcessRequest_Test_HkMID, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessRequest_Test_HkMID");
    UtTest_Add(SCA_ProcessRequest_Test_MIDError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessRequest_Test_MIDError");
    UtTest_Add(SCA_VerifyCmdLength_Test_LenError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_VerifyCmdLength_Test_LenError");
}
