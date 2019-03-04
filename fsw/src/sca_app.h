/*************************************************************************
 ** File:
 **   $Id: sca_app.h 1.2 2015/02/20 16:58:19EST czogby Exp  $
 **
 ** Purpose: 
 **     This file contains the Stored Command Absolute main event loop header
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_app.h  $
 **   Revision 1.2 2015/02/20 16:58:19EST czogby 
 **   Move sca_app.c function prototypes into sca_app.h
 **   Revision 1.1 2014/08/26 15:38:22EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
 
 *************************************************************************/

#ifndef _sca_app_
#define _sca_app_

/*************************************************************************
** Includes
*************************************************************************/

#include "cfe.h"
#include "sca_platform_cfg.h"
#include "sca_msgdefs.h"
#include "sca_msg.h"
#include "sca_utils.h"
#include "sca_child.h"

/**************************************************************************
 **
 ** Function prototypes
 **
 **************************************************************************/

/************************************************************************/
/** \brief Main loop for SCA
 **  
 **  \par Description
 **       This function is the entry point and main loop for the Stored
 **       Command Absolute (SCA) application.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void SCA_AppMain (void);

/************************************************************************/
/** \brief Initialize application
 **  
 **  \par Description
 **       This function initializes the SCA application. The return value
 **       is either CFE_SUCCESS or the error code from the failed cFE
 **       function call. Note that all errors generate an identifying
 **       event message.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \returns
 **  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
 **  \retstmt Return status from CFE initialization function \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 SCA_AppInit (void);

/************************************************************************/
/** \brief Create Data Mutexes
 **  
 **  \par Description
 **       This function creates the data mutexes used by the SCA application. 
 **       The return value is either CFE_SUCCESS or the error code from the 
 **       failed cFE function call. Note that all errors generate an identifying
 **       event message.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \returns
 **  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
 **  \retstmt Return status from mutex create function \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 SCA_CreateMutexes (void);

/************************************************************************/
/** \brief Create Semaphores
 **  
 **  \par Description
 **       This function creates and takes the child semaphores used to create
 **       delays when waiting for the right time to send a command. **
 **       The return value is either CFE_SUCCESS or the error code from the 
 **       failed cFE function call. Note that all errors generate an identifying
 **       event message.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \returns
 **  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESS \endcode
 **  \retstmt Return status from semaphore create function \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 SCA_CreateSemaphores (void);

/************************************************************************
** Macro Definitions
*************************************************************************/

/**
** \name SCA housekeeping mutex name */ 
/** \{ */
#define SCA_HK_MUTEX_NAME      "SCA_HK_MUTEX"
/** \} */

/**
** \name SCA housekeeping data mutex options */ 
/** \{ */
#define SCA_HK_MUTEX_OPTIONS   0
/** \} */

/**
** \name  Cmd pipe name */ 
/** \{ */
#define SCA_CMD_PIPE_NAME        "SCA_CMD_PIPE"
/** \} */

/*********************************************************************************************/

/** 
**  \brief SCA Operational Data Structure 
*/
typedef struct
{
    CFE_SB_MsgPtr_t         MsgPtr;                             /**< \brief Pointer to command message          */
    CFE_SB_PipeId_t         CmdPipe;                            /**< \brief Command pipe ID                     */

    SCA_HkTlm_t             HkPacket;                           /**< \brief SCA Housekeeping structure           */
    uint32                  HkMutex;

    SCA_ChildInfo_t         ChildInfo[SCA_MAX_NUM_ATS];
    SCA_ChildInfoOutput_t   ChildInfoOutput[SCA_MAX_NUM_ATS];  /**< \brief Used for SCA_GetATSInfo */
    
} SCA_OperData_t;

/************************************************************************
** Exported Data
*************************************************************************/
extern SCA_OperData_t    SCA_OperData;

#endif /* _sca_app_ */

/************************/
/*  End of File Comment */
/************************/
