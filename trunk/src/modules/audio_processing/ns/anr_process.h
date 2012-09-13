/*****************************************************************/
/* DITECH NETWORKS INC.                                          */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* C Code Module for StarCore DSP (SC140/3400) & TMS320C54x      */
/*                                                               */
/* File Name       : anr_process.h                               */
/*                                                               */
/* PURPOSE :                                                     */
/*                                                               */
/*                                                               */
/* Author          : Chunyan Li, JB Hong                         */
/* Date            : Aug/12/2006                                 */
/*---------------------------------------------------------------*/
/* REVISION HISTORY :                                            */
/*   $Revision: 1.00 $ (SEP/16/2002) by Chunyan Li               */
/*              file created                                     */
/*   $Revision: 1.50 $ (AUG/12/2006) by JB HONG                  */
/*              Polished/refined codes for Freescale SC140.      */
/*****************************************************************/
#ifndef	_ANR_PROCESS_H
#define	_ANR_PROCESS_H
#include "anr_vad.h"

void NS_cfg_flt (ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg);
void SS_cfg_flt (ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg);
void ANR_cfg_flt (ANR_STATE_FLT *pANRState, ANR_CFG_FLT *pANRCfg);
void ANR_FLT_init (ANR_STATE_FLT *pANRState);

void	ANR_process_flt (
ANR_STATE_FLT *pANRState,
FLOAT	vad_flag,
FLOAT	*buff,		
FLOAT	*fmag,		
ANR_STATE_FLT *pANRStateHc
);

#endif
