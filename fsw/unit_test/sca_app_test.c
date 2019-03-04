 /*************************************************************************
 ** File:
 **   $Id: sca_app_test.c 1.1 2015/03/10 13:08:56EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file sca_app.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_app_test.c  $
 **   Revision 1.1 2015/03/10 13:08:56EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "sca_app_test.h"
#include "sca_events.h"
#include "sca_msgids.h"
#include "sca_test_utils.h"
#include "sca_version.h"
#include "ut_osapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_es_hooks.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <time.h>

/* STRINGIFY(x) is a preprocessor macro to convert preprocessor constants into strings */
#define STR(x) #x
#define STRINGIFY(x) STR(x)

extern SCA_OperData_t SCA_OperData;

/*
 * Function Definitions
 */

void SCA_AppMain_Test_Nominal(void)
{
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, CFE_SUCCESS, 1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, SCA_SEND_HK_MID, 1);
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_RUNLOOP_INDEX, FALSE, 2);

    /* Execute the function being tested */
    SCA_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SCA_INIT_INFO_EID,
            CFE_EVS_INFORMATION,
            "SCA Initialized. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV)),
        "SCA Initialized. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV));

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_AppMain_Test_Nominal */

void SCA_AppMain_Test_StartUpAndFatalError(void)
{
    /**
     **  Test case: start-up error and "fatal" process error
     **/

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 2);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 0, 1);
    Ut_CFE_ES_SetReturnCode(UT_CFE_ES_REGISTERAPP_INDEX, -1, 1);

    /* Execute the function being tested */
    SCA_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_APP_EXIT_ERR_EID, CFE_EVS_ERROR, "App terminating, Result = 0xFFFFFFFF"),
        "App terminating, Result = 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("SCA App terminating, Result = 0xFFFFFFFF\n"),
        "SCA App terminating, Result = 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");
} /* end SCA_AppMain_Test_StartUpAndFatalError */

void SCA_AppMain_Test_SBError(void)
{
    /**
     **  Test case: Software Bus error
     **/

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_RCVMSG_INDEX, -1, 1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_GETMSGID_INDEX, 0, 1);

    /* Execute the function being tested */
    SCA_AppMain();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_APP_EXIT_ERR_EID, CFE_EVS_ERROR, "App terminating, Result = 0xFFFFFFFF"),
        "App terminating, Result = 0xFFFFFFFF");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SCA_INIT_INFO_EID,
            CFE_EVS_INFORMATION,
            "SCA Initialized. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV)),
        "SCA Initialized. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV));

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 2, "Ut_CFE_EVS_GetEventQueueDepth() == 2");

} /* end SCA_AppMain_Test_SBError */

void SCA_AppInit_Test_Nominal(void)
{
    uint32 ReturnValue;
    uint16 i;
    uint8 InitializedTlmHeader[CFE_SB_TLM_HDR_SIZE];

    CFE_PSP_MemSet (InitializedTlmHeader, 0, CFE_SB_TLM_HDR_SIZE);
    CFE_SB_InitMsg(&InitializedTlmHeader, SCA_HK_TLM_MID, sizeof(SCA_HkTlm_t), FALSE);

    /* Execute the function being tested */
    ReturnValue = SCA_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == CFE_SUCCESS, "ReturnValue == CFE_SUCCESS");
    UtAssert_True (SCA_OperData.MsgPtr == 0, "MsgPtr == 0");
    UtAssert_True (SCA_OperData.CmdPipe == 0, "CmdPipe == 0");

    UtAssert_True (SCA_OperData.HkPacket.CmdErrCtr == 0, "HkPacket.CmdErrCtr == 0");
    UtAssert_True (SCA_OperData.HkPacket.CmdCtr == 0, "HkPacket.CmdCtr == 0");
    UtAssert_True (SCA_OperData.HkPacket.LastAtsCmdErrSeqNum == 0, "HkPacket.LastAtsCmdErrSeqNum == 0");
    UtAssert_True (SCA_OperData.HkPacket.LastAtsCmdErrFile[0] == 0, "HkPacket.LastAtsCmdErrFile[0] == 0");

    for (i = 0; i < SCA_MAX_NUM_ATS; i++)
    {
        UtAssert_True (SCA_OperData.HkPacket.AtsCmdCtr[i] == 0, "HkPacket.AtsCmdCtr[i] == 0");
        UtAssert_True (SCA_OperData.HkPacket.AtsCmdErrCtr[i] == 0, "HkPacket.AtsCmdErrCtr[i] == 0");
    }

    UtAssert_MemCmp(SCA_OperData.HkPacket.TlmHeader, InitializedTlmHeader, CFE_SB_TLM_HDR_SIZE, "HkPacket.TlmHeader == InitializedTlmHeader");

    UtAssert_True
        (strncmp (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE) == 0,
        "ChildInfo[SCA_CHILD_1].ChildName == SCA_CHILD_1_NAME");

    UtAssert_True
        (strncmp (SCA_OperData.ChildInfo[SCA_CHILD_2].ChildName, SCA_CHILD_2_NAME, SCA_CHILD_NAME_SIZE) == 0,
        "ChildInfo[SCA_CHILD_2].ChildName == SCA_CHILD_2_NAME");

    UtAssert_True
        (strncmp (SCA_OperData.ChildInfo[SCA_CHILD_3].ChildName, SCA_CHILD_3_NAME, SCA_CHILD_NAME_SIZE) == 0,
        "ChildInfo[SCA_CHILD_3].ChildName == SCA_CHILD_3_NAME");

    UtAssert_True
        (strncmp (SCA_OperData.ChildInfo[SCA_CHILD_4].ChildName, SCA_CHILD_4_NAME, SCA_CHILD_NAME_SIZE) == 0,
        "ChildInfo[SCA_CHILD_4].ChildName == SCA_CHILD_4_NAME");

    UtAssert_True
        (strncmp (SCA_OperData.ChildInfo[SCA_CHILD_5].ChildName, SCA_CHILD_5_NAME, SCA_CHILD_NAME_SIZE) == 0,
        "ChildInfo[SCA_CHILD_5].ChildName == SCA_CHILD_5_NAME");

    for (i = 0; i < SCA_MAX_NUM_ATS; i++)
    {
        OS_printf ("SCA_OperData.ChildInfo[%i]:\n", i);
        UtAssert_True (SCA_OperData.ChildInfo[i].StopATS == FALSE, "StopATS == FALSE");
        UtAssert_True (SCA_OperData.ChildInfo[i].ChildAtsFile[0] == 0, "ChildAtsFile[0] == 0");
        UtAssert_True (SCA_OperData.ChildInfo[i].RunStatus == 0, "RunStatus == 0");
        UtAssert_True (SCA_OperData.ChildInfo[i].ChildTaskID == 0, "ChildTaskID == 0");
        UtAssert_True (SCA_OperData.ChildInfo[i].FileDescriptor == 0, "FileDescriptor == 0");
        UtAssert_True (SCA_OperData.ChildInfo[i].CommandStatus == 0, "CommandStatus == 0");
        UtAssert_True (SCA_OperData.ChildInfo[i].CommandSequenceNumber == 0, "CommandSequenceNumber == 0");
        UtAssert_True (SCA_OperData.ChildInfo[i].ForceFlag == 0, "ForceFlag == 0");
        UtAssert_True (SCA_OperData.ChildInfo[i].TimeoutPeriod == 0, "TimeoutPeriod == 0");
        UtAssert_True (SCA_OperData.ChildInfo[i].TimeNextCmd == 0, "TimeNextCmd == 0");
        UtAssert_True (SCA_OperData.ChildInfo[i].CurrentTime_Last == 0, "CurrentTime_Last == 0");
    }

    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SCA_INIT_INFO_EID,
            CFE_EVS_INFORMATION,
            "SCA Initialized. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV)),
        "SCA Initialized. Version " STRINGIFY (SCA_MAJOR_VERSION) "." STRINGIFY (SCA_MINOR_VERSION) "." STRINGIFY (SCA_REVISION) "." STRINGIFY (SCA_MISSION_REV));

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_AppInit_Test_Nominal */

void SCA_AppInit_Test_EVSRegisterError(void)
{
    /**
     **  Test case: "Event Services Register returned: 0x%08X\n"
     **/

    int32 ReturnValue;

    Ut_CFE_EVS_SetReturnCode(UT_CFE_EVS_REGISTER_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SCA_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("Event Services Register returned: 0xFFFFFFFF\n"),
        "Event Services Register returned: 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_AppInit_Test_EVSRegisterError */

void SCA_AppInit_Test_SBCreatePipeError(void)
{
    /**
     **  Test case: "Software Bus Create Pipe returned: 0x%08X", Result
     **/

    int32 ReturnValue;

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_CREATEPIPE_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SCA_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SCA_INIT_SB_CREATE_ERR_EID,
            CFE_EVS_ERROR,
            "Software Bus Create Pipe returned: 0xFFFFFFFF"),
        "Software Bus Create Pipe returned: 0xFFFFFFFF");

    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_AppInit_Test_SBCreatePipeError */

void SCA_AppInit_Test_SBSubscribeHKError(void)
{
    /**
     **  Test case: "Software Bus subscribe to housekeeping returned: 0x%08X", Result
     **/

    int32 ReturnValue;

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SCA_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SCA_INIT_SB_SUBSCRIBE_HK_ERR_EID,
            CFE_EVS_ERROR,
            "Software Bus subscribe to housekeeping returned: 0xFFFFFFFF"),
        "Software Bus subscribe to housekeeping returned: 0xFFFFFFFF");

    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_AppInit_Test_SBSubscribeHKError */

void SCA_AppInit_Test_SBSubscribeToCmdError(void)
{
    /**
     **  Test case: "Software Bus subscribe to command returned: 0x%08X", Result
     **/

    int32 ReturnValue;

    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SUBSCRIBE_INDEX, -1, 2);

    /* Execute the function being tested */
    ReturnValue = SCA_AppInit();
    
    /* Verify results */
    UtAssert_True
        (Ut_CFE_EVS_EventSent(
            SCA_INIT_SB_SUBSCRIBE_CMD_ERR_EID,
            CFE_EVS_ERROR,
            "Software Bus subscribe to command returned: 0xFFFFFFFF"),
        "Software Bus subscribe to command returned: 0xFFFFFFFF");

    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_AppInit_Test_SBSubscribeToCmdError */

void SCA_AppInit_Test_MutSemCreateError(void)
{
    /**
     **  Test case: OS_MutSemCreate does not return OS_SUCCESS
     **/

    int32 ReturnValue;

    Ut_OSAPI_SetReturnCode(UT_OSAPI_MUTSEMCREATE_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SCA_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");
    
    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("SCA Create HK Mutex error: returned 0xFFFFFFFF\n"),
        "SCA Create HK Mutex error: returned 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_AppInit_Test_MutSemCreateError */

void SCA_AppInit_Test_CreateChildMutexError(void)
{
    /**
     **  Test case: "SCA Create Child Mutex %d error: returned 0x%08X"
     **/

    int32 ReturnValue;

    Ut_OSAPI_SetReturnCode(UT_OSAPI_MUTSEMCREATE_INDEX, -1, 2);

    /* Execute the function being tested */
    ReturnValue = SCA_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("SCA Create Child Mutex 1 error: returned 0xFFFFFFFF\n"),
        "SCA Create Child Mutex 1 error: returned 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_AppInit_Test_CreateChildMutexError */

void SCA_AppInit_Test_CreateChildInfoCopyMutexError(void)
{
    /**
     **  Test case: "Create Child Info Copy Mutex %d error: returned 0x%08X"
     **/

    int32 ReturnValue;

    Ut_OSAPI_SetReturnCode(UT_OSAPI_MUTSEMCREATE_INDEX, -1, 3);

    /* Execute the function being tested */
    ReturnValue = SCA_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("SCA Create Child Info Copy Mutex 1 error: returned 0xFFFFFFFF\n"),
        "SCA Create Child Info Copy Mutex 1 error: returned 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_AppInit_Test_CreateChildInfoCopyMutexError */

void SCA_AppInit_Test_BinSemCreateError(void)
{
    /**
     **  Test case: OS_BinSemCreate does not return OS_SUCCESS
     **/

    int32 ReturnValue;

    Ut_OSAPI_SetReturnCode(UT_OSAPI_BINSEMCREATE_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnValue = SCA_AppInit();
    
    /* Verify results */
    UtAssert_True (ReturnValue == -1, "ReturnValue == -1");

    UtAssert_True
        (Ut_CFE_ES_SysLogWritten("SCA Create Child Pending Command Semaphore 1 error: returned 0xFFFFFFFF\n"),
        "SCA Create Child Pending Command Semaphore 1 error: returned 0xFFFFFFFF\n");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_AppInit_Test_BinSemCreateError */

/* No tests for SCA_CreateMutexes or SCA_CreateSemaphores because they're covered by SCA_AppInit tests. */

void SCA_App_Test_AddTestCases(void)
{
    UtTest_Add(SCA_AppMain_Test_Nominal, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppMain_Test_Nominal");
    UtTest_Add(SCA_AppMain_Test_StartUpAndFatalError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppMain_Test_StartUpAndFatalError");
    UtTest_Add(SCA_AppMain_Test_SBError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppMain_Test_SBError");
    UtTest_Add(SCA_AppInit_Test_Nominal, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppInit_Test_Nominal");
    UtTest_Add(SCA_AppInit_Test_EVSRegisterError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppInit_Test_EVSRegisterError");
    UtTest_Add(SCA_AppInit_Test_SBCreatePipeError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppInit_Test_SBCreatePipeError");
    UtTest_Add(SCA_AppInit_Test_SBSubscribeHKError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppInit_Test_SBSubscribeHKError");
    UtTest_Add(SCA_AppInit_Test_SBSubscribeToCmdError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppInit_Test_SBSubscribeToCmdError");
    UtTest_Add(SCA_AppInit_Test_MutSemCreateError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppInit_Test_MutSemCreateError");
    UtTest_Add(SCA_AppInit_Test_CreateChildMutexError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppInit_Test_CreateChildMutexError");
    UtTest_Add(SCA_AppInit_Test_CreateChildInfoCopyMutexError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppInit_Test_CreateChildInfoCopyMutexError");
    UtTest_Add(SCA_AppInit_Test_BinSemCreateError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_AppInit_Test_BinSemCreateError");
}
