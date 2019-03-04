 /*************************************************************************
 ** File:
 **   $Id: sca_child_test.h 1.1 2015/03/10 13:08:58EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the function prototypes for the unit test cases for the functions contained in the file sca_child.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_child_test.h  $
 **   Revision 1.1 2015/03/10 13:08:58EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "common_types.h"
#include "utassert.h"
#include "uttest.h"
#include "sca_app.h"
#include "sca_child.h"

/*
 * Function Prototypes
 */

void SCA_ProcessFile_Test_StopAts(void);
void SCA_ProcessFile_Test_RunFullAts(void);
void SCA_ProcessFile_Test_FileOpenError(void);
void SCA_ProcessFile_Test_CloseFileError(void);
void SCA_GetNextCommand_Test_StopAts(void);
void SCA_GetNextCommand_Test_AllNominalCases(void);
void SCA_GetNextCommand_Test_ReadFileError(void);
void SCA_ProcessAllParameters_Test_Nominal(void);
void SCA_ProcessAllParameters_Test_BadStringError(void);
void SCA_SendCommand_Test_StopAts1(void);
void SCA_SendCommand_Test_ForceFlagFalseSendLater(void);
void SCA_SendCommand_Test_ForceFlagFalseSendNow(void);
void SCA_SendCommand_Test_ForceFlagTrueSendLater(void);
void SCA_SendCommand_Test_ForceFlagTrueSendNow(void);
void SCA_SendCommand_Test_ForceFlagFalseSkippedCmd(void);
void SCA_SendCommand_Test_ForceFlagFalseFailedSend(void);
void SCA_SendCommand_Test_ForceFlagTrueSkippedCmd(void);
void SCA_SendCommand_Test_ForceFlagTrueFailedSend(void);
void SCA_ChildSleepUntilCommandTime_Test_TimeDriftError(void);
void SCA_Child1Main_StopATS (void);
void SCA_Child1Main_ExecutionCompleted (void);
void SCA_Child2Main_StopATS (void);
void SCA_Child2Main_ExecutionCompleted (void);
void SCA_Child3Main_StopATS (void);
void SCA_Child3Main_ExecutionCompleted (void);
void SCA_Child4Main_StopATS (void);
void SCA_Child4Main_ExecutionCompleted (void);
void SCA_Child5Main_StopATS (void);
void SCA_Child5Main_ExecutionCompleted (void);
void SCA_Child_Test_AddTestCases(void);
