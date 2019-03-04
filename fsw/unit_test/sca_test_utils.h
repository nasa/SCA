 /*************************************************************************
 ** File:
 **   $Id: sca_test_utils.h 1.1 2015/03/10 13:09:02EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the function prototypes and global variables for the unit test utilities for the SCA.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_test_utils.h  $
 **   Revision 1.1 2015/03/10 13:09:02EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include "sca_app_test.h"
#include "sca_events.h"
#include "sca_msgids.h"
#include "sca_version.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_es_stubs.h"
#include "ut_cfe_evs_stubs.h"
#include "ut_cfe_evs_hooks.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"
#include "ut_cfe_tbl_stubs.h"
#include "ut_cfe_fs_stubs.h"
#include "ut_cfe_time_stubs.h"
#include <time.h>

/*
 * Global Variable Definitions
 */

uint32             GLOBAL_UT_OSFileAPI_ReadByteIndex;
boolean            GLOBAL_UT_CFE_TIME_GetTime_FirstRun;
boolean            GLOBAL_UT_CFE_TIME_GetTime_ChangeAfterFirstRun;
CFE_TIME_SysTime_t GLOBAL_UT_SCA_Child_LocalTime;

/*
 * Function Definitions
 */

void SCA_Test_Setup(void);
void SCA_Test_TearDown(void);
