 /*************************************************************************
 ** File:
 **   $Id: sca_cmds_test.h 1.1 2015/03/10 13:08:59EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the function prototypes for the unit test cases for the functions contained in the file sca_cmds.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_cmds_test.h  $
 **   Revision 1.1 2015/03/10 13:08:59EDT czogby 
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
#include "sca_cmds.h"

/*
 * Function Prototypes
 */

void SCA_ProcessCommand_Test_NoOp(void);
void SCA_ProcessCommand_Test_ResetAllCounters(void);
void SCA_ProcessCommand_Test_ResetCmdCounters(void);
void SCA_ProcessCommand_Test_ResetAtsCounters(void);
void SCA_ProcessCommand_Test_StartAts(void);
void SCA_ProcessCommand_Test_StopAts(void);
void SCA_ProcessCommand_Test_GetAtsInfo(void);
void SCA_ProcessCommand_Test_InvalidCmdError(void);
void SCA_ProcessRequest_Test_CmdMID(void);
void SCA_ProcessRequest_Test_HkMID(void);
void SCA_ProcessRequest_Test_MIDError(void);
void SCA_VerifyCmdLength_Test_LenError(void);
void SCA_Cmds_Test_AddTestCases(void);
