/*************************************************************************
 ** File:
 **   $Id: sca_child.h 1.3 2014/11/17 12:14:34EST czogby Exp  $
 **
 ** Purpose: 
 **     This file is for SCA child processes
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_child.h  $
 **   Revision 1.3 2014/11/17 12:14:34EST czogby 
 **   --- Added comments ---  czogby [2014/11/17 20:15:11Z]
 **   Added procedure SCA_ChildSleepUntilCommandTime, added definitions for semaphores, and removed unneeded comments.
 **   Revision 1.2 2014/09/24 16:25:36EDT czogby 
 **   Updates from code inspection.
 **   Revision 1.1 2014/08/26 15:38:24EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
 
 *************************************************************************/

#ifndef _sca_child_
#define _sca_child_

#include "sca_utils.h"

/**
** \name SCA Child no error return value */ 
/** \{ */
#define SCA_CHILD_NO_ERROR           0
/** \} */

/**
** \name SCA Child error return value */ 
/** \{ */
#define SCA_CHILD_ERROR                -1
/** \} */

/**
** \name SCA child process index names */ 
/** \{ */
#define SCA_CHILD_1      0
#define SCA_CHILD_2      1
#define SCA_CHILD_3      2
#define SCA_CHILD_4      3
#define SCA_CHILD_5      4
/** \} */

/**
** \name SCA child process status */ 
/** \{ */
#define SCA_CHILD_NOT_USED   0
#define SCA_CHILD_EXECUTING  1
/** \} */

/**
** \name SCA "no free child process" return status */ 
/** \{ */
#define SCA_NO_FREE_CHILD    0xFF
/** \} */

/**
** \name SCA child process name length */ 
/** \{ */
#define SCA_CHILD_NAME_SIZE  11
/** \} */

/**
** \name SCA max number of ATSs */ 
/** \{ */
#define SCA_MAX_NUM_ATS  5
/** \} */

/**
** \name SCA child process names */ 
/** \{ */
#define SCA_CHILD_1_NAME  "SCA_CHILD1"
#define SCA_CHILD_2_NAME  "SCA_CHILD2"
#define SCA_CHILD_3_NAME  "SCA_CHILD3"
#define SCA_CHILD_4_NAME  "SCA_CHILD4"
#define SCA_CHILD_5_NAME  "SCA_CHILD5"
/** \} */

/**
** \name SCA child process misc values */ 
/** \{ */
#define SCA_CHILD_STACK_PTR   0
#define SCA_CHILD_FLAGS       0
/** \} */

/**
** \name SCA ATS file processing status */ 
/** \{ */
#define SCA_NON_COMMAND 0
#define SCA_COMMAND     1
#define SCA_EOF         2
#define SCA_FS_ERROR    3
/** \} */

/**
** \name SCA child process mutex and semaphore naming */
/** \{ */
#define  SCA_CHILD_MUTEX_NAME_LENGTH      16
#define  SCA_CHILD_MUTEX_BASE_NAME        "SCA_MUTEX_CHILD"
#define  SCA_CHILD_COPY_MUTEX_BASE_NAME   "SCA_MTX_COPY_CH"

#define  SCA_CHILD_PENDING_CMD_SEMAPHORE_NAME_LENGTH  19
#define  SCA_CHILD_PENDING_CMD_SEMAPHORE_BASE_NAME    "SCA_SEM_PND_CH"
/** \} */

/**
** \name SCA child process data mutex options */ 
/** \{ */
#define  SCA_CHILD_MUTEX_OPTIONS   0                        /**< \brief Child data access mutex options */
/** \} */

/**
** \name Used to specify base in calls to strtoul */ 
/** \{ */
#define  BASE16   16       /**< \brief Specifies base-16 */
/** \} */

/* 
**  SCA Child Info Data Structure
*/
typedef struct
{
    char                  ChildAtsFile[OS_MAX_PATH_LEN];            /**< \brief Child Task ATS Filename*/
    char                  ChildName[SCA_CHILD_NAME_SIZE];           /**< \brief Child Task Main Function Name*/
    uint32                RunStatus;                                /**< \brief Child Task run status */
    uint32                ChildTaskID;                              /**< \brief Child Task ID */
    int32                 FileDescriptor;
    uint8                 CommandStatus;
    uint32                CommandSequenceNumber;
    boolean               ForceFlag;
    SCA_AbsTimeTag_t      TimeoutPeriod;
    SCA_AbsTimeTag_t      TimeNextCmd;
    SCA_AbsTimeTag_t      CurrentTime_Last;
    boolean               StopATS;
    uint32                DataMutex;                                /**< \brief Mutex to protect child data */
    uint32                OutputDataMutex;                          /**< \brief Mutex to protect child data */
    uint32                PendingCmdSemaphore;                      /**< \Used to create delay when waiting for command time */

} SCA_ChildInfo_t;

/* 
**  SCA Child Info Data Structure
*/
typedef struct
{
    char                  ChildAtsFile[OS_MAX_PATH_LEN];            /**< \brief Child Task ATS Filename*/
    char                  ChildName[SCA_CHILD_NAME_SIZE];           /**< \brief Child Task Main Function Name*/
    uint32                RunStatus;                                /**< \brief Child Task run status */
    uint32                ChildTaskID;                              /**< \brief Child Task ID */
    uint8                 CommandStatus;
    uint32                CommandSequenceNumber;
    boolean               ForceFlag;
    SCA_AbsTimeTag_t      TimeoutPeriod;
    SCA_AbsTimeTag_t      TimeNextCmd;
    SCA_AbsTimeTag_t      CurrentTime_Last;

} SCA_ChildInfoOutput_t;

/************************************************************************/
/** \brief Process ATS file
 **  
 **  \par Description
 **            This routine parses commands from an ATS file and sends each command to 
 **            the software bus at the time specified for the command.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         ChildInfoIndex  a uint8 that specifies a child process
 **
 **  \returns
 **  \retcode #SCA_NO_ERROR \retdesc \copydoc SCA_NO_ERROR \endcode
 **  \retstmt Return status \endcode
 **  \endreturns
 **
 *************************************************************************/
int16 SCA_ProcessFile( uint8 ChildInfoIndex );

/************************************************************************/
/** \brief Get next command from ATS file
 **  
 **  \par Description
 **            This routine parses the next command from the ATS file.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         ChildInfoIndex  a uint8 that specifies a child process
 **  \param [in]         CCSDS_Packet    a uint8 array containing the packet data
 **
 **  \returns
 **  \retcode #SCA_NO_ERROR \retdesc \copydoc SCA_NO_ERROR \endcode
 **  \retstmt Return status \endcode
 **  \endreturns
 **
 *************************************************************************/
int16 SCA_GetNextCommand( uint8 ChildInfoIndex, uint8 *CCSDS_Packet );

/************************************************************************/
/** \brief Process all command parameters and copy to the CCSDS packet
 **  
 **  \par Description
 **            This routine processes a command parameter and copies it to the CCSDS packet.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         ChildInfoIndex  a uint8 that specifies a child process
 **  \param [in]         CCSDS_Packet    a uint8 array containing the packet data
 **  \param [in]         DataString      a string containing the packet data
 **
 **  Parses the parameters from DataString and then copies them into the CCSDS packet. This is necessary because
 **  a.) Little endian machines need bytes reversed for each data item (except strings), and
 **  b.) The need to handle little endian machines caused the file format to need to be the way it is
 **      with each parameter separated, rather than just writing the parameters all together in their final CCSDS format.
 **
 **  \returns
 **  \retcode #SCA_NO_ERROR \retdesc \copydoc SCA_NO_ERROR \endcode
 **  \retstmt Return status \endcode
 **  \endreturns
 **
 *************************************************************************/
uint16 SCA_ProcessAllParameters( uint8 ChildInfoIndex, uint8 *CCSDS_Packet, char *DataString );

/************************************************************************/
/** \brief Process a command parameter and copy it to the CCSDS packet
 **  
 **  \par Description
 **            This routine processes a command parameter and copies it to the CCSDS packet.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 **  \param [in]         ChildInfoIndex         a uint8 that specifies a child process
 **  \param [in]         CCSDS_Packet           a uint8 array containing the packet data
 **  \param [in]         DataString             a string containing the packet data
 **  \param [in]         FirstIndex             a uint16 that specifies the first index into DataString of the parameter
 **  \param [in]         LastIndex              a uint16 that specifies the last index into DataString of the parameter
 **  \param [in]         CCSDS_LastByteWritten  a uint16 that specifies the index of the most recent byte read from DataString
 **
 *************************************************************************/
void SCA_ProcessParameter( uint8 ChildInfoIndex, uint8 *CCSDS_Packet, char *DataString, uint16 FirstIndex, uint16 LastIndex, uint16 *CCSDS_LastByteWritten );

/************************************************************************/
/** \brief Send a command in a CCSDS packet
 **  
 **  \par Description
 **            This routine sends a passed-in CCSDS packet.
 **       
 **  \par Assumptions, External Events, and Notes:
 **            The timing of the packet is not handled in this function - the 
 **            packet is sent immediately when the function is called.
 **
 **  \param [in]         ChildInfoIndex  a uint8 that specifies a child process
 **  \param [in]         CCSDS_Packet    a uint8 array containing the packet data
 **
 **  \returns
 **  \retcode #SCA_NO_ERROR \retdesc \copydoc SCA_NO_ERROR \endcode
 **  \retstmt Return status \endcode
 **  \endreturns
 **
 *************************************************************************/
int16 SCA_SendCommand ( uint8 ChildInfoIndex, uint8 *CCSDS_Packet );

/************************************************************************/
/** \brief Attempts to sleep the process until the time of the pending command.
 **  
 **  \par Description
 **            This routine puts the process to sleep until the time of the pending command, 
 **            unless the time until the pending command is greater than the maximum allowable 
 **            delay, in which case it puts the process to sleep for the maximum allowable delay.
 **       
 **  \par Assumptions, External Events, and Notes:
 **            None
 **
 *************************************************************************/
void SCA_ChildSleepUntilCommandTime( uint8 ChildInfoIndex );

/************************************************************************/
/** \brief Top-level functions for running an ATS in a child process
 **  
 **  \par Description
 **            One of these routines is started in a new child process when an ATS (1 through 5) is 
 **            started.  When the routine exits, the child process is deleted.
 **       
 **  \par Assumptions, External Events, and Notes:
 **        None
 **
 *************************************************************************/
void SCA_Child1Main( void );
void SCA_Child2Main( void );
void SCA_Child3Main( void );
void SCA_Child4Main( void );
void SCA_Child5Main( void );

/************************************************************************/
/** \brief Find the lowest-indexed unused ATS child process
 **  
 **  \par Description
 **            This routine finds the lowest-indexed unused ATS child 
 **            process and returns its index.
 **       
 **  \par Assumptions, External Events, and Notes:
 **            None.
 **
 **  \returns
 **  \retstmt Free Child Index \endcode
 **  \endreturns
 **
 *************************************************************************/
uint8 SCA_FindFreeChild( void );

/************************************************************************/
/** \brief Clear info for specified ATS child process
 **  
 **  \par Description
 **            This routine clears the ChildInfo data structure for 
 **            the specified ATS child process.
 **       
 **  \par Assumptions, External Events, and Notes:
 **            None.
 **
 **  \param [in]         ChildInfoIndex  a uint8 that specifies a child process
 **
 *************************************************************************/
void SCA_ClearChildInformation(uint8 ChildInfoIndex);

/************************************************************************/
/** \brief Copy info of specified ATS child process into output structure
 **  
 **  \par Description
 **            This routine copies the ChildInfo data structure of specified ATS 
 **            child process into the output data structure ChildInfoOutput. This 
 **            is done to avoid mutex conflicts.
 **       
 **  \par Assumptions, External Events, and Notes:
 **            None.
 **
 **  \param [in]         ChildInfoIndex  a uint8 that specifies a child process
 **
 *************************************************************************/
void SCA_CopyChildInformation(uint8 ChildInfoIndex);

#endif /* _sca_child_ */
