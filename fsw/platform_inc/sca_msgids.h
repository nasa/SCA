 /*************************************************************************
 ** File:
 **   $Id: sca_msgids.h 1.1 2014/08/26 15:38:20EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the message ID's used by Stored Command Absolute
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_msgids.h  $ 
 **   Revision 1.1 2014/08/26 15:38:20EDT czogby  
 **   Initial revision 
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/platform_inc/project.pj 
 *************************************************************************/
#ifndef _sca_msgids_
#define _sca_msgids_

/*************************************************************************
 ** Macro Definitions
 *************************************************************************/

/**
 ** \name SCA Command Message Numbers */ 
/** \{ */
#define SCA_CMD_MID                          (0x18C0)    /**< \brief Msg ID for cmds to SCA   */
#define SCA_SEND_HK_MID                      (0x18C1)    /**< \brief Msg ID to request SCA HK */
/** \} */

/**
 ** \name SCA Telemetery Message Number */ 
/** \{ */
#define SCA_HK_TLM_MID                       (0x08BB)    /**< \brief Msg ID to send telemtry down on */
/** \} */

#endif /*_sca_msgids_*/

/************************/
/*  End of File Comment */
/************************/
