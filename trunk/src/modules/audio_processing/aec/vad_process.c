/*****************************************************************/
/* DITECH NETWORKS INC.						                     */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* C Code Module for StarCore DSP (SC140/3400)/TMS320C54x        */
/*                                                               */
/* File Name       : vad_process.c                               */
/* Author          : Chunyan Li, JB Hong, M Godavarti            */
/* Date            : Aug/12/2006                                 */
/*---------------------------------------------------------------*/
/* Revision History:                                             */
/*   $Revision: 1.00 $ (NOV/06/2002) by Chunyan Li               */
/*              file created                                     */
/*   $Revision: 1.50 $ (AUG/12/2006) by JB HONG                  */
/*              Polished/refined codes for Freescale SC140.      */
/*   $Revision: 2.00 $ (MAY/21/2007) by JB HONG & M Godavarti    */
/*              Anysis/Synthesis windowing changed.              */
/*****************************************************************/
#include  "anr_vad.h"
#include  "anr_tab.h"
#include  "vad.h"
#include  "vad_process.h"

#if RFFT_DITECH
#include  "r_fft.h"
#else
#include  "anr_r_fft_fx.h"
#endif

extern FLOAT   mag_flt[FFTLENGTH/2+1];
extern FLOAT   v_vector_f[NUM_CRITICAL_BAND];
extern FLOAT   EdB_f[NUM_CRITICAL_BAND];

/************************************************************************
 *
 * FUNCTION NAME: VAD_cfg()
 *
 * PURPOSE:
 *
 *   Initialize state variables or memory of VAD_process module.
 *
 * INPUTS/OUTPUTS:
 *
 *   *pVADState:(I/O) pointer of state variable structure of VAD_process
 *   *pVADCfg  :(I/O) pointer of state variable structure of VAD_process
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/
void VAD_cfg_FLT(VAD_STATE_FLT *pVADState, VAD_CFG_FLT *pVADCfg)
{
    pVADState->vad_op_mode = pVADCfg->vad_op_mode;
    pVADState->vad_pattern_flag = pVADCfg->vad_pattern_flag;
}
void VAD_cfg_standard_FLT(VAD_STATE_FLT *pVADState)
{
    pVADState->vad_op_mode = VAD_MODE_NORMAL;
    pVADState->vad_pattern_flag = VAD_IGNORE_PATTERN_STANDARD;
}

void VAD_getcfg_FLT(VAD_STATE_FLT *pVADState, VAD_CFG_FLT *pVADCfg)
{
    pVADCfg ->vad_op_mode = pVADState->vad_op_mode;
    pVADCfg->vad_pattern_flag = pVADState->vad_pattern_flag;
}

/************************************************************************
 *
 * FUNCTION NAME: VAD_init()
 *
 * PURPOSE:
 *
 *   Initialize state variables or memory of VAD_process module.
 *
 * INPUTS/OUTPUTS:
 *
 *   *pVADState:(I/O) pointer of state variable structure of VAD_process
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/

void VAD_init_FLT (VAD_STATE_FLT *pVADState_flt)
{
	int	i;

	pVADState_flt->FrCnt = 0;

	for (i = 0; i < LOOK_AHEAD; i++)
	{
		pVADState_flt->Data_flt[i] = 0;
	}

#if !ZERO_PADDING_IN_FFT
	for (i = 0; i < HIST_SIZE; i++) //HIST_SIZE = FFTLENGTH-LOOK_AHEAD-FRAME_SIZE = 136
	{
		pVADState_flt->Data1_flt[i] = 0;
	}

#endif

	pVADState_flt->silence_cnt = 0;
	pVADState_flt->last_silence_cnt =0;
	pVADState_flt->burst_cnt = 0;

	// add VAD hang-over
	pVADState_flt->vad_state = VAD_NON_ACTIVE;
	pVADState_flt->offset_cnt = 0;
	pVADState_flt->onset_cnt = 0;

	pVADState_flt->active_cnt = 0;

	pVADState_flt->pattern_cnt = 0;
	pVADState_flt->Etot_dB_max_flt = (F)9.0; // Q.5

}
/***************************************************************************
 *
 * FUNCTION NAME: VAD_process()
 *
 * PURPOSE:
 *
 *   Ditech voice activity detection module for detecting speech pause
 *   for noise reduction. This is main function for Ditech VAD.
 *
 * INPUTS/OUTPUTS:
 *
 *   *pVADState:
 *   *in_data  :
 *   *buff     :
 *   *fmag     :
 *   *ShiftIn  :
 *   *EdB      :
 *   *v_vector :
 *
 * RETURN VALUE:
 *
 *   vad_flag
 *
 ************************************************************************/

FLOAT VAD_process_FLT(VAD_STATE_FLT *pVADState_flt, FLOAT *in_data, FLOAT *buff,
					FLOAT *fmag, FLOAT *EdB)
{
    int  N;

	FLOAT *in_data_flt;
	FLOAT *buff_flt;
	FLOAT vad_flag_flt;
	FLOAT *fmag_flt;
	FLOAT *EdB_flt;
	
	fmag_flt = fmag;
	EdB_flt = EdB;

	in_data_flt = in_data;
	buff_flt = buff;

#if ZERO_PADDING_IN_FFT
    N = FRAME_SIZE+LOOK_AHEAD+4;
#else
    N = FFTLENGTH;
#endif

   /*------------------------------------------------------*/
   /* construct a frame (256 samples) for 256-point RFFT   */
   /*------------------------------------------------------*/

	construct_FFTframe_FLT (buff_flt, in_data_flt, pVADState_flt->Data_flt,
#if !ZERO_PADDING_IN_FFT
                        pVADState_flt->Data1_flt,
#endif
                        FFTLENGTH);

   /*------------------------------------------------------*/
   /* Spectral Analysis using 256-point RFFT               */
   /*------------------------------------------------------*/

#if RFFT_DITECH

   /* Spectral Analysis using 256-point RFFT */

	rfft_flt (buff_flt, 256, +1);

#else

   /* Spectral Analysis using 256-point RFFT */
    r_fft_flt (buff, +1);

#endif


   /* Compute power spectrum in frequency domain : fmag[0:128] */

	compute_power_spectrum_FLT (buff_flt, fmag_flt, FFTLENGTH);
   /*------------------------------------------------------*/
   /* VAD                                                  */
   /*------------------------------------------------------*/

	vad_flag_flt =vad_FLT (pVADState_flt->FrCnt, fmag_flt, pVADState_flt, EdB_flt);
    pVADState_flt->FrCnt ++ ;

   /* take care of counter overflow */
    if (pVADState_flt->FrCnt == LW_MAX)
	{
       pVADState_flt->FrCnt = 10;	// 10 is the initial counter number for VAD
	}

    return (vad_flag_flt);
}

