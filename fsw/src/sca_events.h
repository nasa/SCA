 /*************************************************************************
 ** File:
 **   $Id: sca_events.h 1.3 2014/11/17 12:14:48EST czogby Exp  $
 **
 ** Purpose:
 **   This file contains the definitions of all of the events sent by
 **   the Stored Command Absolute processor application
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_events.h  $
 **   Revision 1.3 2014/11/17 12:14:48EST czogby 
 **   --- Added comments ---  czogby [2014/11/17 19:47:09Z]
 **   Fixed description of SCA_SENDCMD_TIME_DRIFT_INFO_EID.
 **   Revision 1.2 2014/09/24 16:25:34EDT czogby 
 **   Updates from code inspection.
 **   Revision 1.1 2014/08/26 15:38:25EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
 *************************************************************************/
#ifndef _sca_events_
#define _sca_events_

/** \brief <tt> 'App terminating' </tt>
**  \event <tt> 'App terminating, Result = 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when the App exits
**  due to a fatal error condition
**
**  The \c Result field contains the return status from
**  the function that caused the app to terminate
*/
#define SCA_APP_EXIT_ERR_EID                                 1


/** \brief <tt> 'Invalid message length received' </tt>
**  \event <tt> 'Invalid msg length: ID = 0x\%04X, CC = \%d, Len = \%d, Expected = \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a command is recieved, but it is not of the expected
**  length
**
**  The \c ID field contains the message Id of the message
**  The \c CC field contains the command code of the message
**  The \c Len field contains the length of the message received
**  The \c Expected field contains the expected length of the command
*/
#define SCA_LEN_ERR_EID                                      2


/** \brief <tt> 'Software Bus error received creating command pipe' </tt>
**  \event <tt> 'Software Bus Create Pipe returned: 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when #CFE_SB_CreatePipe returns an
**  error
**
**  The \c returned field contains the result of the #CFE_SB_CreatePipe
**  call
*/
#define SCA_INIT_SB_CREATE_ERR_EID                           3


/** \brief <tt> 'Software Bus error received subscribing to housekeeping message' </tt>
**  \event <tt> 'Software Bus subscribe to housekeeping returned: 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when #CFE_SB_Subscribe to the Housekeeping
**  Request packet fails
**
**  The \c returned field contains the result of the #CFE_SB_Subscribe call
*/
#define SCA_INIT_SB_SUBSCRIBE_HK_ERR_EID                     4


/** \brief <tt> 'Software Bus subscribe to command returned an error' </tt>
**  \event <tt> 'Software Bus subscribe to command returned: 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when #CFE_SB_Subscribe to the SCA Command
**  Request packet fails
**
**  The \c returned field contains the result of the #CFE_SB_Subscribe call
*/
#define SCA_INIT_SB_SUBSCRIBE_CMD_ERR_EID                    5


/** \brief <tt> 'SCA Initialized. Version #.#.#.#' </tt>
**  \event <tt> 'SCA Initialized. Version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when the App has
**  completed initialization.
**
**  The \c Version fields contain the #SCA_MAJOR_VERSION,
**  #SCA_MINOR_VERSION, #SCA_REVISION, and #SCA_MISSION_REV
**  version identifiers.
*/
#define SCA_INIT_INFO_EID                                    6

/** \brief <tt> 'ATS Child Task Created' </tt>
**  \event <tt> 'Started ATS Child Task: \%s' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when an ATS child task is started
**  successfully
**
**  The \c field contains the file name of the ATS that was started
*/
#define SCA_STARTATS_CMD_CREAT_CHILD_INFO_EID                7


/** \brief <tt> 'ATS stopped' </tt>
**  \event <tt> 'ATS \%s stopped' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when a #SCA_STOP_ATS_CC command successfully
**  stopped an ATS
**
**  The \c field contains the name of the ATS file that was stopped
*/
#define SCA_STOPATS_CMD_INFO_EID                              8


/** \brief <tt> 'ATS is not running' </tt>
**  \event <tt> 'ATS %s is not running' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when a #SCA_STOP_ATS_CC command was issued
**  but the command specified ATS was not running
*/
#define SCA_STOPATS_NOTRUNNING_INFO_EID                       9

/** \brief <tt> 'ATS command failed to send on software bus' </tt>
**  \event <tt> '%s command %i failed to send on software bus: 0x\%08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS command is about to be sent out,
**  and the #CFE_SB_SendMsg call failed to send it
**
**  The \c fields contain the the name of the file containing the failed
**  command, the sequence number of the command that failed, and the error
**  code that was returned from the call to the #CFE_SB_SendMsg function.
*/
#define SCA_ATS_SB_SEND_FAIL_ERR_EID                         10


/** \brief <tt> 'Reset counters command' </tt>
**  \event <tt> 'Reset counters command: reset all counters' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when the #SCA_RESET_COUNTERS_CC command was received
**  with the request to reset all counters
*/
#define SCA_RESET_ALL_COUNTERS_DBG_EID                       11


/** \brief <tt> 'No-op command. Version #.#.#.#' </tt>
**  \event <tt> 'No-op command. Version \%d.\%d.\%d.\%d' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**
**  This event message is issued when a #SCA_NOOP_CC command has been received
**
**  The \c Version fields contain the #SCA_MAJOR_VERSION,
**  #SCA_MINOR_VERSION, #SCA_REVISION, and #SCA_MISSION_REV
**  version identifiers.
*/
#define SCA_NOOP_INFO_EID                                     12

/** \brief <tt> 'Invalid software bus pipe message ID received' </tt>
**  \event <tt> 'Invalid software bus pipe message ID: 0x%\08X' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an invalid message Id is recieved in the
**  command pipe
**
**  The \c field contains the erroneous message ID
*/
#define SCA_MID_ERR_EID                                      13


/** \brief <tt> 'Invalid Command Code: MID =  0x\%04X CC =  \%d' </tt>
**  \event <tt> 'Invalid Command Code: MID =  0x\%04X CC =  \%d' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an invalid command code was recieved in
**  the command pipe
**
**  The MID \c field contains the message Id for the command, and the \c CC
**  field contains the erroneous command code
*/
#define SCA_INVLD_CMD_ERR_EID                                14

/** \brief <tt> 'ATS Execution Completed' </tt>
**  \event <tt> 'ATS \%s Execution Completed' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when an ATS completes execution
**
**  The \c field contains the name of the ATS file that completed
*/
#define SCA_ATS_COMPL_INFO_EID                                15

/** \brief <tt> 'Start ATS Rejected: Max number of ATS executing' </tt>
**  \event <tt> 'Start ATS Rejected: Max number of ATS executing' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SCA_START_ATS_CC command was
**  issued but there is already the maximum number of ATS executing
*/
#define SCA_STARTATS_CMD_MAX_ATS_ERR_EID                    16

/** \brief <tt> 'Start ATS Rejected: Child Task Creation Failure' </tt>
**  \event <tt> 'Start ATS Rejected: Child Task Creation Failure: 0x\%x, \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SCA_START_ATS_CC command failed because
**  the child task could not successfully be created.
**
**  The \c fields contains the error code that was returned from the call to
**  the CFE_ES_CreateChildTask function and the name of the child task that
**  failed to be created.
*/
#define SCA_STARTATS_CMD_CREAT_CHILD_ERR_EID                17

/** \brief <tt> 'Start ATS Rejected: File Open Failure' </tt>
**  \event <tt> 'Start ATS Rejected: File Open Failure: 0x\%x, \%s, \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SCA_START_ATS_CC command failed because
**  the specified ATS file could not be opened.
**
**  The \c fields contain the error code that was returned from the call to
**  the OS_open function, the name of the child task that failed to open the
**  file, and the name of the file that failed to be opened.
*/
#define SCA_STARTATS_CMD_FILEOPEN_ERR_EID                    18

/** \brief <tt> 'ATS Stopped: Read File Error' </tt>
**  \event <tt> 'ATS Stopped: Read File Error: 0x\%x, \%s, \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SCA_START_ATS_CC command failed
**  because the specified ATS file could not be read.
**
**  The \c fields contain the error code that was returned from the call to
**  the OS_read function, the name of the child task that failed to open the
**  file, and the name of the file that failed to be read.
*/
#define SCA_STARTATS_CMD_READFILE_ERR_EID                    19

/** \brief <tt> 'Get ATS Info Rejected: Write File Error' </tt>
**  \event <tt> 'Get ATS Info Rejected: Write File Error: 0x\%x, \%s, \%s' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SCA_GET_ATS_INFO_CC command failed
**  because the specified output file could not be written.
**
**  The \c fields contain the error code that was returned from the call to
**  the OS_write function, the name of the child task that failed to write the
**  file, and the name of the output file that failed to be written to.
*/
#define SCA_ATSINFO_WRITEFILE_ERR_EID                        20

/** \brief <tt> 'Get ATS Info: Command Success' </tt>
**  \event <tt> 'Get ATS Info: \%i Bytes written to \%s' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when a #SCA_GET_ATS_INFO_CC command
**  successfully completed writing all executing ATS information to
**  the command specified file.
**
**  The \c fields contain the number of bytes written to the output file
**  and the name of the output file.
*/
#define SCA_ATSINFO_WRITTEN_EID                              21

/** \brief <tt> 'Get ATS Info: No Running ATS, data file not created' </tt>
**  \event <tt> 'Get ATS Info: No Running ATS, data file not created' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when a #SCA_GET_ATS_INFO_CC command
**  did not create and write to an output data file because there were
**  no ATS executing to report information on.
**
*/
#define SCA_ATSINFO_NOFILE_EID                               22

/** \brief <tt> 'Get ATS Info: Failed to create file' </tt>
**  \event <tt> 'Get ATS Info: Failed to create file \%s: err = 0x\%x' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SCA_GET_ATS_INFO_CC command
**  failed because there was an error creating the output data file.
**
**  The \c fields contain the name of the output file that could not be
**  created and the error code that was returned from the call to
**  the OS_creat function.
*/
#define SCA_ATSINFO_CREATFILE_ERR_EID                        23

/** \brief <tt> 'ATS already in use' </tt>
**  \event <tt> 'ATS \%s already in use' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when a #SCA_START_ATS_CC command failed
**  because the specified ATS file is currently in use.
**
**  The \c fields contain the name of the ATS file that failed
*/
#define SCA_ATS_START_ATS_IN_USE_ERR_EID                     24

/** \brief <tt> 'Get ATS Info: Error closing file' </tt>
**  \event <tt> 'Get ATS Info: Error closing file \%s, err = 0x\%x' </tt>
**
**  \par Type: CRITICAL ERROR
**
**  \par Cause:
**  This event message is issued when a #SCA_GET_ATS_INFO_CC command failed
**  to close the command specified ATS information file.  This error indicates
**  a critical system error has occurred.
**
**  The \c fields contain the name of the file that failed to close and the
**  error code that was returned from the call to the OS_close function.
*/
#define SCA_ATSINFO_CLOSE_FILE_CRITICAL_EID                       25

/** \brief <tt> 'Get ATS Info: Error closing file' </tt>
**  \event <tt> 'Start ATS: Error closing file %s, %i, %s, %i' </tt>
**
**  \par Type: CRITICAL ERROR
**
**  \par Cause:
**  This event message is issued when a #SCA_START_ATS_CC command failed
**  to close the command specified ATS information file.  This error indicates
**  a critical system error has occurred.
**
**  The \c fields contain the name of the file that failed to close, the
**  value of the file descriptor, the name of the child task that failed
**  to close the file, and the error code that was returned from the call
**  to the OS_close function.
*/
#define SCA_STARTATS_CLOSE_FILE_CRITICAL_EID                 26

/** \brief <tt> 'Reset counters command' </tt>
**  \event <tt> 'Reset counters command: reset command counters' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when the #SCA_RESET_COUNTERS_CC command was received
**  with the request to reset the application command counters only
*/
#define SCA_RESET_CMD_COUNTERS_DBG_EID                       30

/** \brief <tt> 'Reset counters command' </tt>
**  \event <tt> 'Reset counters command: reset ATS counters' </tt>
**
**  \par Type: DEBUG
**
**  \par Cause:
**  This event message is issued when the #SCA_RESET_COUNTERS_CC command was received
**  with the request to reset the ATS counters only
*/
#define SCA_RESET_ATS_COUNTERS_DBG_EID                       31

/** \brief <tt> 'ATS command skipped' </tt>
**  \event <tt> 'ATS \%s command \%i skipped' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS command has been skipped.  ATS
**  commands will skip when the force flag is set to FALSE and the current
**  time is greater than the command time or the command has timed out.
**
**  The \c fields contain the name of the ATS file containing the skipped
**  command and the sequence number of the skipped command.
*/
#define SCA_ATS_SB_SKIPPED_CMD_ERR_EID                       32

/** \brief <tt> 'ATS command bad parameter string' </tt>
**  \event <tt> 'ATS Stopped: missing end quote in %s, command %i' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when an ATS command has a string parameter 
**  that's missing a closing quotation mark.
**
**  The \c fields contain the name of the ATS file containing the
**  command and the sequence number of the command.
*/
#define SCA_STARTATS_CMD_READFILE_BAD_STRING_ERR_EID         33

/** \brief <tt> 'ATS read command error' </tt>
**  \event <tt> 'ATS %s command %i error reading from file' </tt>
**
**  \par Type: ERROR
**
**  \par Cause:
**  This event message is issued when there is an error reading an ATS command.
**
**  The \c fields contain the name of the ATS file containing the
**  command and the sequence number of the command.
*/
#define SCA_ATS_READ_CMD_ERR_EID         34

/** \brief <tt> 'Report detected time drift while waiting to send a command' </tt>
**  \event <tt> 'Time drift detected.' </tt>
**
**  \par Type: INFORMATIONAL
**
**  \par Cause:
**  This event message is issued when the current time in seconds does not match the expected 
**  time after a specific delay.  This could be caused by time drift or the time being jammed.
*/
#define SCA_SENDCMD_TIME_DRIFT_INFO_EID         35

#endif /*_sca_events_*/

/************************/
/*  End of File Comment */
/************************/
