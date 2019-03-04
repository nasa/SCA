 /*************************************************************************
 ** File:
 **   $Id: sca_utils.h 1.2 2014/11/17 12:15:06EST czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the utilty functions for Stored Command Absolute
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_utils.h  $
 **   Revision 1.2 2014/11/17 12:15:06EST czogby 
 **   --- Added comments ---  sstrege [2014/11/17 19:13:32Z]
 **   Updated SCA_GetCurrentTime function description.  Added function prototype and description for SCA_GetCurrentTimeFull function.
 **   Revision 1.1 2014/08/26 15:38:26EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
 *************************************************************************/
#ifndef _sca_utils_
#define _sca_utils_

#include "cfe.h"
#include "sca_platform_cfg.h"

/** 
**  \brief Absolute Value time tag for ATC's
*/
typedef     uint32     SCA_AbsTimeTag_t;

/************************************************************************/
/** \brief Gets the current time from CFE, ignoring subseconds
 **  
 **  \par Description
 **       Queries the CFE TIME services and retieves the Current time, returning only the seconds
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
SCA_AbsTimeTag_t   SCA_GetCurrentTime (void);

/************************************************************************/
/** \brief Gets the current time from CFE, including subseconds
 **  
 **  \par Description
 **       Queries the CFE TIME services and retieves the Current time
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
CFE_TIME_SysTime_t SCA_GetCurrentTimeFull(void);

#endif /*_sca_utils_*/

/************************/
/*  End of File Comment */
/************************/
