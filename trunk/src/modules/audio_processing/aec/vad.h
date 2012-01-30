/*****************************************************************/
/* DITECH NETWORKS INC.                                          */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* C Code Module for StarCore DSP (SC140/3400) & TMS320C54x      */
/*                                                               */
/* File Name       : vad.h                                       */
/*                                                               */
/* PURPOSE :                                                     */
/*                                                               */
/*                                                               */
/* Author          : Chunyan Li, JB Hong, M Godavarti            */
/* Date            : Aug/12/2006                                 */
/*---------------------------------------------------------------*/
/* REVISION HISTORY :                                            */
/*   $Revision: 1.00 $ (SEP/16/2002) by Chunyan Li               */
/*              file created                                     */
/*   $Revision: 1.50 $ (AUG/12/2006) by JB HONG                  */
/*              Polished/refined codes for Freescale SC140.      */
/*   $Revision: 2.00 $ (MAY/21/2007) by JB HONG & M Godavarti    */
/*              Anysis/Synthesis windowing changed.              */
/*****************************************************************/
#ifndef	_VAD_H
#define	_VAD_H

#include	"anr_vad.h"


FLOAT vad_FLT (
int	fno,
FLOAT	*fmag,	// scaled by shift
VAD_STATE_FLT *pVADState_flt,
FLOAT	*EdB
);

void construct_FFTframe_FLT (FLOAT *new_data, FLOAT *curr_data,
                         FLOAT *old_data,
#if !ZERO_PADDING_IN_FFT
                         FLOAT *old_data1,
#endif
                         int lenRFFT);

void compute_power_spectrum_FLT (FLOAT buff[], FLOAT fmag[], int lenRFFT);

// added VAD hang-over for improve clean speech input @5/1/2003
FLOAT vad_smoothing_FLT (VAD_STATE_FLT *pVADState_flt, FLOAT in_flag);

#endif