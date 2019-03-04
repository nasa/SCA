/************************************************************************
** File:
**   $Id: sca_msgdefs.h 1.3 2014/11/17 12:14:55EST czogby Exp  $
**
** Purpose:
**   Specification for the CFS Stored Command (SCA) command and telemetry
**   message constant definitions.
**
** Notes:
**   These Macro definitions have been put in this file (instead of
**   sca_msg.h) so this file can be included directly into ASIST build
**   test scripts. ASIST RDL files can accept C language #defines but
**   can't handle type definitions. As a result: DO NOT PUT ANY
**   TYPEDEFS OR STRUCTURE DEFINITIONS IN THIS FILE!
**   ADD THEM TO sca_msg.h IF NEEDED!
**
**   $Log: sca_msgdefs.h  $
**   Revision 1.3 2014/11/17 12:14:55EST czogby 
**   --- Added comments ---  czogby [2014/11/17 19:44:53Z]
**   No changes.
**   Revision 1.2 2014/09/24 16:25:33EDT czogby 
**   Updates from code inspection.
**   Revision 1.1 2014/08/26 15:38:26EDT czogby 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
*************************************************************************/

#ifndef _sca_msgdefs_
#define _sca_msgdefs_
/************************************************************************
** Macro Definitions
*************************************************************************/

/**
** \name constants for Reset Counters Command for which counters to reset
*/
#define SCA_ALL_COUNTERS       0
#define SCA_CMD_COUNTERS_ONLY  1
#define SCA_ATS_COUNTERS_ONLY  2

/**
** \name constants for config parameters for which TIME to use */
/** \{ */
#define SCA_USE_CFE_TIME      0      /**< \brief Use cFE configured time */
#define SCA_USE_TAI           1      /**< \brief Use TAI Time */
#define SCA_USE_UTC           2      /**< \brief USE UTC Time */
/** \} */

/************************************************************************
** Command Code Definitions
*************************************************************************/

/** \scacmd Noop
**
**  \par Description
**       This command performs no operation other than to generate an informational
**       event that also contains the SCA application version data. The command is
**       most often used as a general aliveness test by demonstrating that the SCA
**       application can receive commands and generate telemetry.
**
**  \scacmdmnemonic \SCA_NOOP
**
**  \par Command Structure
**       #SCA_NoArgsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \SCA_CMDPC - command counter will increment
**       - The #SCA_NOOP_INF_EID informational event message will be
**         sent
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \SCA_CMDEC - command error counter will increment
**       - #SCA_LEN_ERR_EID error specific event message
**
**  \par Criticality
**       None
**
**  \sa #SCA_NOOP_CC
*/
#define SCA_NOOP_CC            0

/** \scacmd Reset Counters
**
**  \par Description
**       This command resets the command specified SCA housekeeping counters.
**       Command options include:
**       <ul> <li> resetting all counters (ResetFlag set to 0) </li>
**            <li> resetting the SCA command and command error counters only (Reset Flag set to 1) </li>
**            <li> resetting the ATS command and command error counters only (Reset Flag set to 2) </li>
**       </ul><BR>
**
**       This command accepts one unsigned 8-bit integer as a command argument. The command argument specifies
**       the value of the Reset Flag. Setting the argument to the value 0 will reset all counters.  Setting the
**       argument to the value of 1 will reset the SCA command and command error counters only.  Setting the
**       argument to 2 will reset the ATS command and command error counters only.  All other argument values
**       will be ignored.
**
**  \scacmdmnemonic \SCA_RESETCTRS
**
**  \par Command Structure
**       #SCA_ResetCntrsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - The command specified housekeeping telemetry counters will be set to zero
**       - The #SCA_RESET_ALL_COUNTERS_DBG_EID, #SCA_RESET_CMD_COUNTERS_DBG_EID, or
**         #SCA_RESET_ATS_COUNTERS_DBG_EID debug event message will be
**         generated when the command is successfully executed
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \SCA_CMDEC - command error counter will increment
**       - #SCA_LEN_ERR_EID error specific event message
**
**  \par Criticality
**       None
**
**  \sa #SCA_RESET_COUNTERS_CC
*/
#define SCA_RESET_COUNTERS_CC            1

/** \scacmd Start ATS
**
**  \par Description
**       This command starts the command specified ATS file.  This command will
**       send out the ATS file specified commands over the software bus at the file
**       specified absolute time.  Each command within the file includes a
**       Boolean “force” flag indicating that the command should be executed if its
**       absolute time tag has passed.  Each command also includes a timeout (in seconds)
**       after which the force flag is ignored. <BR>
**
**       This command accepts one character array as a command argument (sized to
**       the #OS_MAX_PATH_LEN configuration parameter setting).  The command
**       argument specifies the full path specification of the ATS file to start.
**
**  \scacmdmnemonic \SCA_STARTATS
**
**  \par Command Structure
**       #SCA_StartAtsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \SCA_CMDPC       - command counter will increment
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - ATS file is currently in use
**       - Max number of ATS are currently running
**       - ATS child task failed to be created
**       - ATS file failed to open
**       - ATS file failed to read
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \SCA_CMDEC - command error counter will increment
**       - One of the following error specifc event messages will be sent:
**         <ul> <li> #SCA_LEN_ERR_EID </li>
**              <li> #SCA_ATS_START_ATS_IN_USE_ERR_EID </li>
**              <li> #SCA_STARTATS_CMD_MAX_ATS_ERR_EID </li>
**              <li> #SCA_STARTATS_CMD_CREAT_CHILD_ERR_EID </li>
**              <li> #SCA_STARTATS_CMD_FILEOPEN_ERR_EID </li>
**              <li> #SCA_STARTATS_CMD_READFILE_ERR_EID </li></ul>
**
**  \par Criticality
**       Caution is advised in the use of this command. The SCA application will perfom
**       no verification of the file and/or the commands within the file.  It is the
**       responsibility of the user to verify the integrity of the command specified file
**       on the ground before issuing this command.
**
**  \sa #SCA_STOP_ATS_CC
*/
#define SCA_START_ATS_CC                 2

/** \scacmd Stop ATS
**
**  \par Description
**       This command stops the command specified ATS file from executing. <BR>
**
**       This command accepts one character array as a command argument (sized to
**       the #OS_MAX_PATH_LEN configuration parameter setting).  The command
**       argument specifies the full path specification of the ATS file to stop.
**
**  \scacmdmnemonic \SCA_STOPATS
**
**  \par Command Structure
**       #SCA_StopAtsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \SCA_CMDPC       - command counter will increment
**       - One of the following informational event messages will be sent:
**         <ul> <li> #SCA_STOPATS_CMD_INF_EID </li>
**              <li> #SCA_STOPATS_NOTRUNNING_INF_EID </li> </ul>
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Command specified ATS file is not running
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \SCA_CMDEC - command error counter will increment
**       - #SCA_LEN_ERR_EID error specific event message
**
**  \par Criticality
**       None
**
**  \sa #SCA_START_ATS_CC
*/
#define SCA_STOP_ATS_CC                   3


/** \scacmd Get ATS Information
**
**  \par Description
**       This commands writes information on each executing ATS to the
**       command specified output file in binary format.  If there are
**       no ATS executing, the output data file will not be generated.
**       The following set of information on each executing ATS will
**       be written to the output file: <BR>
**
**       <ul> <li> Full path specification of executing ATS file </li>
**            <li> Name of the child task main function running the ATS </li>
**            <li> Child task run status </li>
**            <li> Child task ID</li>
**            <li> Command Status - specifies if last line read from the file is a command or not </li>
**            <li> Command Sequence Number - specifies the sequence number of the next command to be sent </li>
**            <li> Force Flag setting of next command to be sent </li>
**            <li> Timeout period of next command to be sent </li>
**            <li> Time of next command to be sent </li>
**            <li> Last Spacecraft time value queried by the child task </li></ul>
**
**  \scacmdmnemonic \SCA_GETATSINFO
**
**  \par Command Structure
**       #SCA_GetATSInfoCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \SCA_CMDPC       - command counter will increment
**       - One of the following informational event messages will be sent:
**         <ul> <li> #SCA_ATSINFO_WRITTEN_EID </li>
**              <li> #SCA_ATSINFO_NOFILE_EID </li> </ul>
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Command packet length not as expected
**       - Output file failed to create
**       - Failed to write data to output file
**
**  \par Evidence of failure may be found in the following telemetry:
**       - \b \c \SCA_CMDEC - command error counter will increment
**       - One of the following error specifc event messages will be sent:
**         <ul> <li> #SCA_LEN_ERR_EID </li>
**              <li> #SCA_ATSINFO_WRITEFILE_ERR_EID </li>
**              <li> #SCA_ATSINFO_CREATFILE_ERR_EID </li></ul>
**
**  \par Criticality
**       None
**
**  \sa #SCA_START_ATS_CC
*/
#define SCA_GET_ATS_INFO_CC               4


#endif /* _sca_msgdefs */

/************************/
/*  End of File Comment */
/************************/
