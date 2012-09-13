/*****************************************************************/
/* DITECH NETWORKS INC.                                          */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* StarCore DSP (SC140/3400) C Code Module                       */
/*                                                               */
/* File Name       : syn_process.h                               */
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

#ifndef _SYN_PROCESS_H
#define	_SYN_PROCESS_H


void SYN_init_FLT (SYN_STATE_FLT	*pSYNState);
void SYN_process_FLT (SYN_STATE_FLT *pSYNState, FLOAT *buff, FLOAT *out_data);

void syn_ovlp_add_FLT (FLOAT *old_buff, FLOAT *new_buff, FLOAT *out_buff);

#if LOW_COMPLEXITY_SYNTHESIS
void down_sample_spectrum_FLT (FLOAT *buff, int len);
#endif

#define SYN_run_FLT SYN_process_FLT

#endif
