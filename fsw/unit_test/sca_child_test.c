 /*************************************************************************
 ** File:
 **   $Id: sca_child_test.c 1.1 2015/03/10 13:08:58EDT czogby Exp  $
 **
 ** Purpose: 
 **   This file contains unit test cases for the functions contained in the file sca_child.c.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 **   $Log: sca_child_test.c  $
 **   Revision 1.1 2015/03/10 13:08:58EDT czogby 
 **   Initial revision
 **   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/unit_test/project.pj
 *************************************************************************/

/*
 * Includes
 */

#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include "sca_child_test.h"
#include "sca_events.h"
#include "sca_test_utils.h"
#include "ut_osapi_stubs.h"
#include "ut_osfileapi_stubs.h"
#include "ut_cfe_sb_stubs.h"
#include "ut_cfe_time_stubs.h"
#include "ut_cfe_psp_memutils_stubs.h"

extern SCA_OperData_t SCA_OperData;

/*
 * Function Definitions
 */

int32  UT_OSFileAPI_ReadHookNominal(int32  filedes, void *buffer, uint32 nbytes);
int32  UT_OSFileAPI_ReadHookNominal(int32  filedes, void *buffer, uint32 nbytes)
{
    int   FileLength = 68;
    char  FileContents[FileLength];
    int32 ReturnValue;

    strncpy (FileContents, "; comment\n 0000100,1,2,1803C00000010025,00AABBCC ; SB NOOP Command\n\0", FileLength);

    *(char *)buffer = FileContents[GLOBAL_UT_OSFileAPI_ReadByteIndex];

    if (FileContents[GLOBAL_UT_OSFileAPI_ReadByteIndex] == 0)
    {
        // EOF
        ReturnValue = 0;
    }
    else
    {
        ReturnValue = 1;
    }
    
    GLOBAL_UT_OSFileAPI_ReadByteIndex++;
    
    return ReturnValue;
}

int32 UT_OSAPI_BinSemTimedWaitHook (uint32 sem_id, uint32 msecs);
int32 UT_OSAPI_BinSemTimedWaitHook (uint32 sem_id, uint32 msecs)
{
    CFE_TIME_SysTime_t TimeIncrement;

    TimeIncrement.Seconds    = floor(msecs / 1000);
    TimeIncrement.Subseconds = CFE_TIME_Micro2SubSecs((msecs % 1000) * 1000);

    GLOBAL_UT_SCA_Child_LocalTime = CFE_TIME_Add (GLOBAL_UT_SCA_Child_LocalTime, TimeIncrement);

    return OS_SUCCESS;
}

/**  
 **  Hook functions returning a CFE_TIME_SysTime_t value cannot be replaced with 
 **  SetReturnCode, because SetReturnCode only works with integers.
 **/
CFE_TIME_SysTime_t  UT_CFE_TIME_GetTimeHook1( void );
CFE_TIME_SysTime_t  UT_CFE_TIME_GetTimeHook1( void )
{
    CFE_TIME_SysTime_t ReturnValue;

    if ((GLOBAL_UT_CFE_TIME_GetTime_ChangeAfterFirstRun == TRUE) && (GLOBAL_UT_CFE_TIME_GetTime_FirstRun == FALSE))
    {
        ReturnValue.Seconds    = 200;
        ReturnValue.Subseconds = 0;
    }
    else
    {
        ReturnValue.Seconds    = 100;
        ReturnValue.Subseconds = 0;
    }

    GLOBAL_UT_CFE_TIME_GetTime_FirstRun = FALSE;
    
    return ReturnValue;
}

CFE_TIME_SysTime_t UT_CFE_TIME_GetTimeHook2 ( void );
CFE_TIME_SysTime_t UT_CFE_TIME_GetTimeHook2 ( void )
{

    return GLOBAL_UT_SCA_Child_LocalTime;
}

CFE_TIME_SysTime_t  UT_CFE_TIME_GetTimeHook3( void );
CFE_TIME_SysTime_t  UT_CFE_TIME_GetTimeHook3( void )
{
    CFE_TIME_SysTime_t ReturnValue;

    if (GLOBAL_UT_CFE_TIME_GetTime_FirstRun == FALSE)
    {
        ReturnValue.Seconds    = 25;
        ReturnValue.Subseconds = 0;
    }
    else
    {
        ReturnValue.Seconds    = 0;
        ReturnValue.Subseconds = 0;
    }

    GLOBAL_UT_CFE_TIME_GetTime_FirstRun = FALSE;
    
    return ReturnValue;
}

void SCA_ProcessFile_Test_StopAts(void)
{
    /**
     **  Test case: Stop ATS
     **/

    int16 ReturnStatus = SCA_CHILD_NO_ERROR;
    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    OS_BinSemTake(SCA_OperData.ChildInfo[SCA_CHILD_1].PendingCmdSemaphore);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS = TRUE;

    /* Execute the function being tested */
    ReturnStatus = SCA_ProcessFile( SCA_CHILD_1 );
    
    /* Verify results */
    UtAssert_True(ReturnStatus == SCA_CHILD_NO_ERROR, "ReturnStatus == SCA_CHILD_NO_ERROR");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "AtsCmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "AtsCmdErrCtr == 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_ProcessFile_Test_StopAts */

void SCA_ProcessFile_Test_RunFullAts(void)
{
    /**
     **  Test case: Run through entire ATS.
     **/

    int16 ReturnStatus = SCA_CHILD_NO_ERROR;
    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    OS_BinSemTake(SCA_OperData.ChildInfo[SCA_CHILD_1].PendingCmdSemaphore);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook2);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETUTC_INDEX, &UT_CFE_TIME_GetTimeHook2);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTAI_INDEX, &UT_CFE_TIME_GetTimeHook2);
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_OSFileAPI_ReadHookNominal);

    /* Execute the function being tested */
    ReturnStatus = SCA_ProcessFile( SCA_CHILD_1 );
    
    /* Verify results */
    UtAssert_True(ReturnStatus == SCA_CHILD_NO_ERROR, "ReturnStatus == SCA_CHILD_NO_ERROR");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 1, "AtsCmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "AtsCmdErrCtr == 0");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");
} /* end SCA_ProcessFile_Test_RunFullAts */

void SCA_ProcessFile_Test_FileOpenError(void)
{
    /**
     **  Test case: SCA_STARTATS_CMD_FILEOPEN_ERR_EID
     **/

    int16 ReturnStatus = SCA_CHILD_NO_ERROR;
    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_OPEN_INDEX, SCA_CHILD_ERROR, 1);

    /* Execute the function being tested */
    ReturnStatus = SCA_ProcessFile( SCA_CHILD_1 );
    
    /* Verify results */
    UtAssert_True(ReturnStatus == SCA_CHILD_ERROR, "ReturnStatus == SCA_CHILD_ERROR");
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "AtsCmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "AtsCmdErrCtr == 0");

     UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_FILEOPEN_ERR_EID, CFE_EVS_ERROR, "Start ATS Rejected: File Open Failure: 0xFFFFFFFF, SCA_CHILD1, filename.txt"),
        "Start ATS Rejected: File Open Failure: 0xFFFFFFFF, SCA_CHILD1, filename.txt");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessFile_Test_FileOpenError */

void SCA_ProcessFile_Test_CloseFileError(void)
{
    /**
     **  Test case: SCA_STARTATS_CLOSE_FILE_CRITICAL_EID
     **/

    int16 ReturnStatus = SCA_CHILD_NO_ERROR;
    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus = SCA_CHILD_EXECUTING;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_CLOSE_INDEX, SCA_CHILD_ERROR, 1);

    /* Execute the function being tested */
    ReturnStatus = SCA_ProcessFile( SCA_CHILD_1 );
    
    /* Verify results - not an SCA ATS error */
    UtAssert_True(ReturnStatus == SCA_CHILD_NO_ERROR, "ReturnStatus == SCA_CHILD_NO_ERROR");
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 1, "CmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 0, "CmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CLOSE_FILE_CRITICAL_EID, CFE_EVS_CRITICAL, "Start ATS: Error closing file filename.txt, 0, SCA_CHILD1, -1"),
        "Start ATS: Error closing file filename.txt, 0, SCA_CHILD1, -1");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessFile_Test_CloseFileError */

void SCA_GetNextCommand_Test_StopAts(void)
{
    /**
     **  Test case: Stop ATS
     **/

    int16         FilenameLength = 12;
    char          Filename[FilenameLength];
    uint8         CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    uint8         Expected_CCSDS_Packet[8];
    int16         ReturnStatus;

    strncpy (Filename, "filename.txt", FilenameLength);

    CFE_PSP_MemSet (CCSDS_Packet, 0, SCA_MAX_CMD_PACKET_LENGTH);
    CFE_PSP_MemSet (Expected_CCSDS_Packet, 0, SCA_MAX_CMD_PACKET_LENGTH);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS    = TRUE;

    SCA_OperData.ChildInfo[SCA_CHILD_1].FileDescriptor = OS_open (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, OS_READ_ONLY, 0);

    /* Execute the function being tested */
    ReturnStatus = SCA_GetNextCommand( SCA_CHILD_1, CCSDS_Packet );
    
    /* Verify results */
    UtAssert_True(ReturnStatus == SCA_CHILD_NO_ERROR, "ReturnStatus == SCA_CHILD_NO_ERROR");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "CommandStatus == SCA_NON_COMMAND");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "AtsCmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "AtsCmdErrCtr == 0");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_GetNextCommand_Test_StopAts */

void SCA_GetNextCommand_Test_AllNominalCases(void)
{
    /**
     **  Test case: Read a line that encompasses all cases in the while-loop of SCA_GetNextCommand. 
     **  In order to reach all cases, there must be at least one non-command line before the first command in the file.
     **/

    int16         FilenameLength = 12;
    char          Filename[FilenameLength];
    uint8         CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    uint8         CCSDS_Packet_First8Bytes[8];
    uint8         Expected_CCSDS_Packet[8];
    int16         ReturnStatus;
    int i;

    strncpy (Filename, "filename.txt", FilenameLength);

    CFE_PSP_MemSet (CCSDS_Packet, 0, SCA_MAX_CMD_PACKET_LENGTH);
    CFE_PSP_MemSet (Expected_CCSDS_Packet, 0, SCA_MAX_CMD_PACKET_LENGTH);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus  = SCA_CHILD_EXECUTING;

    SCA_OperData.ChildInfo[SCA_CHILD_1].FileDescriptor = OS_open (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, OS_READ_ONLY, 0);

    /* Hex values: 18 03 C0 00 00 01 00 25 */
    Expected_CCSDS_Packet[0]  = 0x18;
    Expected_CCSDS_Packet[1]  = 0x03;
    Expected_CCSDS_Packet[2]  = 0xC0;
    Expected_CCSDS_Packet[3]  = 0x00;
    Expected_CCSDS_Packet[4]  = 0x00;
    Expected_CCSDS_Packet[5]  = 0x01;
    Expected_CCSDS_Packet[6]  = 0x00;
    Expected_CCSDS_Packet[7]  = 0x25;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_OSFILEAPI_SetFunctionHook(UT_OSFILEAPI_READ_INDEX, &UT_OSFileAPI_ReadHookNominal);

    /* Execute the function being tested.  Calling the function 3 times to read two lines (and the EOF): a comment line followed by a command line. */
    ReturnStatus = SCA_GetNextCommand( SCA_CHILD_1, CCSDS_Packet );

    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "ChildInfo[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND");
    UtAssert_True(ReturnStatus == SCA_CHILD_NO_ERROR, "ReturnStatus == SCA_CHILD_NO_ERROR");

    /* Execute the function being tested.  Calling the function 3 times to read two lines (and the EOF): a comment line followed by a command line. */
    ReturnStatus = SCA_GetNextCommand( SCA_CHILD_1, CCSDS_Packet );

    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_COMMAND, "ChildInfo[SCA_CHILD_1].CommandStatus == SCA_COMMAND");

    for (i = 0; i < 8; i++)
    {
        CCSDS_Packet_First8Bytes[i] = CCSDS_Packet[i];
    }
    
    UtAssert_True(ReturnStatus == SCA_CHILD_NO_ERROR, "ReturnStatus == SCA_CHILD_NO_ERROR");
    UtAssert_MemCmp(CCSDS_Packet_First8Bytes, Expected_CCSDS_Packet, 8, "CCSDS_Packet == Expected_CCSDS_Packet");

    /* Execute the function being tested.  Calling the function 3 times to read two lines (and the EOF): a comment line followed by a command line. */
    ReturnStatus = SCA_GetNextCommand( SCA_CHILD_1, CCSDS_Packet );

    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_EOF, "ChildInfo[SCA_CHILD_1].CommandStatus == SCA_EOF");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_GetNextCommand_Test_AllNominalCases */

void SCA_GetNextCommand_Test_ReadFileError(void)
{
    /**
     **  Test case: SCA_STARTATS_CMD_READFILE_ERR_EID
     **/

    uint8          CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    uint8          Expected_CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16          ReturnStatus;
    int16          FilenameLength = 12;
    char           Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    CFE_PSP_MemSet (CCSDS_Packet, 0, SCA_MAX_CMD_PACKET_LENGTH);
    CFE_PSP_MemSet (Expected_CCSDS_Packet, 0, SCA_MAX_CMD_PACKET_LENGTH);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus  = SCA_CHILD_EXECUTING;

    SCA_OperData.ChildInfo[SCA_CHILD_1].FileDescriptor = OS_open (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, OS_READ_ONLY, 0);

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_OSFILEAPI_SetReturnCode(UT_OSFILEAPI_READ_INDEX, OS_FS_ERROR, 1);

    /* Execute the function being tested */
    ReturnStatus = SCA_GetNextCommand( SCA_CHILD_1, CCSDS_Packet );
    
    /* Verify results */
    UtAssert_True(ReturnStatus == SCA_CHILD_ERROR, "ReturnStatus == SCA_CHILD_ERROR");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_FS_ERROR, "CommandStatus == SCA_FS_ERROR");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "CommandStatus == SCA_NON_COMMAND");
    UtAssert_True(SCA_OperData.HkPacket.CmdCtr == 0, "CmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.CmdErrCtr == 1, "CmdErrCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "AtsCmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "AtsCmdErrCtr == 0");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_READFILE_ERR_EID, CFE_EVS_ERROR, "ATS Stopped: Read File Error: 0xffffffff, SCA_CHILD1, filename.txt"),
        "ATS Stopped: Read File Error: 0xffffffff, SCA_CHILD1, filename.txt");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

    /* SCA_ATS_READ_CMD_ERR_EID can't be tested because it would require a hook function for a non-cfe/osal function. */
} /* end SCA_GetNextCommand_Test_ReadFileError */

void SCA_ProcessAllParameters_Test_Nominal(void)
{
    /**
     **  Test case: Process a parameter string containing three parameters: a string and two numbers.
     **/

    int            ParameterBytes = 10;
    uint8          CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    uint8          Expected_CCSDS_Packet[8 + ParameterBytes];
    int16          ReturnStatus;
    int            i;
    uint8          CCSDS_PacketShort[8 + ParameterBytes];
    int16          FilenameLength = 12;
    char           Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    CFE_PSP_MemSet (CCSDS_Packet, 0, SCA_MAX_CMD_PACKET_LENGTH);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    /* Set non-parameter bytes of CCSDS packet to zero. */
    for (i = 0; i < 8; i++)
    {
        CCSDS_Packet[i]          = 0;
        Expected_CCSDS_Packet[i] = 0;
    }

    /* Execute the function being tested */
    ReturnStatus = SCA_ProcessAllParameters( SCA_CHILD_1, CCSDS_Packet, "\"test\":0AB36E:ABC123" );

    for (i = 0; i < 8 + ParameterBytes; i++)
    {
        CCSDS_PacketShort[i] = CCSDS_Packet[i];
    }

    Expected_CCSDS_Packet[8]  = 't';
    Expected_CCSDS_Packet[9]  = 'e';
    Expected_CCSDS_Packet[10] = 's';
    Expected_CCSDS_Packet[11] = 't';
    Expected_CCSDS_Packet[12]  = 0x6E;
    Expected_CCSDS_Packet[13]  = 0xB3;
    Expected_CCSDS_Packet[14]  = 0x0A;
    Expected_CCSDS_Packet[15]  = 0x23;
    Expected_CCSDS_Packet[16]  = 0xC1;
    Expected_CCSDS_Packet[17]  = 0xAB;
    
    /* Verify results */
    UtAssert_True(ReturnStatus == SCA_CHILD_NO_ERROR, "ReturnStatus == SCA_CHILD_NO_ERROR");
    UtAssert_MemCmp(CCSDS_PacketShort, Expected_CCSDS_Packet, 8 + ParameterBytes, "CCSDS_Packet == Expected_CCSDS_Packet");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_ProcessAllParameters_Test_Nominal */

void SCA_ProcessAllParameters_Test_BadStringError(void)
{
    /**
     **  Test case: SCA_STARTATS_CMD_READFILE_BAD_STRING_ERR_EID
     **/

    int16  ReturnStatus;
    uint8  CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16  FilenameLength = 12;
    char   Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    CFE_PSP_MemSet (CCSDS_Packet, 0, SCA_MAX_CMD_PACKET_LENGTH);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    /* Execute the function being tested */
    ReturnStatus = SCA_ProcessAllParameters( SCA_CHILD_1, CCSDS_Packet, "\"test:0AB36E" );
    
    /* Verify results */
    UtAssert_True(ReturnStatus == SCA_CHILD_ERROR, "ReturnStatus == SCA_CHILD_ERROR");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_FS_ERROR, "CommandStatus == SCA_FS_ERROR");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 1, "AtsCmdErrCtr == 1");
    UtAssert_MemCmp(SCA_OperData.HkPacket.LastAtsCmdErrFile, Filename, FilenameLength, "LastAtsCmdErrFile == Filename");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STARTATS_CMD_READFILE_BAD_STRING_ERR_EID, CFE_EVS_ERROR, "ATS Stopped: missing end quote in filename.txt, command 0"),
        "ATS Stopped: missing end quote in filename.txt, command 0");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ProcessAllParameters_Test_BadStringError */

void SCA_SendCommand_Test_StopAts(void)
{
    /**
     **  Test case: Stop ATS
     **/

    uint8 CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16 ReturnStatus = SCA_CHILD_NO_ERROR;

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS = TRUE;

    /* Execute the function being tested */
    ReturnStatus = SCA_SendCommand( SCA_CHILD_1, CCSDS_Packet );
    
    /* Verify results */
    UtAssert_True(ReturnStatus == SCA_CHILD_NO_ERROR, "Case 1: ReturnStatus == SCA_CHILD_NO_ERROR");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "Case 1: AtsCmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "Case 1: AtsCmdErrCtr == 0");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_SendCommand_Test_StopAts */

void SCA_SendCommand_Test_ForceFlagFalseSendLater(void)
{
    /**
     **  Test case: ForceFlag = False, send a command to be sent at a future time.
     **/

    uint8 CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16 ReturnStatus = SCA_CHILD_NO_ERROR;

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus     = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus = SCA_COMMAND;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd   = 200;
    GLOBAL_UT_CFE_TIME_GetTime_ChangeAfterFirstRun    = TRUE;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETUTC_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTAI_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook1);

    /* Execute the function being tested */
    ReturnStatus = SCA_SendCommand( SCA_CHILD_1, CCSDS_Packet );

    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 1, "Case 2: AtsCmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "Case 2: AtsCmdErrCtr == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "Case 2: CommandStatus == SCA_NON_COMMAND");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_SendCommand_Test_ForceFlagFalseSendLater */

void SCA_SendCommand_Test_ForceFlagFalseSendNow(void)
{
    /**
     **  Test case: ForceFlag = False, send a command to be sent at the present time.
     **/

    uint8 CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16 ReturnStatus = SCA_CHILD_NO_ERROR;

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus     = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus = SCA_COMMAND;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd   = 100;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETUTC_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTAI_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook1);

    /* Execute the function being tested */
    ReturnStatus = SCA_SendCommand( SCA_CHILD_1, CCSDS_Packet );

    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 1, "Case 3: AtsCmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "Case 3: AtsCmdErrCtr == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "Case 3: CommandStatus == SCA_NON_COMMAND"); /* status reverted after cmd sent*/
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_SendCommand_Test_ForceFlagFalseSendNow */

void SCA_SendCommand_Test_ForceFlagTrueSendLater(void)
{
    /**
     **  Test case: ForceFlag = True, send a command to be sent at a future time.
     **/

    uint8 CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16 ReturnStatus = SCA_CHILD_NO_ERROR;

    SCA_OperData.ChildInfo[SCA_CHILD_1].ForceFlag     = TRUE;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeoutPeriod = 10;
    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus     = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus = SCA_COMMAND;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd   = 191;
    GLOBAL_UT_CFE_TIME_GetTime_ChangeAfterFirstRun    = TRUE;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETUTC_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTAI_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook1);

    /* Execute the function being tested */
    ReturnStatus = SCA_SendCommand( SCA_CHILD_1, CCSDS_Packet );

    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 1, "Case 4: AtsCmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "Case 4: AtsCmdErrCtr == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "Case 4: CommandStatus == SCA_NON_COMMAND");
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_SendCommand_Test_ForceFlagTrueSendLater */

void SCA_SendCommand_Test_ForceFlagTrueSendNow(void)
{
    /**
     **  Test case: ForceFlag = True, send a command to be sent at the present time.
     **/

    uint8 CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16 ReturnStatus = SCA_CHILD_NO_ERROR;

    SCA_OperData.ChildInfo[SCA_CHILD_1].ForceFlag     = TRUE;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeoutPeriod = 10;
    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus     = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus = SCA_COMMAND;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd   = 91;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETUTC_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTAI_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook1);

    /* Execute the function being tested */
    ReturnStatus = SCA_SendCommand( SCA_CHILD_1, CCSDS_Packet );

    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 1, "Case 5: AtsCmdCtr == 1");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 0, "Case 5: AtsCmdErrCtr == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "Case 5: CommandStatus == SCA_NON_COMMAND"); /* status reverted after cmd sent*/
    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 0, "Ut_CFE_EVS_GetEventQueueDepth() == 0");

} /* end SCA_SendCommand_Test_ForceFlagTrueSendNow */

void SCA_SendCommand_Test_ForceFlagFalseSkippedCmd(void)
{
    /**
     **  Test case: Force flag false, skipped command
     **/

    uint8 CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16 ReturnStatus = SCA_CHILD_NO_ERROR;
    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus     = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus = SCA_COMMAND;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd   = 91;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETUTC_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTAI_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook1);

    /* Execute the function being tested */
    ReturnStatus = SCA_SendCommand( SCA_CHILD_1, CCSDS_Packet );

    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "Case 1: AtsCmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 1, "Case 1: AtsCmdErrCtr == 1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_COMMAND, "Case 1: CommandStatus == SCA_COMMAND"); /* status reverted after cmd sent*/
    UtAssert_MemCmp(SCA_OperData.HkPacket.LastAtsCmdErrFile, Filename, FilenameLength, "LastAtsCmdErrFile == Filename");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_SB_SKIPPED_CMD_ERR_EID, CFE_EVS_ERROR, "ATS filename.txt command 0 skipped"),
        "ATS filename.txt command 0 skipped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_SendCommand_Test_ForceFlagFalseSkippedCmd */

void SCA_SendCommand_Test_ForceFlagFalseFailedSend(void)
{
    /**
     **  Test case: Force flag false, failed to send
     **/

    uint8 CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16 ReturnStatus = SCA_CHILD_NO_ERROR;
    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus     = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus = SCA_COMMAND;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd   = 100;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETUTC_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTAI_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SENDMSG_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnStatus = SCA_SendCommand( SCA_CHILD_1, CCSDS_Packet );

    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "Case 2: AtsCmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 1, "Case 2: AtsCmdErrCtr == 1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "Case 2: CommandStatus == SCA_NON_COMMAND"); /* status reverted after cmd sent*/
    UtAssert_MemCmp(SCA_OperData.HkPacket.LastAtsCmdErrFile, Filename, FilenameLength, "LastAtsCmdErrFile == Filename");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_SB_SEND_FAIL_ERR_EID, CFE_EVS_ERROR, "filename.txt command 0 failed to send on software bus: 0xFFFFFFFF"),
        "filename.txt command 0 failed to send on software bus: 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_SendCommand_Test_ForceFlagFalseFailedSend */

void SCA_SendCommand_Test_ForceFlagTrueSkippedCmd(void)
{
    /**
     **  Test case: Force flag true, skipped command
     **/

    uint8 CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16 ReturnStatus = SCA_CHILD_NO_ERROR;
    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].ForceFlag     = TRUE;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeoutPeriod = 10;
    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus     = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus = SCA_COMMAND;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd   = 89;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETUTC_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTAI_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook1);

    /* Execute the function being tested */
    ReturnStatus = SCA_SendCommand( SCA_CHILD_1, CCSDS_Packet );

    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "Case 3: AtsCmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 1, "Case 3: AtsCmdErrCtr == 1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_COMMAND, "Case 3: CommandStatus == SCA_COMMAND"); /* status reverted after cmd sent*/
    UtAssert_MemCmp(SCA_OperData.HkPacket.LastAtsCmdErrFile, Filename, FilenameLength, "LastAtsCmdErrFile == Filename");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_SB_SKIPPED_CMD_ERR_EID, CFE_EVS_ERROR, "ATS filename.txt command 0 skipped"),
        "ATS filename.txt command 0 skipped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_SendCommand_Test_ForceFlagTrueSkippedCmd */

void SCA_SendCommand_Test_ForceFlagTrueFailedSend(void)
{
    /**
     **  Test case: Force flag true, failed to send
     **/

    uint8 CCSDS_Packet[SCA_MAX_CMD_PACKET_LENGTH];
    int16 ReturnStatus = SCA_CHILD_NO_ERROR;
    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);
    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName, SCA_CHILD_1_NAME, SCA_CHILD_NAME_SIZE);

    SCA_OperData.ChildInfo[SCA_CHILD_1].ForceFlag     = TRUE;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeoutPeriod = 1;
    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus     = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus = SCA_COMMAND;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd   = 100;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETUTC_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTAI_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook1);
    Ut_CFE_SB_SetReturnCode(UT_CFE_SB_SENDMSG_INDEX, -1, 1);

    /* Execute the function being tested */
    ReturnStatus = SCA_SendCommand( SCA_CHILD_1, CCSDS_Packet );

    UtAssert_True(SCA_OperData.HkPacket.AtsCmdCtr[SCA_CHILD_1] == 0, "Case 4: AtsCmdCtr == 0");
    UtAssert_True(SCA_OperData.HkPacket.AtsCmdErrCtr[SCA_CHILD_1] == 1, "Case 4: AtsCmdErrCtr == 1");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == SCA_NON_COMMAND, "Case 4: CommandStatus == SCA_NON_COMMAND"); /* status reverted after cmd sent*/
    UtAssert_MemCmp(SCA_OperData.HkPacket.LastAtsCmdErrFile, Filename, FilenameLength, "LastAtsCmdErrFile == Filename");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_SB_SEND_FAIL_ERR_EID, CFE_EVS_ERROR, "filename.txt command 0 failed to send on software bus: 0xFFFFFFFF"),
        "filename.txt command 0 failed to send on software bus: 0xFFFFFFFF");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_SendCommand_Test_ForceFlagTrueFailedSend */

/* SCA_ChildSleepUntilCommandTime_Test_Nominal not needed - nominal case already covered in other tests. */

void SCA_ChildSleepUntilCommandTime_Test_TimeDriftError(void)
{
    /**
     **  Test case: SCA_SENDCMD_TIME_DRIFT_INFO_EID
     **/

    SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS     = FALSE;
    SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd = 5;

    Ut_OSAPI_SetFunctionHook(UT_OSAPI_BINSEMTIMEDWAIT_INDEX, &UT_OSAPI_BinSemTimedWaitHook);
    Ut_CFE_TIME_SetFunctionHook(UT_CFE_TIME_GETTIME_INDEX, &UT_CFE_TIME_GetTimeHook3);

    /* Execute the function being tested */
    SCA_ChildSleepUntilCommandTime( SCA_CHILD_1 );

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_SENDCMD_TIME_DRIFT_INFO_EID, CFE_EVS_INFORMATION, "Time drift detected"),
        "Time drift detected");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_ChildSleepUntilCommandTime_Test_TimeDriftError */

void SCA_Child1Main_StopATS (void)
{
    /**
     **  Test case: SCA_Child1Main - StopATS
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS    = TRUE;

    /* Execute the function being tested */
    SCA_Child1Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].ChildTaskID == 0, "ChildInfo[SCA_CHILD_1].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_1].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].FileDescriptor == 0, "ChildInfo[SCA_CHILD_1].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == 0, "ChildInfo[SCA_CHILD_1].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_1].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].ForceFlag == 0, "ChildInfo[SCA_CHILD_1].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_1].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_1].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_1].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile[0] == '\0', "ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS == FALSE, "StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus == SCA_CHILD_NOT_USED, "RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].ChildAtsFile[0] == '\0', "ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_1].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].ChildTaskID == 0, "ChildInfo[SCA_CHILD_1].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].CommandStatus == 0, "ChildInfo[SCA_CHILD_1].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_1].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].ForceFlag == 0, "ChildInfo[SCA_CHILD_1].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_1].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_1].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_1].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].RunStatus == SCA_CHILD_NOT_USED, "RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt stopped"),
        "ATS filename.txt stopped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child1Main_StopATS */

void SCA_Child1Main_ExecutionCompleted (void)
{
    /**
     **  Test case: SCA_Child1Main - Execution Completed
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS    = FALSE;

    /* Execute the function being tested */
    SCA_Child1Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].ChildTaskID == 0, "ChildInfo[SCA_CHILD_1].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_1].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].FileDescriptor == 0, "ChildInfo[SCA_CHILD_1].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandStatus == 0, "ChildInfo[SCA_CHILD_1].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_1].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].ForceFlag == 0, "ChildInfo[SCA_CHILD_1].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_1].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_1].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_1].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].ChildAtsFile[0] == '\0', "ChildInfo[SCA_CHILD_1].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].StopATS == FALSE, "ChildInfo[SCA_CHILD_1].StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_1].RunStatus == SCA_CHILD_NOT_USED, "ChildInfo[SCA_CHILD_1].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].ChildAtsFile[0] == '\0', "ChildInfoOutput[SCA_CHILD_1].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].ChildName[0] == '\0', "ChildInfoOutput[SCA_CHILD_1].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].ChildTaskID == 0, "ChildInfoOutput[SCA_CHILD_1].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].CommandStatus == 0, "ChildInfoOutput[SCA_CHILD_1].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].CommandSequenceNumber == 0, "ChildInfoOutput[SCA_CHILD_1].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].ForceFlag == 0, "ChildInfoOutput[SCA_CHILD_1].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].TimeoutPeriod == 0, "ChildInfoOutput[SCA_CHILD_1].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].TimeNextCmd == 0, "ChildInfoOutput[SCA_CHILD_1].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].CurrentTime_Last == 0, "ChildInfoOutput[SCA_CHILD_1].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_1].RunStatus == SCA_CHILD_NOT_USED, "ChildInfoOutput[SCA_CHILD_1].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt Execution Completed"),
        "ATS filename.txt Execution Completed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child1Main_ExecutionCompleted */

void SCA_Child2Main_StopATS (void)
{
    /**
     **  Test case: SCA_Child2Main - StopATS
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_2].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_2].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_2].StopATS    = TRUE;

    /* Execute the function being tested */
    SCA_Child2Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].ChildTaskID == 0, "ChildInfo[SCA_CHILD_2].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_2].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].FileDescriptor == 0, "ChildInfo[SCA_CHILD_2].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].CommandStatus == 0, "ChildInfo[SCA_CHILD_2].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_2].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].ForceFlag == 0, "ChildInfo[SCA_CHILD_2].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_2].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_2].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_2].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].ChildAtsFile[0] == '\0', "ChildInfo[SCA_CHILD_2].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].StopATS == FALSE, "ChildInfo[SCA_CHILD_2].StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].RunStatus == SCA_CHILD_NOT_USED, "ChildInfo[SCA_CHILD_2].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].ChildAtsFile[0] == '\0', "ChildInfoOutput[SCA_CHILD_2].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].ChildName[0] == '\0', "ChildInfoOutput[SCA_CHILD_2].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].ChildTaskID == 0, "ChildInfoOutput[SCA_CHILD_2].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].CommandStatus == 0, "ChildInfoOutput[SCA_CHILD_2].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].CommandSequenceNumber == 0, "ChildInfoOutput[SCA_CHILD_2].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].ForceFlag == 0, "ChildInfoOutput[SCA_CHILD_2].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].TimeoutPeriod == 0, "ChildInfoOutput[SCA_CHILD_2].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].TimeNextCmd == 0, "ChildInfoOutput[SCA_CHILD_2].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].CurrentTime_Last == 0, "ChildInfoOutput[SCA_CHILD_2].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].RunStatus == SCA_CHILD_NOT_USED, "ChildInfoOutput[SCA_CHILD_2].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt stopped"),
        "ATS filename.txt stopped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child2Main_StopATS */

void SCA_Child2Main_ExecutionCompleted (void)
{
    /**
     **  Test case: SCA_Child2Main - Execution Completed
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_2].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_2].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_2].StopATS    = FALSE;

    /* Execute the function being tested */
    SCA_Child2Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].ChildTaskID == 0, "ChildInfo[SCA_CHILD_2].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_2].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].FileDescriptor == 0, "ChildInfo[SCA_CHILD_2].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].CommandStatus == 0, "ChildInfo[SCA_CHILD_2].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_2].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].ForceFlag == 0, "ChildInfo[SCA_CHILD_2].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_2].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_2].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_2].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].ChildAtsFile[0] == '\0', "ChildInfo[SCA_CHILD_2].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].StopATS == FALSE, "ChildInfo[SCA_CHILD_2].StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_2].RunStatus == SCA_CHILD_NOT_USED, "ChildInfo[SCA_CHILD_2].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].ChildAtsFile[0] == '\0', "ChildInfoOutput[SCA_CHILD_2].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].ChildName[0] == '\0', "ChildInfoOutput[SCA_CHILD_2].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].ChildTaskID == 0, "ChildInfoOutput[SCA_CHILD_2].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].CommandStatus == 0, "ChildInfoOutput[SCA_CHILD_2].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].CommandSequenceNumber == 0, "ChildInfoOutput[SCA_CHILD_2].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].ForceFlag == 0, "ChildInfoOutput[SCA_CHILD_2].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].TimeoutPeriod == 0, "ChildInfoOutput[SCA_CHILD_2].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].TimeNextCmd == 0, "ChildInfoOutput[SCA_CHILD_2].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].CurrentTime_Last == 0, "ChildInfoOutput[SCA_CHILD_2].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_2].RunStatus == SCA_CHILD_NOT_USED, "ChildInfoOutput[SCA_CHILD_2].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt Execution Completed"),
        "ATS filename.txt Execution Completed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child2Main_ExecutionCompleted */

void SCA_Child3Main_StopATS (void)
{
    /**
     **  Test case: SCA_Child3Main - StopATS
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_3].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_3].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_3].StopATS    = TRUE;

    /* Execute the function being tested */
    SCA_Child3Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].ChildTaskID == 0, "ChildInfo[SCA_CHILD_3].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_3].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].FileDescriptor == 0, "ChildInfo[SCA_CHILD_3].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].CommandStatus == 0, "ChildInfo[SCA_CHILD_3].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_3].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].ForceFlag == 0, "ChildInfo[SCA_CHILD_3].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_3].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_3].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_3].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].ChildAtsFile[0] == '\0', "ChildInfo[SCA_CHILD_3].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].StopATS == FALSE, "ChildInfo[SCA_CHILD_3].StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].RunStatus == SCA_CHILD_NOT_USED, "ChildInfo[SCA_CHILD_3].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].ChildAtsFile[0] == '\0', "ChildInfoOutput[SCA_CHILD_3].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].ChildName[0] == '\0', "ChildInfoOutput[SCA_CHILD_3].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].ChildTaskID == 0, "ChildInfoOutput[SCA_CHILD_3].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].CommandStatus == 0, "ChildInfoOutput[SCA_CHILD_3].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].CommandSequenceNumber == 0, "ChildInfoOutput[SCA_CHILD_3].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].ForceFlag == 0, "ChildInfoOutput[SCA_CHILD_3].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].TimeoutPeriod == 0, "ChildInfoOutput[SCA_CHILD_3].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].TimeNextCmd == 0, "ChildInfoOutput[SCA_CHILD_3].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].CurrentTime_Last == 0, "ChildInfoOutput[SCA_CHILD_3].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].RunStatus == SCA_CHILD_NOT_USED, "ChildInfoOutput[SCA_CHILD_3].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt stopped"),
        "ATS filename.txt stopped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child3Main_StopATS */

void SCA_Child3Main_ExecutionCompleted (void)
{
    /**
     **  Test case: SCA_Child3Main - Execution Completed
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_3].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_3].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_3].StopATS    = FALSE;

    /* Execute the function being tested */
    SCA_Child3Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].ChildTaskID == 0, "ChildInfo[SCA_CHILD_3].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_3].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].FileDescriptor == 0, "ChildInfo[SCA_CHILD_3].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].CommandStatus == 0, "ChildInfo[SCA_CHILD_3].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_3].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].ForceFlag == 0, "ChildInfo[SCA_CHILD_3].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_3].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_3].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_3].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].ChildAtsFile[0] == '\0', "ChildInfo[SCA_CHILD_3].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].StopATS == FALSE, "ChildInfo[SCA_CHILD_3].StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_3].RunStatus == SCA_CHILD_NOT_USED, "ChildInfo[SCA_CHILD_3].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].ChildAtsFile[0] == '\0', "ChildInfoOutput[SCA_CHILD_3].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].ChildName[0] == '\0', "ChildInfoOutput[SCA_CHILD_3].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].ChildTaskID == 0, "ChildInfoOutput[SCA_CHILD_3].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].CommandStatus == 0, "ChildInfoOutput[SCA_CHILD_3].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].CommandSequenceNumber == 0, "ChildInfoOutput[SCA_CHILD_3].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].ForceFlag == 0, "ChildInfoOutput[SCA_CHILD_3].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].TimeoutPeriod == 0, "ChildInfoOutput[SCA_CHILD_3].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].TimeNextCmd == 0, "ChildInfoOutput[SCA_CHILD_3].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].CurrentTime_Last == 0, "ChildInfoOutput[SCA_CHILD_3].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_3].RunStatus == SCA_CHILD_NOT_USED, "ChildInfoOutput[SCA_CHILD_3].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt Execution Completed"),
        "ATS filename.txt Execution Completed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child3Main_ExecutionCompleted */

void SCA_Child4Main_StopATS (void)
{
    /**
     **  Test case: SCA_Child4Main - StopATS
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_4].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_4].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_4].StopATS    = TRUE;

    /* Execute the function being tested */
    SCA_Child4Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].ChildTaskID == 0, "ChildInfo[SCA_CHILD_4].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_4].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].FileDescriptor == 0, "ChildInfo[SCA_CHILD_4].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].CommandStatus == 0, "ChildInfo[SCA_CHILD_4].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_4].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].ForceFlag == 0, "ChildInfo[SCA_CHILD_4].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_4].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_4].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_4].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].ChildAtsFile[0] == '\0', "ChildInfo[SCA_CHILD_4].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].StopATS == FALSE, "ChildInfo[SCA_CHILD_4].StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].RunStatus == SCA_CHILD_NOT_USED, "ChildInfo[SCA_CHILD_4].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].ChildAtsFile[0] == '\0', "ChildInfoOutput[SCA_CHILD_4].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].ChildName[0] == '\0', "ChildInfoOutput[SCA_CHILD_4].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].ChildTaskID == 0, "ChildInfoOutput[SCA_CHILD_4].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].CommandStatus == 0, "ChildInfoOutput[SCA_CHILD_4].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].CommandSequenceNumber == 0, "ChildInfoOutput[SCA_CHILD_4].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].ForceFlag == 0, "ChildInfoOutput[SCA_CHILD_4].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].TimeoutPeriod == 0, "ChildInfoOutput[SCA_CHILD_4].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].TimeNextCmd == 0, "ChildInfoOutput[SCA_CHILD_4].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].CurrentTime_Last == 0, "ChildInfoOutput[SCA_CHILD_4].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].RunStatus == SCA_CHILD_NOT_USED, "ChildInfoOutput[SCA_CHILD_4].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt stopped"),
        "ATS filename.txt stopped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child4Main_StopATS */

void SCA_Child4Main_ExecutionCompleted (void)
{
    /**
     **  Test case: SCA_Child4Main - Execution Completed
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_4].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_4].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_4].StopATS    = FALSE;

    /* Execute the function being tested */
    SCA_Child4Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].ChildTaskID == 0, "ChildInfo[SCA_CHILD_4].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_4].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].FileDescriptor == 0, "ChildInfo[SCA_CHILD_4].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].CommandStatus == 0, "ChildInfo[SCA_CHILD_4].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_4].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].ForceFlag == 0, "ChildInfo[SCA_CHILD_4].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_4].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_4].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_4].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].ChildAtsFile[0] == '\0', "ChildInfo[SCA_CHILD_4].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].StopATS == FALSE, "ChildInfo[SCA_CHILD_4].StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_4].RunStatus == SCA_CHILD_NOT_USED, "ChildInfo[SCA_CHILD_4].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].ChildAtsFile[0] == '\0', "ChildInfoOutput[SCA_CHILD_4].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].ChildName[0] == '\0', "ChildInfoOutput[SCA_CHILD_4].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].ChildTaskID == 0, "ChildInfoOutput[SCA_CHILD_4].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].CommandStatus == 0, "ChildInfoOutput[SCA_CHILD_4].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].CommandSequenceNumber == 0, "ChildInfoOutput[SCA_CHILD_4].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].ForceFlag == 0, "ChildInfoOutput[SCA_CHILD_4].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].TimeoutPeriod == 0, "ChildInfoOutput[SCA_CHILD_4].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].TimeNextCmd == 0, "ChildInfoOutput[SCA_CHILD_4].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].CurrentTime_Last == 0, "ChildInfoOutput[SCA_CHILD_4].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_4].RunStatus == SCA_CHILD_NOT_USED, "ChildInfoOutput[SCA_CHILD_4].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt Execution Completed"),
        "ATS filename.txt Execution Completed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child4Main_ExecutionCompleted */

void SCA_Child5Main_StopATS (void)
{
    /**
     **  Test case: SCA_Child5Main - StopATS
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_5].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_5].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_5].StopATS    = TRUE;

    /* Execute the function being tested */
    SCA_Child5Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].ChildTaskID == 0, "ChildInfo[SCA_CHILD_5].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_5].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].FileDescriptor == 0, "ChildInfo[SCA_CHILD_5].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].CommandStatus == 0, "ChildInfo[SCA_CHILD_5].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_5].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].ForceFlag == 0, "ChildInfo[SCA_CHILD_5].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_5].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_5].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_5].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].ChildAtsFile[0] == '\0', "ChildInfo[SCA_CHILD_5].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].StopATS == FALSE, "ChildInfo[SCA_CHILD_5].StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].RunStatus == SCA_CHILD_NOT_USED, "ChildInfo[SCA_CHILD_5].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].ChildAtsFile[0] == '\0', "ChildInfoOutput[SCA_CHILD_5].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].ChildName[0] == '\0', "ChildInfoOutput[SCA_CHILD_5].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].ChildTaskID == 0, "ChildInfoOutput[SCA_CHILD_5].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].CommandStatus == 0, "ChildInfoOutput[SCA_CHILD_5].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].CommandSequenceNumber == 0, "ChildInfoOutput[SCA_CHILD_5].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].ForceFlag == 0, "ChildInfoOutput[SCA_CHILD_5].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].TimeoutPeriod == 0, "ChildInfoOutput[SCA_CHILD_5].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].TimeNextCmd == 0, "ChildInfoOutput[SCA_CHILD_5].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].CurrentTime_Last == 0, "ChildInfoOutput[SCA_CHILD_5].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].RunStatus == SCA_CHILD_NOT_USED, "ChildInfoOutput[SCA_CHILD_5].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_STOPATS_CMD_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt stopped"),
        "ATS filename.txt stopped");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child5Main_StopATS */

void SCA_Child5Main_ExecutionCompleted (void)
{
    /**
     **  Test case: SCA_Child5Main - Execution Completed
     **/

    int16 FilenameLength = 12;
    char  Filename[FilenameLength];

    strncpy (Filename, "filename.txt", FilenameLength);

    strncpy (SCA_OperData.ChildInfo[SCA_CHILD_5].ChildAtsFile, Filename, FilenameLength);

    SCA_OperData.ChildInfo[SCA_CHILD_5].RunStatus  = SCA_CHILD_EXECUTING;
    SCA_OperData.ChildInfo[SCA_CHILD_5].StopATS    = FALSE;

    /* Execute the function being tested */
    SCA_Child5Main();
    
    /* Verify results */
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].ChildTaskID == 0, "ChildInfo[SCA_CHILD_5].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].ChildName[0] == '\0', "ChildInfo[SCA_CHILD_5].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].FileDescriptor == 0, "ChildInfo[SCA_CHILD_5].FileDescriptor == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].CommandStatus == 0, "ChildInfo[SCA_CHILD_5].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].CommandSequenceNumber == 0, "ChildInfo[SCA_CHILD_5].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].ForceFlag == 0, "ChildInfo[SCA_CHILD_5].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].TimeoutPeriod == 0, "ChildInfo[SCA_CHILD_5].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].TimeNextCmd == 0, "ChildInfo[SCA_CHILD_5].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].CurrentTime_Last == 0, "ChildInfo[SCA_CHILD_5].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].ChildAtsFile[0] == '\0', "ChildInfo[SCA_CHILD_5].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].StopATS == FALSE, "ChildInfo[SCA_CHILD_5].StopATS == FALSE");
    UtAssert_True(SCA_OperData.ChildInfo[SCA_CHILD_5].RunStatus == SCA_CHILD_NOT_USED, "ChildInfo[SCA_CHILD_5].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].ChildAtsFile[0] == '\0', "ChildInfoOutput[SCA_CHILD_5].ChildAtsFile[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].ChildName[0] == '\0', "ChildInfoOutput[SCA_CHILD_5].ChildName[0] == '\0'");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].ChildTaskID == 0, "ChildInfoOutput[SCA_CHILD_5].ChildTaskID == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].CommandStatus == 0, "ChildInfoOutput[SCA_CHILD_5].CommandStatus == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].CommandSequenceNumber == 0, "ChildInfoOutput[SCA_CHILD_5].CommandSequenceNumber == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].ForceFlag == 0, "ChildInfoOutput[SCA_CHILD_5].ForceFlag == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].TimeoutPeriod == 0, "ChildInfoOutput[SCA_CHILD_5].TimeoutPeriod == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].TimeNextCmd == 0, "ChildInfoOutput[SCA_CHILD_5].TimeNextCmd == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].CurrentTime_Last == 0, "ChildInfoOutput[SCA_CHILD_5].CurrentTime_Last == 0");
    UtAssert_True(SCA_OperData.ChildInfoOutput[SCA_CHILD_5].RunStatus == SCA_CHILD_NOT_USED, "ChildInfoOutput[SCA_CHILD_5].RunStatus == SCA_CHILD_NOT_USED");

    UtAssert_True
        (Ut_CFE_EVS_EventSent(SCA_ATS_COMPL_INFO_EID, CFE_EVS_INFORMATION, "ATS filename.txt Execution Completed"),
        "ATS filename.txt Execution Completed");

    UtAssert_True (Ut_CFE_EVS_GetEventQueueDepth() == 1, "Ut_CFE_EVS_GetEventQueueDepth() == 1");

} /* end SCA_Child5Main_ExecutionCompleted */

void SCA_Child_Test_AddTestCases(void)
{
    UtTest_Add(SCA_ProcessFile_Test_StopAts, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessFile_Test_StopAts");
    UtTest_Add(SCA_ProcessFile_Test_RunFullAts, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessFile_Test_RunFullAts");
    UtTest_Add(SCA_ProcessFile_Test_FileOpenError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessFile_Test_FileOpenError");
    UtTest_Add(SCA_ProcessFile_Test_CloseFileError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessFile_Test_CloseFileError");
    UtTest_Add(SCA_GetNextCommand_Test_StopAts, SCA_Test_Setup, SCA_Test_TearDown, "SCA_GetNextCommand_Test_StopAts");
    UtTest_Add(SCA_GetNextCommand_Test_AllNominalCases, SCA_Test_Setup, SCA_Test_TearDown, "SCA_GetNextCommand_Test_AllNominalCases");
    UtTest_Add(SCA_GetNextCommand_Test_ReadFileError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_GetNextCommand_Test_ReadFileError");
    UtTest_Add(SCA_ProcessAllParameters_Test_Nominal, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessAllParameters_Test_Nominal");
    UtTest_Add(SCA_ProcessAllParameters_Test_BadStringError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ProcessAllParameters_Test_BadStringError");
    UtTest_Add(SCA_SendCommand_Test_StopAts, SCA_Test_Setup, SCA_Test_TearDown, "SCA_SendCommand_Test_StopAts");
    UtTest_Add(SCA_SendCommand_Test_ForceFlagFalseSendLater, SCA_Test_Setup, SCA_Test_TearDown, "SCA_SendCommand_Test_ForceFlagFalseSendLater");
    UtTest_Add(SCA_SendCommand_Test_ForceFlagFalseSendNow, SCA_Test_Setup, SCA_Test_TearDown, "SCA_SendCommand_Test_ForceFlagFalseSendNow");
    UtTest_Add(SCA_SendCommand_Test_ForceFlagTrueSendLater, SCA_Test_Setup, SCA_Test_TearDown, "SCA_SendCommand_Test_ForceFlagTrueSendLater");
    UtTest_Add(SCA_SendCommand_Test_ForceFlagTrueSendNow, SCA_Test_Setup, SCA_Test_TearDown, "SCA_SendCommand_Test_ForceFlagTrueSendNow");
    UtTest_Add(SCA_SendCommand_Test_ForceFlagFalseSkippedCmd, SCA_Test_Setup, SCA_Test_TearDown, "SCA_SendCommand_Test_ForceFlagFalseSkippedCmd");
    UtTest_Add(SCA_SendCommand_Test_ForceFlagFalseFailedSend, SCA_Test_Setup, SCA_Test_TearDown, "SCA_SendCommand_Test_ForceFlagFalseFailedSend");
    UtTest_Add(SCA_SendCommand_Test_ForceFlagTrueSkippedCmd, SCA_Test_Setup, SCA_Test_TearDown, "SCA_SendCommand_Test_ForceFlagTrueSkippedCmd");
    UtTest_Add(SCA_SendCommand_Test_ForceFlagTrueFailedSend, SCA_Test_Setup, SCA_Test_TearDown, "SCA_SendCommand_Test_ForceFlagTrueFailedSend");
    UtTest_Add(SCA_ChildSleepUntilCommandTime_Test_TimeDriftError, SCA_Test_Setup, SCA_Test_TearDown, "SCA_ChildSleepUntilCommandTime_Test_TimeDriftError");
    UtTest_Add(SCA_Child1Main_StopATS, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child1Main_StopATS");
    UtTest_Add(SCA_Child1Main_ExecutionCompleted, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child1Main_ExecutionCompleted");
    UtTest_Add(SCA_Child2Main_StopATS, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child2Main_StopATS");
    UtTest_Add(SCA_Child2Main_ExecutionCompleted, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child2Main_ExecutionCompleted");
    UtTest_Add(SCA_Child3Main_StopATS, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child3Main_StopATS");
    UtTest_Add(SCA_Child3Main_ExecutionCompleted, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child3Main_ExecutionCompleted");
    UtTest_Add(SCA_Child4Main_StopATS, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child4Main_StopATS");
    UtTest_Add(SCA_Child4Main_ExecutionCompleted, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child4Main_ExecutionCompleted");
    UtTest_Add(SCA_Child5Main_StopATS, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child5Main_StopATS");
    UtTest_Add(SCA_Child5Main_ExecutionCompleted, SCA_Test_Setup, SCA_Test_TearDown, "SCA_Child5Main_ExecutionCompleted");
}
