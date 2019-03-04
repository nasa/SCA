
/*
 * Filename: sca_test_utils.c
 *
 * Purpose: This file contains unit test utilities for the SCA.
 *
 */

/*
 * Includes
 */

#include "sca_test_utils.h"

/*
 * Function Definitions
 */

void SCA_Test_Setup(void)
{
    /* initialize test environment to default state for every test */

    SCA_ClearChildInformation (SCA_CHILD_1);
    SCA_ClearChildInformation (SCA_CHILD_2);
    SCA_ClearChildInformation (SCA_CHILD_3);
    SCA_ClearChildInformation (SCA_CHILD_4);
    SCA_ClearChildInformation (SCA_CHILD_5);

    CFE_SB_InitMsg(&SCA_OperData.HkPacket, SCA_HK_TLM_MID, sizeof(SCA_HkTlm_t), TRUE);

    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();

    GLOBAL_UT_OSFileAPI_ReadByteIndex              = 0;
    GLOBAL_UT_CFE_TIME_GetTime_FirstRun            = TRUE;
    GLOBAL_UT_CFE_TIME_GetTime_ChangeAfterFirstRun = FALSE;
    GLOBAL_UT_SCA_Child_LocalTime.Seconds          = 0;
    GLOBAL_UT_SCA_Child_LocalTime.Subseconds       = 0;
}

void SCA_Test_TearDown(void)
{
    /* cleanup test environment */
}
