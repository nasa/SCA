 /*************************************************************************
 ** File:
 **   $Id: sca_cmds.h 1.2 2015/02/20 17:04:58EST czogby Exp  $
 **
 ** Purpose: 
 **   This file contains the prototypes for the functions to handle processing 
 **   of ground command requests and housekeeping requests
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_cmds.h  $ 
 **   Revision 1.2 2015/02/20 17:04:58EST czogby  
 **   Move sca_cmds.c function prototype into sca_cmds.h 
 **   Revision 1.1 2014/08/26 15:38:24EDT czogby  
 **   Initial revision 
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj 
 *************************************************************************/
#ifndef _sca_cmds_
#define _sca_cmds_

#include "cfe.h"

/************************************************************************/
/** \brief Routes commands to be processed
 **  
 **  \par Description
 **           This routine determines the source of a request to
 **           the Stored Command Absolute processor and routes it to one 
 **           of the lower level request processing routines.
 **
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message
 **
 *************************************************************************/
void SCA_ProcessRequest (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Processes commands
 **  
 **  \par Description
 **       Process commands. Commands can be from external sources or from SCA
 **       itself.
 **       
 **       
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **
 *************************************************************************/
void SCA_ProcessCommand (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Sends out an Event message
 **  
 **  \par Description
 **       Command for testing aliveness of SCA    
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SCA_NOOP_CC
 **
 *************************************************************************/
void SCA_NoOpCmd (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Reset Counters Command
 **  
 **  \par Description
 **       Clears the command counters and error counters for SCA  
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SCA_RESET_COUNTERS_CC
 **
 *************************************************************************/
void SCA_ResetCountersCmd (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Send Hk Packet to the ground 
 **  
 **  \par Description
 **       This routine collects the housekeeping status information,
 **       formats the packet and sends the packet over the software bus
 **       to health and safety.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None

 *************************************************************************/
void SCA_SendHkPacket (void);

/************************************************************************/
/** \brief Verify command message length
 **  
 **  \par Description
 **       This routine will check if the actual length of a software bus
 **       command message matches the expected length and send an
 **       error event message if a mismatch occurs
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \param [in]   msg              A #CFE_SB_MsgPtr_t pointer that
 **                                 references the software bus message 
 **
 **  \param [in]   ExpectedLength   The expected length of the message
 **                                 based upon the command code
 **
 **  \returns
 **  \retstmt Returns TRUE if the length is as expected      \endcode
 **  \retstmt Returns FALSE if the length is not as expected \endcode
 **  \endreturns
 **
 **  \sa #SC_LEN_ERR_EID
 **
 *************************************************************************/
boolean SCA_VerifyCmdLength(CFE_SB_MsgPtr_t msg, 
                            uint16          ExpectedLength);

#endif /*_sca_cmds_*/

/************************/
/*  End of File Comment */
/************************/
