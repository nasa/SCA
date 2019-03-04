 /*************************************************************************
 ** File:
 **   $Id: sca_testrunner.c 1.1 2015/03/10 13:09:02EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the unit test runner for the SCA.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_testrunner.c  $
 **   Revision 1.1 2015/03/10 13:09:02EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "uttest.h"
#include "sca_app_test.h"
#include "sca_child_test.h"
#include "sca_atsrq_test.h"
#include "sca_cmds_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    SCA_Child_Test_AddTestCases();
    SCA_AtsRq_Test_AddTestCases();
    SCA_Cmds_Test_AddTestCases();
    SCA_App_Test_AddTestCases();

    return(UtTest_Run());
}
