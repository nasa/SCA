 /*************************************************************************
 ** File:
 **   $Id: sca_app_test.h 1.1 2015/03/10 13:08:56EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains function prototypes for the unit test cases for the functions contained in the file sca_app.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_app_test.h  $
 **   Revision 1.1 2015/03/10 13:08:56EDT czogby 
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

/*
 * Function Prototypes
 */

void SCA_AppMain_Test_Nominal(void);
void SCA_AppMain_Test_StartUpAndFatalError(void);
void SCA_AppMain_Test_SBError(void);
void SCA_AppInit_Test_Nominal(void);
void SCA_AppInit_Test_EVSRegisterError(void);
void SCA_AppInit_Test_SBCreatePipeError(void);
void SCA_AppInit_Test_SBSubscribeHKError(void);
void SCA_AppInit_Test_SBSubscribeToCmdError(void);
void SCA_AppInit_Test_MutSemCreateError(void);
void SCA_AppInit_Test_CreateChildMutexError(void);
void SCA_AppInit_Test_CreateChildInfoCopyMutexError(void);
void SCA_AppInit_Test_BinSemCreateError(void);
void SCA_App_Test_AddTestCases(void);
