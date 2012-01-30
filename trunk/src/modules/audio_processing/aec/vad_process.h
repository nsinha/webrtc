/*****************************************************************/
/* DITECH NETWORKS INC.                                          */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* StarCore DSP (SC140/3400) C Code Module                       */
/*                                                               */
/* File Name       : vad_process.h                               */
/*                                                               */
/* PURPOSE :                                                     */
/*                                                               */
/*                                                               */
/* Author          : Chunyan Li, JB Hong                         */
/* Date            : Aug/12/2006                                 */
/*---------------------------------------------------------------*/
/* REVISION HISTORY :                                            */
/*  $Revision: 1.00 $ (SEP/16/2002) by Chunyan Li                */
/*             file created                                      */
/*  $Revision: 1.50 $ (AUG/12/2006) by JB HONG                   */
/*             Polished/refined codes for Freescale SC140.       */
/*  $Revision: 2.00 $ (MAY/21/2007) by JB HONG & M Godavarti     */
/*             One analysis and two levels of synthesis windows  */
/*             added and triggered when ZERO_PADDING_IN_FFT == 0.*/
/*****************************************************************/
#include "mpu_types.h"

#include "anr_const.h"
#ifndef	_VAD_PROCESS_H
#define	_VAD_PROCESS_H

/*---------------------------------------*/
/* Function Prototypes                   */
/*---------------------------------------*/

#if ZERO_PADDING_IN_FFT
void construct_FFTframe_FLT (FLOAT *new_data, FLOAT *curr_data,
                         FLOAT *old_data,
                         int lenRFFT);
#else
void construct_FFTframe_FLT (FLOAT *new_data, FLOAT *curr_data,
                         FLOAT *old_data,FLOAT *old_data1,
                         int lenRFFT);
#endif
void compute_power_spectrum_FLT (FLOAT buff[], FLOAT fmag[], int lenRFFT);

FLOAT	vad_FLT (int	fno,float	*fmag,	VAD_STATE_FLT *pVADState_flt,
float	*EdB);

FLOAT vad_smoothing_FLT (VAD_STATE_FLT *pVADState, FLOAT in_flag);
void VAD_cfg_standard_FLT(VAD_STATE_FLT *pVADState);
#endif
