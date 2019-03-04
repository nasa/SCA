/*************************************************************************
 ** File:
 **   $Id: sca_child.c 1.6 2015/04/03 19:32:50EDT czogby Exp  $
 **
 ** Purpose: 
 **     This file contains the functions for running an SCA ATS file in a child process.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **
 ** History:
 **   $Log: sca_child.c  $
 **   Revision 1.6 2015/04/03 19:32:50EDT czogby 
 **   Change sscanf to use %hhu for force flag
 **   Revision 1.5 2015/02/24 16:38:01EST czogby 
 **   In sca_child.c, improve comment and add FIXME
 **   Revision 1.4 2014/11/17 12:14:26EST czogby 
 **   --- Added comments ---  czogby [2014/11/17 20:18:11Z]
 **   Replaced calls to OS_TaskDelay with new procedure SCA_ChildSleepUntilCommandTime, updated code for detecting time drift, and fixed other minor issues.
 **   Revision 1.3 2014/10/21 14:19:37EDT czogby 
 **   Removed command header byte swapping to make it compatible with cFE 6.4.0.
 **   Revision 1.2 2014/09/24 16:25:34EDT czogby 
 **   Updates from code inspection.
 **   Revision 1.1 2014/08/26 15:38:23EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
 *************************************************************************/

/*
** Required cFE/App HeaderHex files.ATS_Success
*/
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include "cfe.h"
#include "sca_child.h"
#include "sca_app.h"
#include "sca_events.h"
#include "sca_msg.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Process ATS file                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int16 SCA_ProcessFile( uint8 ChildInfoIndex )
{
    int16   ATS_ReturnStatus = SCA_CHILD_NO_ERROR;
    uint8   CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int32   CloseStatus = OS_SUCCESS;

    SCA_OperData.ChildInfo[ChildInfoIndex].FileDescriptor = OS_open (SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile, OS_READ_ONLY, 0);

    if( SCA_OperData.ChildInfo[ChildInfoIndex].FileDescriptor < OS_SUCCESS )
    {
        SCA_OperData.HkPacket.CmdErrCtr++;
        ATS_ReturnStatus = SCA_CHILD_ERROR;

        CFE_EVS_SendEvent( SCA_STARTATS_CMD_FILEOPEN_ERR_EID, CFE_EVS_ERROR,
                           "Start ATS Rejected: File Open Failure: 0x%08X, %s, %s",
                           SCA_OperData.ChildInfo[ChildInfoIndex].FileDescriptor,
                           SCA_OperData.ChildInfo[ChildInfoIndex].ChildName,
                           SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile );

        return ATS_ReturnStatus;
    }
    else
    {
        SCA_OperData.HkPacket.CmdCtr++;

        /*
        ** Parse the lines from the file. If it has an error
        ** or reaches EOF, then abort the loop.
        */
        while((SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus != SCA_EOF) &&
              (SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus != SCA_FS_ERROR))
        {
            OS_MutSemTake(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex); /* Protect data used by both the parent and children */

            if (SCA_OperData.ChildInfo[ChildInfoIndex].StopATS)
            {
                OS_MutSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex);
                break;
            }

            OS_MutSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex);

            ATS_ReturnStatus = SCA_GetNextCommand (ChildInfoIndex, CCSDS_Packet);

            SCA_CopyChildInformation(ChildInfoIndex);

            if (SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus == SCA_COMMAND)
            {
                ATS_ReturnStatus = SCA_SendCommand (ChildInfoIndex, CCSDS_Packet);
                SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber++;
                SCA_CopyChildInformation(ChildInfoIndex);
            }
        }
    } /* end if */

    if ((CloseStatus = OS_close(SCA_OperData.ChildInfo[ChildInfoIndex].FileDescriptor)) != OS_FS_SUCCESS)
    {
        CFE_EVS_SendEvent( SCA_STARTATS_CLOSE_FILE_CRITICAL_EID, CFE_EVS_CRITICAL,
                           "Start ATS: Error closing file %s, %i, %s, %i",
                           SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile,
                           SCA_OperData.ChildInfo[ChildInfoIndex].FileDescriptor,
                           SCA_OperData.ChildInfo[ChildInfoIndex].ChildName,
                           CloseStatus);
    }

    return ATS_ReturnStatus;
} /* End SCA_ProcessFile */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Get next command from ATS file                                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int16 SCA_GetNextCommand( uint8 ChildInfoIndex, uint8 *CCSDS_Packet )
{
    int16   i                = 0;
    int16   ATS_ReturnStatus = SCA_CHILD_NO_ERROR;
    int32   ReadStatus       = 0;
    uint8   Byte             = 0;
    uint16  Column           = 0;
    boolean InComment        = FALSE;
    int8    SscanfStatus     = 0;

    char  Line[SCA_MAX_FILE_LINE_LENGTH + 1];
    char  HeaderHex[2*CFE_SB_CMD_HDR_SIZE + 1];
    char  DataString[SCA_MAX_FILE_LINE_LENGTH + 1];
    char  HexByte[3];
    char  CCSDS_Packet_Hex[SCA_MAX_FILE_LINE_LENGTH + 1];

    CFE_PSP_MemSet (Line, '\0', SCA_MAX_FILE_LINE_LENGTH + 1);

    while (1)
    {
        OS_MutSemTake(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex); /* Protect data used by both the parent and children */

        if (SCA_OperData.ChildInfo[ChildInfoIndex].StopATS)
        {
            OS_MutSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex);
            break;
        }

        OS_MutSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex);

        ReadStatus = OS_read(SCA_OperData.ChildInfo[ChildInfoIndex].FileDescriptor, &Byte, 1);

        if ( ReadStatus == OS_FS_ERROR )
        {
            CFE_EVS_SendEvent( SCA_STARTATS_CMD_READFILE_ERR_EID, CFE_EVS_ERROR,
                               "ATS Stopped: Read File Error: 0x%x, %s, %s",
                               ReadStatus,
                               SCA_OperData.ChildInfo[ChildInfoIndex].ChildName,
                               SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile );

            SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus = SCA_FS_ERROR;
            SCA_OperData.HkPacket.CmdErrCtr++;
            ATS_ReturnStatus = SCA_CHILD_ERROR;
    
            break;
        }
        else if ( ReadStatus == 0 )
        {
            /* EOF Reached */
            // FIXME: There should be an EOF constant defined by OSAL.  There is a new DCR about this.  Fix this after the DCR is completed.
    
            SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus = SCA_EOF;
    
            break;
        }
        else if ( Byte == '\n' )
        {
            /* End of line reached */
    
            Line[Column] = '\0';
    
            CFE_PSP_MemSet (HeaderHex, '\0', 2*CFE_SB_CMD_HDR_SIZE + 1);
            CFE_PSP_MemSet (DataString, '\0', SCA_MAX_FILE_LINE_LENGTH + 1);
            CFE_PSP_MemSet (CCSDS_Packet_Hex, '\0', SCA_MAX_FILE_LINE_LENGTH + 1);
            CFE_PSP_MemSet (CCSDS_Packet, 0, SCA_MAX_CMD_PACKET_LENGTH);
    
            /* Parse line, copying data items into variables */
            SscanfStatus = sscanf(Line,"%u,%hhu,%u,%[^','],%s",
                                  &SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd,
                                  &SCA_OperData.ChildInfo[ChildInfoIndex].ForceFlag,
                                  &SCA_OperData.ChildInfo[ChildInfoIndex].TimeoutPeriod,
                                  HeaderHex, DataString);

            /* Not checking for all possible mistakes in the line - only checking if there is an error before any data could be read. */
            if (SscanfStatus == 0) // FIXME: should be EOF constant - see other FIXME.
            {
                CFE_EVS_SendEvent( SCA_ATS_READ_CMD_ERR_EID, CFE_EVS_ERROR,
                                   "ATS %s command %i error reading from file",
                                   SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile,
                                   SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber);
    
                SCA_OperData.HkPacket.AtsCmdErrCtr[ChildInfoIndex]++;
    
                OS_MutSemTake(SCA_OperData.HkMutex); /* Protect data used by both the parent and children */
                SCA_OperData.HkPacket.LastAtsCmdErrSeqNum = SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber;
                strncpy (SCA_OperData.HkPacket.LastAtsCmdErrFile, SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile, OS_MAX_PATH_LEN);
                OS_MutSemGive(SCA_OperData.HkMutex);
                break;
            }

            HeaderHex[2*CFE_SB_CMD_HDR_SIZE + 1] = '\0';

            /* Check to see whether the line is a command or not. */
            if (HeaderHex[0] != '\0')
            {
                SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus = SCA_COMMAND;

                /* Convert each header byte from Hex, copy into CCSDS_Packet */
                for (i = 0; HeaderHex[i] != '\0'; i++)
                {
                    HexByte[0] = HeaderHex[2*i];
                    HexByte[1] = HeaderHex[2*i + 1];
                    HexByte[2] = '\0';
    
                    CCSDS_Packet[i] = strtoul (HexByte, NULL, BASE16);
                }

                if (DataString[0] != '\0')
                {
                    ATS_ReturnStatus = SCA_ProcessAllParameters (ChildInfoIndex, CCSDS_Packet, DataString);
                }
            }
            else
            {
                SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus = SCA_NON_COMMAND;
            }

            break;
        }
        else if (Byte == ';')
        {
            InComment = TRUE;
            Column++;
        }
        else if ((InComment == FALSE) && isspace(Byte) == 0)
        {
            /* Put byte into array. Ignore comments and whitespace. */
            Line[Column] = (char) Byte;
            Column++;
        } /* end if */
    } /* end while loop */

    return ATS_ReturnStatus;
} /* End SCA_GetNextCommand */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Process all command parameters and copy to the CCSDS packet    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint16 SCA_ProcessAllParameters( uint8 ChildInfoIndex, uint8 *CCSDS_Packet, char *DataString )
{
    boolean InString              = FALSE;
    uint16  CCSDS_LastByteWritten = CFE_SB_CMD_HDR_SIZE - 1;
    uint16  ParamFirstIndex       = 0;
    uint16  ParamLastIndex        = 0;
    int16   i                     = 0;

    /*  Parse the parameters from DataString and then copy them into the CCSDS packet. This is necessary because              */
    /*  a.) Little endian machines need bytes reversed for each data item (except strings), and                               */
    /*  b.) The need to handle little endian machines caused the file format to need to be the way it is                      */
    /*      with each parameter separated, rather than just writing the parameters all together in their final CCSDS format.  */
    for (i = 0; i <= SCA_MAX_FILE_LINE_LENGTH + 1; i++)
    {
        if (DataString[i] == '\"')
        {
            if (InString == FALSE)
            {
                InString        = TRUE;
            }
            else
            {
                InString       = FALSE;
            }
        }
        else if (DataString[i] == '\0')
        {
            if (InString == TRUE)
            {
                CFE_EVS_SendEvent( SCA_STARTATS_CMD_READFILE_BAD_STRING_ERR_EID, CFE_EVS_ERROR,
                                   "ATS Stopped: missing end quote in %s, command %i",
                                   SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile,
                                   SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber );
                
                SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus = SCA_FS_ERROR;
                SCA_OperData.HkPacket.AtsCmdErrCtr[ChildInfoIndex]++;
                OS_MutSemTake(SCA_OperData.HkMutex); /* Protect data used by both the parent and children */
                SCA_OperData.HkPacket.LastAtsCmdErrSeqNum = SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber;
                strncpy (SCA_OperData.HkPacket.LastAtsCmdErrFile, SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile, OS_MAX_PATH_LEN);
                OS_MutSemGive(SCA_OperData.HkMutex);
                return SCA_CHILD_ERROR;
            }
            else
            {
                if (ParamFirstIndex != 0)
                {
                    ParamFirstIndex = ParamLastIndex + 2; /* +2 in order to move past colon delimiter */
                }
                ParamLastIndex  = i - 1;
                SCA_ProcessParameter (ChildInfoIndex, CCSDS_Packet, DataString, ParamFirstIndex, ParamLastIndex, &CCSDS_LastByteWritten);
                break;
            }
        }
        else if (DataString[i] == ':')
        {
            /* ParamFirstIndex is 0 only before and immediately after reading the first parameter. */
            if (ParamFirstIndex != 0)
            {
                ParamFirstIndex = ParamLastIndex + 2; /* +2 in order to move past colon delimiter */
            }
            ParamLastIndex  = i - 1;
            SCA_ProcessParameter (ChildInfoIndex, CCSDS_Packet, DataString, ParamFirstIndex, ParamLastIndex, &CCSDS_LastByteWritten);
            
            /* +2 in order to move past colon delimiter. This must be here to allow parameters after the first to be read correctly. */
            ParamFirstIndex = ParamLastIndex + 2;
        }
    } /* end for loop */

    return SCA_CHILD_NO_ERROR;
} /* End SCA_ProcessAllParameters */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Process a command parameter and copy it to the CCSDS packet    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_ProcessParameter( uint8 ChildInfoIndex, uint8 *CCSDS_Packet, char *DataString, uint16 FirstIndex, uint16 LastIndex, uint16 *CCSDS_LastByteWritten )
{
    uint8   TempBuffer[SCA_MAX_CMD_PACKET_LENGTH];
    char    HexByte[3];
    uint16  DataLength = 0;
    uint16  i          = 0;

    #ifdef _STRUCT_LOW_BIT_FIRST_
        uint8   LE_Buffer[SCA_MAX_CMD_PACKET_LENGTH];
    #endif

    if (DataString[FirstIndex] == '\"')
    {
        /* Convert each character of the string into binary, excluding the start and end quotation marks. */
        for (i = FirstIndex + 1; i <= LastIndex - 1; i++)
        {
            /*
            ** Copy the current character into the CCSDS packet, beginning after the header. 
            ** Subtracting FirstIndex because i starts at FirstIndex + 1.
            */
            CCSDS_Packet[(*CCSDS_LastByteWritten + i) - FirstIndex] = (uint8)(DataString[i]);
            DataLength++;
        }
    }
    else
    {
        /* Convert each pair of hex characters in the temporary buffer into decimal */
        for (i = 0; i < (LastIndex - FirstIndex + 1)/2; i++)
        {
            HexByte[0] = DataString[FirstIndex + 2*i];
            HexByte[1] = DataString[FirstIndex + 2*i + 1];
            HexByte[2] = '\0';
    
            TempBuffer[i] = strtoul (HexByte, NULL, 16);
            DataLength++;
        }

        #ifdef _STRUCT_LOW_BIT_FIRST_
            /* Reverse the order of the bytes in the temporary buffer */
            for (i = 0; i < DataLength; i++)
            {
                LE_Buffer[i] = TempBuffer[(DataLength - 1) - i];
            }
    
            /* Copy the buffer into the current index of CCSDS_Packet */
            for (i = 0; i < DataLength; i++)
            {
                CCSDS_Packet[(*CCSDS_LastByteWritten + 1) + i] = LE_Buffer[i];
            }
        #else
            /* Copy the buffer into the current index of CCSDS_Packet */
            for (i = 0; i < DataLength; i++)
            {
                CCSDS_Packet[(*CCSDS_LastByteWritten + 1) + i] = TempBuffer[i];
            }
        #endif
    }

    *CCSDS_LastByteWritten = *CCSDS_LastByteWritten + DataLength;
} /* end SCA_ProcessParameter */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Send a command in a CCSDS packet                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int16 SCA_SendCommand ( uint8 ChildInfoIndex, uint8 *CCSDS_Packet )
{
    int32   OS_Status             = 0;
    int16 ATS_ReturnStatus        = SCA_CHILD_NO_ERROR;
    boolean FirstLoop             = TRUE;
    SCA_AbsTimeTag_t CurrentTime  = 0;

    while (1)
    {
        OS_MutSemTake(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex); /* Protect data used by both the parent and children */

        if (SCA_OperData.ChildInfo[ChildInfoIndex].StopATS)
        {
            OS_MutSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex);
            break;
        }

        OS_MutSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex);

        CurrentTime = SCA_GetCurrentTime();

        SCA_OperData.ChildInfo[ChildInfoIndex].CurrentTime_Last = CurrentTime;

        if (SCA_OperData.ChildInfo[ChildInfoIndex].ForceFlag == FALSE)
        {
            if (SCA_OperData.ChildInfo[ChildInfoIndex].CurrentTime_Last > SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd)
            {
                /* skip command */

                CFE_EVS_SendEvent( SCA_ATS_SB_SKIPPED_CMD_ERR_EID, CFE_EVS_ERROR,
                                   "ATS %s command %i skipped",
                                   SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile,
                                   SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber);
                SCA_OperData.HkPacket.AtsCmdErrCtr[ChildInfoIndex]++;

                OS_MutSemTake(SCA_OperData.HkMutex); /* Protect data used by both the parent and children */
                SCA_OperData.HkPacket.LastAtsCmdErrSeqNum = SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber;
                strncpy (SCA_OperData.HkPacket.LastAtsCmdErrFile, SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile, OS_MAX_PATH_LEN);
                OS_MutSemGive(SCA_OperData.HkMutex);
                break;
            }
            else if (SCA_OperData.ChildInfo[ChildInfoIndex].CurrentTime_Last == SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd)
            {
                /* send command */
        
                OS_Status = CFE_SB_SendMsg((CFE_SB_Msg_t *)CCSDS_Packet);

                if (OS_Status == CFE_SUCCESS)
                {
                    SCA_OperData.HkPacket.AtsCmdCtr[ChildInfoIndex]++;
                }
                else
                {
                    CFE_EVS_SendEvent( SCA_ATS_SB_SEND_FAIL_ERR_EID, CFE_EVS_ERROR,
                                       "%s command %i failed to send on software bus: 0x\%08X",
                                       SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile,
                                       SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber,
                                       OS_Status);

                    ATS_ReturnStatus = SCA_CHILD_ERROR;

                    SCA_OperData.HkPacket.AtsCmdErrCtr[ChildInfoIndex]++;

                    OS_MutSemTake(SCA_OperData.HkMutex); /* Protect data used by both the parent and children */
                    SCA_OperData.HkPacket.LastAtsCmdErrSeqNum = SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber;
                    strncpy (SCA_OperData.HkPacket.LastAtsCmdErrFile, SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile, OS_MAX_PATH_LEN);
                    OS_MutSemGive(SCA_OperData.HkMutex);
                }

                SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus = SCA_NON_COMMAND; /* Prevents the command from being repeated. */
                break;
            } /* end if */
            else
            {
                SCA_ChildSleepUntilCommandTime(ChildInfoIndex);
            }
        }
        else
        {
            if (SCA_OperData.ChildInfo[ChildInfoIndex].CurrentTime_Last > (SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd + SCA_OperData.ChildInfo[ChildInfoIndex].TimeoutPeriod))
            {
                /* command timed out - skip */

                CFE_EVS_SendEvent( SCA_ATS_SB_SKIPPED_CMD_ERR_EID, CFE_EVS_ERROR,
                                   "ATS %s command %i skipped",
                                   SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile,
                                   SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber);

                SCA_OperData.HkPacket.AtsCmdErrCtr[ChildInfoIndex]++;

                OS_MutSemTake(SCA_OperData.HkMutex); /* Protect data used by both the parent and children */
                SCA_OperData.HkPacket.LastAtsCmdErrSeqNum = SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber;
                strncpy (SCA_OperData.HkPacket.LastAtsCmdErrFile, SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile, OS_MAX_PATH_LEN);
                OS_MutSemGive(SCA_OperData.HkMutex);
                break;
            }
            else if (SCA_OperData.ChildInfo[ChildInfoIndex].CurrentTime_Last >= SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd)
            {
                /* send command */

                OS_Status = CFE_SB_SendMsg((CFE_SB_Msg_t *)CCSDS_Packet);

                if (OS_Status == CFE_SUCCESS)
                {
                    SCA_OperData.HkPacket.AtsCmdCtr[ChildInfoIndex]++;
                }
                else
                {
                    CFE_EVS_SendEvent( SCA_ATS_SB_SEND_FAIL_ERR_EID, CFE_EVS_ERROR,
                                       "%s command %i failed to send on software bus: 0x\%08X",
                                       SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile,
                                       SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber,
                                       OS_Status);

                    ATS_ReturnStatus = SCA_CHILD_ERROR;

                    SCA_OperData.HkPacket.AtsCmdErrCtr[ChildInfoIndex]++;

                    OS_MutSemTake(SCA_OperData.HkMutex); /* Protect data used by both the parent and children */
                    SCA_OperData.HkPacket.LastAtsCmdErrSeqNum = SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber;
                    strncpy (SCA_OperData.HkPacket.LastAtsCmdErrFile, SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile, OS_MAX_PATH_LEN);
                    OS_MutSemGive(SCA_OperData.HkMutex);
                }

                SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus = SCA_NON_COMMAND; /* Prevents the command from being repeated. */
                break;
            }
            else
            {
                SCA_ChildSleepUntilCommandTime(ChildInfoIndex);
            }
        } /* end if */

        FirstLoop = FALSE;

    } /* end while loop */

    return ATS_ReturnStatus;
} /* End SCA_SendCommand */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Attempt to put the child to sleep until the command time       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_ChildSleepUntilCommandTime( uint8 ChildInfoIndex )
{
    /* 
     * Not yet time for the command - put process to sleep.  This code uses a binary semaphore in place of a call to OS_TaskDelay 
     * because this allows the delay to be interrupted by a call to SCA_StopATS.
     */

    int32              OS_Status                = 0;
    uint32             MillisecondsUntilCommand = 0;
    CFE_TIME_SysTime_t MaxCmdPendDelay;
    CFE_TIME_SysTime_t ExpectedTimeAfterDelay;
    CFE_TIME_SysTime_t TimeNextCmdFull;
    CFE_TIME_SysTime_t TimeUntilCmd;

    MaxCmdPendDelay.Seconds = floor(SCA_MAX_CMD_PEND_DELAY / 1000);
    MaxCmdPendDelay.Subseconds = CFE_TIME_Micro2SubSecs((SCA_MAX_CMD_PEND_DELAY % 1000) * 1000);

    TimeNextCmdFull.Seconds    = SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd;
    TimeNextCmdFull.Subseconds = 0;

    TimeUntilCmd             = CFE_TIME_Subtract (TimeNextCmdFull, SCA_GetCurrentTimeFull());
    MillisecondsUntilCommand = 1000*TimeUntilCmd.Seconds + CFE_TIME_Sub2MicroSecs(TimeUntilCmd.Subseconds)/1000;

    if (MillisecondsUntilCommand < 1)
    {
        MillisecondsUntilCommand = 1; /* Prevents command from being skipped. */
    }

    /* 
     * Initializing OS_Status to OS_ERR_INVALID_ID before attempting to take the mutex, so that 
     * if OS_BinSemFlush is called the subsequent code will not try to give back the mutex.
     */
    OS_Status = OS_ERR_INVALID_ID;

    if (MillisecondsUntilCommand > SCA_MAX_CMD_PEND_DELAY)
    {
        ExpectedTimeAfterDelay = CFE_TIME_Add (SCA_GetCurrentTimeFull(), MaxCmdPendDelay);

        OS_Status = OS_BinSemTimedWait((SCA_OperData.ChildInfo[ChildInfoIndex].PendingCmdSemaphore), SCA_MAX_CMD_PEND_DELAY);
    }
    else
    {
        ExpectedTimeAfterDelay.Seconds    = SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd;
        ExpectedTimeAfterDelay.Subseconds = 0;

        OS_Status = OS_BinSemTimedWait((SCA_OperData.ChildInfo[ChildInfoIndex].PendingCmdSemaphore) , MillisecondsUntilCommand);
    }

    if ((SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS == FALSE) && (SCA_GetCurrentTime() > ExpectedTimeAfterDelay.Seconds))
    {
        CFE_EVS_SendEvent( SCA_SENDCMD_TIME_DRIFT_INFO_EID, CFE_EVS_INFORMATION,
                           "Time drift detected");
    }

    if (OS_Status == OS_SUCCESS)
    {
        /* This case should never actually happen, since the main SCA task never gives back these semaphores */
        OS_BinSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].PendingCmdSemaphore);
    }
} /* End SCA_ChildSleepUntilCommandTime */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Top-level function for running an ATS in a child process       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_Child1Main( void )
{
    int16 ATS_ReturnStatus = SCA_CHILD_NO_ERROR;

    CFE_ES_RegisterChildTask();

    ATS_ReturnStatus = SCA_ProcessFile(SCA_CHILD_1);

    if (SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS == TRUE)
    {
        CFE_EVS_SendEvent( SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s stopped",
                           SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile);
    }
    else if (ATS_ReturnStatus == SCA_CHILD_NO_ERROR)
    {
        CFE_EVS_SendEvent( SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s Execution Completed",
                           SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile);
    }
    /* Other cases of ATS_ReturnStatus aren't handled here because their error event messages are sent in the earlier call to SCA_ProcessFile */

    SCA_ClearChildInformation(SCA_CHILD_1);
    CFE_ES_ExitChildTask();
    
} /* End SCA_Child1Main */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Top-level function for running an ATS in a child process       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_Child2Main( void )
{
    int16 ATS_ReturnStatus = SCA_CHILD_NO_ERROR;

    CFE_ES_RegisterChildTask();

    ATS_ReturnStatus = SCA_ProcessFile(SCA_CHILD_2);

    if (SCA_OperData.ChildInfo[SCA_CHILD_2].StopATS == TRUE)
    {
        CFE_EVS_SendEvent( SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s stopped",
                           SCA_OperData.ChildInfo[SCA_CHILD_2].ChildAtsFile);
    }
    else if (ATS_ReturnStatus == SCA_CHILD_NO_ERROR)
    {
        CFE_EVS_SendEvent( SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s Execution Completed",
                           SCA_OperData.ChildInfo[SCA_CHILD_2].ChildAtsFile);
    }
    /* Other cases of ATS_ReturnStatus aren't handled here because their error event messages are sent in the earlier call to SCA_ProcessFile */

    SCA_ClearChildInformation(SCA_CHILD_2);
    CFE_ES_ExitChildTask();

} /* End SCA_Child2Main */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Top-level function for running an ATS in a child process       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_Child3Main( void )
{
    int16 ATS_ReturnStatus = SCA_CHILD_NO_ERROR;

    CFE_ES_RegisterChildTask();

    ATS_ReturnStatus = SCA_ProcessFile(SCA_CHILD_3);

    if (SCA_OperData.ChildInfo[SCA_CHILD_3].StopATS == TRUE)
    {
        CFE_EVS_SendEvent( SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s stopped",
                           SCA_OperData.ChildInfo[SCA_CHILD_3].ChildAtsFile);
    }
    else if (ATS_ReturnStatus == SCA_CHILD_NO_ERROR)
    {
        /* FIXME: Skipped commands currently generate an error but do not set SCA_CHILD_ERROR, so this code branch is exectued in spite of the error.  Should it?   */
        CFE_EVS_SendEvent( SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s Execution Completed",
                           SCA_OperData.ChildInfo[SCA_CHILD_3].ChildAtsFile);
    }
    /* Other cases of ATS_ReturnStatus aren't handled here because their error event messages are sent in the earlier call to SCA_ProcessFile */

    SCA_ClearChildInformation(SCA_CHILD_3);
    CFE_ES_ExitChildTask();
    
} /* End SCA_Child3Main */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Top-level function for running an ATS in a child process       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_Child4Main( void )
{
    int16 ATS_ReturnStatus = SCA_CHILD_NO_ERROR;

    CFE_ES_RegisterChildTask();

    ATS_ReturnStatus = SCA_ProcessFile(SCA_CHILD_4);

    if (SCA_OperData.ChildInfo[SCA_CHILD_4].StopATS == TRUE)
    {
        CFE_EVS_SendEvent( SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s stopped",
                           SCA_OperData.ChildInfo[SCA_CHILD_4].ChildAtsFile);
    }
    else if (ATS_ReturnStatus == SCA_CHILD_NO_ERROR)
    {
        CFE_EVS_SendEvent( SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s Execution Completed",
                           SCA_OperData.ChildInfo[SCA_CHILD_4].ChildAtsFile);
    }
    /* Other cases of ATS_ReturnStatus aren't handled here because their error event messages are sent in the earlier call to SCA_ProcessFile */

    SCA_ClearChildInformation(SCA_CHILD_4);
    CFE_ES_ExitChildTask();
    
} /* End SCA_Child4Main */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Top-level function for running an ATS in a child process       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_Child5Main( void )
{
    int16 ATS_ReturnStatus = SCA_CHILD_NO_ERROR;

    CFE_ES_RegisterChildTask();

    ATS_ReturnStatus = SCA_ProcessFile(SCA_CHILD_5);

    if (SCA_OperData.ChildInfo[SCA_CHILD_5].StopATS == TRUE)
    {
        CFE_EVS_SendEvent( SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s stopped",
                           SCA_OperData.ChildInfo[SCA_CHILD_5].ChildAtsFile);
    }
    else if (ATS_ReturnStatus == SCA_CHILD_NO_ERROR)
    {
        CFE_EVS_SendEvent( SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION,
                           "ATS %s Execution Completed",
                           SCA_OperData.ChildInfo[SCA_CHILD_5].ChildAtsFile);
    }
    /* Other cases of ATS_ReturnStatus aren't handled here because their error event messages are sent in the earlier call to SCA_ProcessFile */

    SCA_ClearChildInformation(SCA_CHILD_5);
    CFE_ES_ExitChildTask();
    
} /* End SCA_Child5Main */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Find the lowest-indexed unused ATS child process               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint8 SCA_FindFreeChild( void )
{
   uint8 i;
   
   for(i = SCA_CHILD_1; i < SCA_MAX_NUM_ATS; i++)
   {
      OS_MutSemTake(SCA_OperData.ChildInfo[i].DataMutex); /* Protect data used by both the parent and children */
      if(SCA_OperData.ChildInfo[i].RunStatus == SCA_CHILD_NOT_USED)
      {
         OS_MutSemGive(SCA_OperData.ChildInfo[i].DataMutex);
         return(i);
      }
       OS_MutSemGive(SCA_OperData.ChildInfo[i].DataMutex);
   }

   return (SCA_NO_FREE_CHILD);

} /* End SCA_FindFreeChild */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/*  Clear info for specified ATS child process                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_ClearChildInformation(uint8 ChildInfoIndex)
{ 
    SCA_OperData.ChildInfo[ChildInfoIndex].ChildTaskID = 0;
    SCA_OperData.ChildInfo[ChildInfoIndex].ChildName[0] = '\0';
    SCA_OperData.ChildInfo[ChildInfoIndex].FileDescriptor = 0;
    SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus = 0;
    SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber = 0;
    SCA_OperData.ChildInfo[ChildInfoIndex].ForceFlag = 0;
    SCA_OperData.ChildInfo[ChildInfoIndex].TimeoutPeriod = 0;
    SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd = 0;
    SCA_OperData.ChildInfo[ChildInfoIndex].CurrentTime_Last = 0;
    SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile[0] = '\0';

    OS_MutSemTake(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex); /* Protect data used by both the parent and children */
    SCA_OperData.ChildInfo[ChildInfoIndex].StopATS = FALSE;
    SCA_OperData.ChildInfo[ChildInfoIndex].RunStatus = SCA_CHILD_NOT_USED;
    OS_MutSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex);

    SCA_CopyChildInformation(ChildInfoIndex); /* Also clear ChildInfoOutput */
                
} /* End SA_ClearChildInformation */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                   */
/*  Copy info of specified ATS child process into output structure   */
/*                                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void SCA_CopyChildInformation(uint8 ChildInfoIndex)
{
    /* Copy everything except StopATS, FileDescriptor, and DataMutex. */

    OS_MutSemTake(SCA_OperData.ChildInfo[ChildInfoIndex].OutputDataMutex); /* Protect data used by both the parent and children */
    strncpy (SCA_OperData.ChildInfoOutput[ChildInfoIndex].ChildAtsFile, SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile, OS_MAX_PATH_LEN);
    strncpy (SCA_OperData.ChildInfoOutput[ChildInfoIndex].ChildName, SCA_OperData.ChildInfo[ChildInfoIndex].ChildName, SCA_CHILD_NAME_SIZE);
    SCA_OperData.ChildInfoOutput[ChildInfoIndex].ChildTaskID = SCA_OperData.ChildInfo[ChildInfoIndex].ChildTaskID;
    SCA_OperData.ChildInfoOutput[ChildInfoIndex].CommandStatus = SCA_OperData.ChildInfo[ChildInfoIndex].CommandStatus;
    SCA_OperData.ChildInfoOutput[ChildInfoIndex].CommandSequenceNumber = SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber;
    SCA_OperData.ChildInfoOutput[ChildInfoIndex].ForceFlag = SCA_OperData.ChildInfo[ChildInfoIndex].ForceFlag;
    SCA_OperData.ChildInfoOutput[ChildInfoIndex].TimeoutPeriod = SCA_OperData.ChildInfo[ChildInfoIndex].TimeoutPeriod;
    SCA_OperData.ChildInfoOutput[ChildInfoIndex].TimeNextCmd = SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd;
    SCA_OperData.ChildInfoOutput[ChildInfoIndex].CurrentTime_Last = SCA_OperData.ChildInfo[ChildInfoIndex].CurrentTime_Last;

    OS_MutSemTake(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex); /* Protect data used by both the parent and children */
    SCA_OperData.ChildInfoOutput[ChildInfoIndex].RunStatus = SCA_OperData.ChildInfo[ChildInfoIndex].RunStatus;
    OS_MutSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].DataMutex);

    OS_MutSemGive(SCA_OperData.ChildInfo[ChildInfoIndex].OutputDataMutex);
} /* End SCA_CopyChildInformation */

/* * * * * * * * * * * * */
/*  End of File Comment  */
/* * * * * * * * * * * * */

