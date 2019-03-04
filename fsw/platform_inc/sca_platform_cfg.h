 /*************************************************************************
 ** File:
 **   $Id: sca_platform_cfg.h 1.4 2015/03/26 21:13:22EDT czogby Exp  $
 **
 ** Purpose:
 **   This file contains the platform configuration parameters used by
 **   Stored Command Absolute
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_platform_cfg.h  $
 **   Revision 1.4 2015/03/26 21:13:22EDT czogby 
 **   Adjust SCA Time Drift Configuration Parameter
 **   Revision 1.3 2014/11/17 12:03:48EST czogby 
 **   --- Added comments ---  sstrege [2014/11/17 19:08:46Z]
 **   Added SCA_MAX_CMD_PEND_DELAY parameter
 **   Revision 1.2 2014/09/24 16:25:32EDT czogby 
 **   Updates from code inspection.
 **   Revision 1.1 2014/08/26 15:38:21EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/platform_inc/project.pj
 *************************************************************************/
#ifndef _sca_platform_cfg_
#define _sca_platform_cfg_

/*************************************************************************
 ** Macro Definitions
 *************************************************************************/

/** \scacfg Command Pipe Depth
**
**  \par Description:
**       Maximum number of messages that will be allowed in the
**       SCA command pipe at one time. Used during initialization
**       in the call to #CFE_SB_CreatePipe
**
**  \par Limits:
**       This parameter must be greater than zero and less than or equal to
**       CFE_SB_MAX_PIPE_DEPTH.
*/
#define SCA_PIPE_DEPTH                   12

/** \scacfg Defines the TIME SCA should use when processing ATS commands
**
**
**  \par Description:
**       This parameter defines what type of time SCA should use when sending out
**       ATS commands
**
**  \par Limits:
**       Must be SCA_USE_CFE_TIME, SCA_USE_TAI, or SCA_USE_UTC
*/
#define SCA_TIME_TO_USE                  SCA_USE_CFE_TIME

/** \scacfg Mission specific version number for SCA application
**
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "sca_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define SCA_MISSION_REV                  0

/** \scacfg ATS Child Task Stack Size
**
**  \par Description:
**       Stack size of child tasks created by the SCA parent application.  The
**       parent spawns a child task to process a commanded ATS file.
**
**  \par Limits:
**       It is recommended this parameter be greater than or equal to 4KB. This parameter
**       is limited by the maximum value allowed by the data type. In this case, the data
**       type is an unsigned 32-bit integer, so the valid range is 0 to 0xFFFFFFFF.
**
*/
#define SCA_ATS_CHILD_STACK_SIZE          8192

/** \scacfg ATS Child Task Priority
**
**  \par Description:
**       Priority of child tasks created by the SCA parent application.
**       Lower numbers are higher priority, with 1 being the highest priority
**       in the case of a child task.
**
**  \par Limits:
**       Valid range for a child task is 1 to 255 however, the priority cannot
**       be higher (lower number) than the SCA parent application priority.
*/
#define SCA_ATS_CHILD_TASK_PRIORITY        86

/** \scacfg Maximum ATS File Line Length
**
**  \par Description:
**      Defines the maximum number of ASCII characters (including hex characters,
**      non-hex characters, and delimiters) that are allowed on a single line
**      within an SCA Start ATS command input file.
**
**  \par Limits:
**       It is recommended this parameter be greater than or equal to 30 to
**       allow character space on a single line for the time tag, force flag,
**       timeout value, simple ASCII Hex command with no parameters, and
**       delimiters.  There is no limit verificaton performed on this
**       parameter.
**
*/
#define SCA_MAX_FILE_LINE_LENGTH           512

/** \scacfg Maximum Command Packet Length
**
**  \par Description:
**       Defines the maximum size of a CCSDS command packet that the SCA application
**       ATS child tasks will send out over the software bus.
**
**       Note:
**
**  \par Limits:
**       There is not a direct correlation between the SCA_MAX_FILE_LINE_LENGTH
**       and the SCA_MAX_CMD_PACKET_LENGTH since the file can contain a mix of hex
**       characters, non-hex characters, and delimiters.  However, the bytes in the
**       command packet are constructed from the hex characters contained on a single
**       line in the input file.  Therefore the number of bytes in a command packet
**       cannot be any bigger than SCA_MAX_FILE_LINE_LENGTH/2 which is a good,
**       recommended size for SCA_MAX_CMD_PACKET_LENGTH.  There is no limit verificaton
**       performed on this parameter.
**
*/
#define SCA_MAX_CMD_PACKET_LENGTH          256

/** \scacfg Default ATSInfo Filename
**
**  \par Description:
**      Defines the default filename used by SCA_GetATSInfo if no filename is specified in the command.
**
**  \par Limits:
**       Must be defined as a string between quotation marks.  Path must not be longer than OS_MAX_PATH_LEN.
*/
#define SCA_DEFAULT_ATSINFO_FILENAME       "/ram/sca_atsinfo.txt"

/** \scacfg Maximum Pending Command Delay
**
**  \par Description:
**       Defines the maximum time that a child process can sleep for when waiting for the specified time 
**       to send a CCSDS command packet.  Increasing this number reduces the frequency of child process wakeups, which can save processor time.  
**       Chosen based on trial-and-error - in the pc-linux environment, choosing
**       10000 did not cause time drift, while choosing higher values occasionally did.
**
**  \par Limits:
**       This parameter is limited by the maximum value allowed by the data type. In this case, the data type is 
**       an unsigned 32-bit integer, so the valid range is 0 to 0xFFFFFFFF.
*/
#define  SCA_MAX_CMD_PEND_DELAY   10000

#endif /*_sca_platform_cfg_*/

/************************/
/*  End of File Comment */
/************************/

