/************************************************************************
** File:
**   $Id: sca_msg.h 1.1 2014/08/26 15:38:25EDT czogby Exp  $
**
** Purpose: 
**   Specification for the CFS Stored Command (SCA) command and telemetry 
**   message data types.
**
** Notes:
**   Constants and enumerated types related to these message structures
**   are defined in sca_msgdefs.h. They are kept separate to allow easy 
**   integration with ASIST RDL files which can't handle typedef
**   declarations (see the main comment block in sca_msgdefs.h for more 
**   info).
**
**   $Log: sca_msg.h  $
**   Revision 1.1 2014/08/26 15:38:25EDT czogby 
**   Initial revision
**   Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/sca/fsw/src/project.pj
*************************************************************************/



#ifndef _sca_msg_
#define _sca_msg_


/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "sca_child.h"
#include "sca_platform_cfg.h"
#include "sca_msgdefs.h"

/************************************************************************
** Macro Definitions
*************************************************************************/




/************************************************************************
** Type Definitions
*************************************************************************/

/** 
**  \scatlm Housekeeping Packet Structure
*/
typedef struct {
    uint8       TlmHeader[CFE_SB_TLM_HDR_SIZE];
                                                /**< \brief current ATS number  1 = ATS A, 2 = ATS B     */                                                                     
    uint8       CmdErrCtr;                       /**< \scatlmmnemonic \SCA_CMDEC */   
                                                 /**< \brief  Counts Request Errors  */
    uint8       CmdCtr;                          /**< \scatlmmnemonic \SCA_CMDPC */    
                                                 /**< \brief  Counts Ground Requests */
    uint16      AtsCmdCtr[SCA_MAX_NUM_ATS];      /**< \brief ATS cmd counter counts commands sent by each ATS */
    uint16      AtsCmdErrCtr[SCA_MAX_NUM_ATS];   /**< \brief ATS cmd Error ctr command errors in each ATS */

    uint16      LastAtsCmdErrSeqNum;
    char        LastAtsCmdErrFile[OS_MAX_PATH_LEN];

}   SCA_HkTlm_t;


/** 
**  \brief No Arguments Command
**  For command details see #SCA_NOOP_CC, #SCA_RESET_COUNTERS_CC, #SCA_STOP_ATS_CC, #SCA_SWITCH_ATS_CC
**  Also see #SCA_SEND_HK_MID
*/
typedef struct
{
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    
} SCA_NoArgsCmd_t;

/**
**  \brief Reset Counters Command
**  For command details see #SCA_RESET_COUNTERS_CC
*/
typedef struct
{
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8               ResetFlag;
} SCA_ResetCntrsCmd_t;

/**
**  \brief Start ATS Command
**  For command details see #SCA_START_ATS_CC
*/
typedef struct 
{
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char                AtsFilename[OS_MAX_PATH_LEN];
} SCA_StartAtsCmd_t;

/** 
**  \brief Stop ATS Command
**  For command details see #SCA_STOP_ATS_CC
*/
typedef struct 
{
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char                AtsFilename[OS_MAX_PATH_LEN];
} SCA_StopAtsCmd_t;

/** 
**  \brief Get ATS Info Command
**  For command details see #SCA_GET_ATS_INFO_CC
*/
typedef struct 
{
    uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char                OutputFilename[OS_MAX_PATH_LEN];
} SCA_GetATSInfoCmd_t;



#endif /* _sca_msg_ */

/************************/
/*  End of File Comment */
/************************/
