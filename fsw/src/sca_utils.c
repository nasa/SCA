 /*************************************************************************
 ** File:
 **   $Id: sca_utils.c 1.2 2014/11/17 12:15:01EST czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the utilty functions for Stored Command Absolute
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_utils.c  $
 **   Revision 1.2 2014/11/17 12:15:01EST czogby 
 **   --- Added comments ---  sstrege [2014/11/17 19:15:31Z]
 **   Added SCA_GetCurrentTimeFull function definition
 **   Revision 1.1 2014/08/26 15:38:26EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
 *************************************************************************/


/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include "cfe.h"
#include "sca_utils.h"
#include "sca_events.h"
#include "sca_msgdefs.h"
#include "sca_platform_cfg.h"

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get the Current time from CFE TIME, ignoring subseconds         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
SCA_AbsTimeTag_t SCA_GetCurrentTime(void)
{
    CFE_TIME_SysTime_t TempTime;
    
    /* Get the system time of the correct time */
    
    if (SCA_TIME_TO_USE == SCA_USE_UTC)
    {
        TempTime = CFE_TIME_GetUTC();
    }
    else
    {
        if (SCA_TIME_TO_USE == SCA_USE_TAI)
        {
            TempTime = CFE_TIME_GetTAI();
        }
        else
        {
            /* Gets the cFE configured time */
            TempTime = CFE_TIME_GetTime();
        }            
    }

    /* We don't care about subseconds */
    return TempTime.Seconds;
    
} /* end of SCA_GetCurrentTime */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Get the Current time from CFE TIME, including subseconds        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_TIME_SysTime_t SCA_GetCurrentTimeFull(void)
{
    CFE_TIME_SysTime_t TempTime;
    
    /* Get the system time of the correct time */
    
    if (SCA_TIME_TO_USE == SCA_USE_UTC)
    {
        TempTime = CFE_TIME_GetUTC();
    }
    else
    {
        if (SCA_TIME_TO_USE == SCA_USE_TAI)
        {
            TempTime = CFE_TIME_GetTAI();
        }
        else
        {
            /* Gets the cFE configured time */
            TempTime = CFE_TIME_GetTime();
        }            
    }

    return TempTime;

} /* end of SCA_GetCurrentTimeFull */

/************************/
/*  End of File Comment */
/************************/
