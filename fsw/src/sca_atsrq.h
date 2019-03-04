 /*************************************************************************
 ** File:
 **   $Id: sca_atsrq.h 1.1 2014/08/26 15:38:23EDT czogby Exp  $
 **
 ** Purpose: 
 **     This file contains header for the functions to handle all of the ATS
 **     executive requests and internal reuqests to control
 **     the ATP and ATSs.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_atsrq.h  $
 **   Revision 1.1 2014/08/26 15:38:23EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
  *************************************************************************/

#ifndef _sca_atsreq_
#define _sca_atsreq_

#include "cfe.h"

/************************************************************************/
/** \brief  Command to Start an ATS
 **  
 **  \par Description
 **         This function starts an ATS. This routine does
 **         not actually execute any commands, it simply sets up all
 **         of the data structures to indicate that the specified ATS
 **         is now running. This function also does all of the parameter
 **         checking.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SCA_START_ATS_CC
 **
 *************************************************************************/
void SCA_StartAtsCmd (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Command to Stop an executing ATS
 **  
 **  \par Description
 **            This routine stops an executing ATS.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SCA_STOP_ATS_CC
 **
 *************************************************************************/
void SCA_StopAtsCmd (CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Command to get info on all executing ATSs
 **  
 **  \par Description
 **            This routine writes information about all executing ATSs to a specified 
 **            output file, in binary format. It will overwrite an existing file of the 
 **            same name. If no ATS is running, the file will not be written / overwritten.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         CmdPacket      a #CFE_SB_MsgPtr_t pointer that 
 **                                     references a software bus message 
 **
 **  \sa #SCA_GET_ATS_INFO_CC
 **
 *************************************************************************/
void SCA_GetATSInfo(CFE_SB_MsgPtr_t CmdPacket);

/************************************************************************/
/** \brief Create a child process for an ATS
 **  
 **  \par Description
 **            This routine starts the ATS child process specified by AtsIndex.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         AtsIndex       the index of the child process to start
 **
 **  \returns
 **  \retcode #SCA_NO_ERROR \retdesc \copydoc SCA_NO_ERROR \endcode
 **  \retstmt Return status \endcode
 **  \endreturns
 **
 **  \sa #SCA_GET_ATS_INFO_CC
 **
 *************************************************************************/
int32 SCA_CreateChild(uint8 AtsIndex);

#endif /* _sca_atsreq_ */

/************************/
/*  End of File Comment */
/************************/
