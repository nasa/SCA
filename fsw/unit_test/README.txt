##############################################################################
## $Id: README.txt 1.1 2015/03/10 13:08:30EDT czogby Exp  $
##
## Purpose: CFS SCA application unit test instructions, results, and code coverage
##
## Author: Charles Zogby
##
## $Log: README.txt  $
## Revision 1.1 2015/03/10 13:08:30EDT czogby 
## Initial revision
## Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/unit_test/project.pj
##############################################################################


-------------------------
CF Unit Test Instructions
-------------------------
This unit test was run in a virtual machine running CentOS and uses the ut-assert stubs and default hooks for the 
cFE, OSAL and PSP. The ut-assert framework, stubs, and hooks are located in the directory cfe/tools/ut-assert and 
are configuration managed in MKS in the FSW tools repository on server tlserver3.

To run the unit test enter the following commands at the command line prompt in
unit test directory.

make clean
make 
make run
make gcov

SCA 1.3.0.0 Unit Test Results:

Tests Executed:    66
Assert Pass Count: 568
Assert Fail Count: 0

gcov: '../src/sca_atsrq.c' 100.00%  102
gcov: '../src/sca_cmds.c' 100.00%  73
gcov: '../src/sca_app.c' 100.00%  94
gcov: '../src/sca_utils.c' 100.00%  10
gcov: '../src/sca_child.c' 97.78%  316

==========================================================================
sca_atsrq.c - 100.00% coverage

==========================================================================
sca_cmds.c - 100.00% coverage

==========================================================================
sca_app.c - 100.00% coverage

==========================================================================
sca_utils.c - 100.00% coverage

==========================================================================
sca_child.c - 97.78% coverage (7 Lines of Code NOT executed)

In procedure SCA_GetNextCommand:

        -:  183:            /* Parse line, copying data items into variables */
        8:  184:            SscanfStatus = sscanf(Line,"%u,%u,%u,%[^','],%s",
        -:  185:                                  &SCA_OperData.ChildInfo[ChildInfoIndex].TimeNextCmd,
        -:  186:                                  &SCA_OperData.ChildInfo[ChildInfoIndex].ForceFlag,
        -:  187:                                  &SCA_OperData.ChildInfo[ChildInfoIndex].TimeoutPeriod,
        -:  188:                                  HeaderHex, DataString);
        -:  189:
        -:  190:            /* Not checking for all possible mistakes in the line - only checking if there is an error before any data could be read. */
        8:  191:            if (SscanfStatus == 0) // FIXME: should be EOF constant - see other FIXME.
        -:  192:            {
    #####:  193:                CFE_EVS_SendEvent( SCA_ATS_READ_CMD_ERR_EID, CFE_EVS_ERROR,
        -:  194:                                   "ATS %s command %i error reading from file",
        -:  195:                                   SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile,
        -:  196:                                   SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber);
        -:  197:    
    #####:  198:                SCA_OperData.HkPacket.AtsCmdErrCtr[ChildInfoIndex]++;
        -:  199:    
    #####:  200:                OS_MutSemTake(SCA_OperData.HkMutex); /* Protect data used by both the parent and children */
    #####:  201:                SCA_OperData.HkPacket.LastAtsCmdErrSeqNum = SCA_OperData.ChildInfo[ChildInfoIndex].CommandSequenceNumber;
    #####:  202:                strncpy (SCA_OperData.HkPacket.LastAtsCmdErrFile, SCA_OperData.ChildInfo[ChildInfoIndex].ChildAtsFile, OS_MAX_PATH_LEN);
    #####:  203:                OS_MutSemGive(SCA_OperData.HkMutex);
    #####:  204:                break;
        -:  205:            }

REASON: Unit testing this branch is not possible because it would require a stub function for sscanf, which is 
not defined by one of the packages covered by the unit test framework (cFE, OSAL, PSP).

==========================================================================
