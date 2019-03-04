/*************************************************************************
 ** File:
 **   $Id: sca_verify.h 1.3 2014/11/17 12:15:12EST czogby Exp  $
 **
 ** Purpose:
 **   Contains CFS Stored Command Absolute macros that run preprocessor
 **   checks on mission configurable parameters
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_verify.h  $
 **   Revision 1.3 2014/11/17 12:15:12EST czogby 
 **   --- Added comments ---  sstrege [2014/11/17 19:19:29Z]
 **   Added verification for the SCA_MAX_CMD_PEND_DELAY parameter
 **   Revision 1.2 2014/09/24 16:25:34EDT czogby 
 **   Updates from code inspection.
 **   Revision 1.1 2014/08/26 15:38:27EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
 *************************************************************************/
#ifndef _sca_verify_
#define _sca_verify_

/*************************************************************************
 ** Includes
 *************************************************************************/

#include "cfe.h"
#include "sca_platform_cfg.h"

/*************************************************************************
 ** Macro Definitions
 *************************************************************************/

#ifndef SCA_PIPE_DEPTH
    #error SCA_PIPE_DEPTH must be defined!
#elif (SCA_PIPE_DEPTH > CFE_SB_MAX_PIPE_DEPTH)
    #error SCA_PIPE_DEPTH cannot be greater than CFE_SB_MAX_PIPE_DEPTH!
#elif (SCA_PIPE_DEPTH < 1)
    #error SCA_PIPE_DEPTH cannot be less than 1!
#endif

#ifndef SCA_TIME_TO_USE
    #error SCA_TIME_TO_USE must be defined!
#elif (SCA_TIME_TO_USE != SCA_USE_CFE_TIME)
  #if (SCA_TIME_TO_USE != SCA_USE_TAI)
    #if (SCA_TIME_TO_USE != SCA_USE_UTC)
      #error SCA_TIME_TO_USE must be either SCA_USE_CFE_TIME, SCA_USE_TAI or SCA_USE_UTC!
    #endif
  #endif
#endif

#ifndef SCA_MISSION_REV
    #error SCA_MISSION_REV must be defined!
#elif (SCA_MISSION_REV < 0)
    #error SCA_MISSION_REV must be greater than or equal to zero!
#endif


/*
 Determination of minimum stack size:
 -----
 SCA_Child1Main:            2 + (SCA_ProcessFile || SCA_ClearChildInformation || 1000)
 SCA_ProcessFile:           2 + 1 + 2 + 256 + 4 + (SCA_GetNextCommand || SCA_CopyChildInformation || SCA_SendCommand || 1000)
 SCA_ClearChildInformation: 1 + (1000 || SCA_CopyChildInformation)
 SCA_GetNextCommand:        2 + 1 + 8 + 14 + 513 + 17 + 513 + 3 + 513 + (SCA_ProcessAllParameters || 1000)
 SCA_CopyChildInformation:  1001
 SCA_SendCommand:           2 + 1 + 8 + 7 + 4 + (SCA_GetCurrentTime || 1000)
 SCA_ProcessAllParameters:  2 + 1 + 16 + 9 + (SCA_ProcessParameter || 1000)
 SCA_ProcessParameter:      1 + 8 + 8 + 2 + 2 + 8 + 256 + 3 + 2 + 2 + 256 + 1000
 SCA_GetCurrentTime:        4 + 8 + 1000
 -----
 SCA_Child1Main:            3427     <-----
 SCA_ProcessFile:           3425
 SCA_ClearChildInformation: 1002
 SCA_GetNextCommand:        3160
 SCA_CopyChildInformation:  1001
 SCA_SendCommand:           1034
 SCA_ProcessAllParameters:  1576
 SCA_ProcessParameter:      1548
 SCA_GetCurrentTime:        1012
*/
#ifndef SCA_ATS_CHILD_STACK_SIZE
    #error SCA_ATS_CHILD_STACK_SIZE must be defined!
#elif (SCA_ATS_CHILD_STACK_SIZE < 3427)
    #error SCA_ATS_CHILD_STACK_SIZE cannot be less than 3427 bytes (8000 bytes is recommended).
#elif (SCA_ATS_CHILD_STACK_SIZE > 0xFFFFFFFF)
    #error SCA_ATS_CHILD_STACK_SIZE cannot be greater than 0xFFFFFFFF.
#endif

#ifndef SCA_ATS_CHILD_TASK_PRIORITY
    #error SCA_ATS_CHILD_TASK_PRIORITY must be defined!
#elif (SCA_ATS_CHILD_TASK_PRIORITY > 255)
    #error SCA_ATS_CHILD_TASK_PRIORITY cannot be greater than 255.
#elif (SCA_ATS_CHILD_TASK_PRIORITY < 1)
    #error SCA_ATS_CHILD_TASK_PRIORITY cannot be less than 1.
#endif

#ifndef SCA_MAX_CMD_PEND_DELAY
    #error SCA_MAX_CMD_PEND_DELAY must be defined!
#elif (SCA_MAX_CMD_PEND_DELAY > 0xFFFFFFFF)
    #error SCA_MAX_CMD_PEND_DELAY cannot be greater than 0xFFFFFFFF.
#elif (SCA_MAX_CMD_PEND_DELAY < 0)
    #error SCA_MAX_CMD_PEND_DELAY cannot be less than 0.
#endif

#endif

/*_sca_verify_*/

/************************/
/*  End of File Comment */
/************************/
