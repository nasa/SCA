 /*************************************************************************
 ** File:
 **   $Id: sca_atsrq_test.h 1.1 2015/03/10 13:08:57EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the function prototypes for the unit test cases for the functions contained in the file sca_atsrq.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_atsrq_test.h  $
 **   Revision 1.1 2015/03/10 13:08:57EDT czogby 
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
#include "sca_atsrq.h"

/*
 * Function Prototypes
 */

void SCA_StartAtsCmd_Test_Nominal(void);
void SCA_StartAtsCmd_Test_InUseError(void);
void SCA_StartAtsCmd_Test_CmdCreateChildError(void);
void SCA_StartAtsCmd_Test_CmdMaxAtsError(void);
void SCA_StopAtsCmd_Test_Nominal(void);
void SCA_StopAtsCmd_Test_NotRunningError(void);
void SCA_GetATSInfo_Test_Nominal(void);
void SCA_GetATSInfo_Test_CreateFileError(void);
void SCA_GetATSInfo_Test_WriteFileError(void);
void SCA_GetATSInfo_Test_CloseFileError(void);
void SCA_GetATSInfo_Test_NoRunningAtsError(void);
void SCA_GetATSInfo_Test_IncorrectMsgLengthError(void);
void SCA_CreateChild_Test_AtsIndexError(void);
void SCA_AtsRq_Test_AddTestCases(void);
