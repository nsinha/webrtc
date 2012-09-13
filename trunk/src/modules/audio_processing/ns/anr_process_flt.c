/*****************************************************************/
/* DITECH NETWORKS INC.						                     */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* C Code												         */
/*                                                               */
/* File Name       : anr_process.c                               */
/* Author          :					                         */
/* Date            :			                                 */
/*---------------------------------------------------------------*/
/* Revision History:                                             */
/*****************************************************************/
#include "mpu_types.h"
#include "anr_const.h"
#include "anr_tab.h"
#include "anr_process.h"
#include "anr_vad.h"

#if LOW_COMPLEXITY_SYNTHESIS
void down_sample_spectrum (Word16 *buff, Word16 len);
#endif
// Deafault ANR Config parameters
ANR_CFG_FLT anrCfg ={3,3,0,0,0,0};//,0,0,0,0};

/************************************************************************
 *
 * FUNCTION NAME: NS_cfg()
 *
 * PURPOSE:
 *
 *   Ditech enhanced noise suppression module
 *
 * INPUTS/OUTPUTS:
 *
 *   *pANRState:(I/O) pointer of state variable structure of ANR_process
 *   *pANRCfg  :(I/O) pointer of state variable structure of ANR_process
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/
void NS_cfg_flt (ANR_STATE_FLT *pANRState, ANR_CFG_FLT *pANRCfg)
{
    (pANRState->anr_cfg).hc_flag = pANRCfg->hc_flag;
    if (pANRCfg->hc_flag)
	{
       pANRState->noise_level = anr_cfg_tab_flt[HC_ANR_NOISE_LEVEL_DEFAULT]; //mapping
       pANRState->cng_flag = pANRCfg->ns_cng_flag;
	}
	else
	{
       pANRState->noise_level = anr_cfg_tab_flt[(pANRState->anr_cfg).noise_level]; //mapping
       if ((pANRState->anr_cfg).anc_adaptive_mode)
	   {
          pANRState->noise_level = anr_cfg_tab_flt[0] ;	// initial default reduction is 21dB
	   }
       pANRState->cng_flag = (pANRState->anr_cfg).anc_cng_flag ;
	}
}

void NS_getCfg_flt (ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg)
{

    pANRCfg->hc_flag = (pANRState->anr_cfg).hc_flag;
}

/************************************************************************
 *
 * FUNCTION NAME: SS_cfg()
 *
 * PURPOSE:
 *
 *   Ditech enhanced noise suppression module
 *
 * INPUTS/OUTPUTS:
 *
 *   *pANRState:(I/O) pointer of state variable structure of ANR_process
 *   *pANRCfg  :(I/O) pointer of state variable structure of ANR_process
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/
void SS_cfg_flt (ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg)
{
    (pANRState->anr_cfg).ss_flag = pANRCfg->ss_flag;
}

void SS_getCfg_flt (ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg)
{
    pANRCfg->ss_flag = (pANRState->anr_cfg).ss_flag;
}

/************************************************************************
 *
 * FUNCTION NAME: ANR_cfg()
 *
 * PURPOSE:
 *
 *   Ditech noise suppression module
 *
 * INPUTS/OUTPUTS:
 *
 *   *pANRState:(I/O) pointer of state variable structure of ANR_process
 *   *pANRCfg  :(I/O) pointer of state variable structure of ANR_process
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/
 void ANR_cfg_flt (ANR_STATE_FLT *pANRState, ANR_CFG_FLT *pANRCfg)
 {
 	(pANRState->anr_cfg).anc_adaptive_mode = pANRCfg->anc_adaptive_mode;
 	if (pANRCfg->anc_adaptive_mode)
 	{
        pANRState->max_snr = (F)anr_max_snr_tab[pANRCfg->noise_level];
        pANRState->noise_level = anr_cfg_tab_flt[2]; // default initial attenuation level 15dB
 	}
 	else
 	{
        pANRState->noise_level = anr_cfg_tab_flt[pANRCfg->noise_level]; //mapping
 	}

 	(pANRState->anr_cfg).noise_level = pANRCfg->noise_level;
 	(pANRState->anr_cfg).anc_cng_flag = pANRCfg->anc_cng_flag;
 	(pANRState->anr_cfg).ns_cng_flag = pANRCfg->ns_cng_flag;
 	pANRState->cng_flag = pANRCfg->anc_cng_flag;
}

void ANR_getCfg_flt (ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg)
{
 	pANRCfg ->noise_level = (pANRState->anr_cfg).noise_level;
// 	pANRCfg ->max_snr_level = (pANRState->anr_cfg).max_snr_level ;
 	pANRCfg ->hc_flag = (pANRState->anr_cfg).hc_flag;
 	pANRCfg ->ss_flag = (pANRState->anr_cfg).ss_flag;
	pANRCfg ->anc_cng_flag = (pANRState->anr_cfg).anc_cng_flag ;
 	pANRCfg ->ns_cng_flag = (pANRState->anr_cfg).ns_cng_flag;
 	pANRCfg ->anc_adaptive_mode = (pANRState->anr_cfg).anc_adaptive_mode;
//	pANRCfg ->frame_loss_handling = (pANRState->anr_cfg).frame_loss_handling ;
// 	pANRCfg ->unvoiced_sensitivity = (pANRState->anr_cfg).unvoiced_sensitivity;
// 	pANRCfg ->voiced_sensitivity = (pANRState->anr_cfg).voiced_sensitivity;
}


/************************************************************************
 *
 * FUNCTION NAME: ANR_init()
 *
 * PURPOSE:
 *
 *   Initialize state variables or memory of ANR_process module.
 *
 * INPUTS/OUTPUTS:
 *
 *   *pANRState:(I/O) pointer of state variable structure of ANR_process
 *   *pANRCfg  :(I/O) pointer of state variable structure of ANR_process
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/
void ANR_FLT_init (ANR_STATE_FLT *pANRState)
 {
 	short	i ;

 	pANRState->bypass_flag = 0;	// default no bypass

 	for (i = 0; i<FFTLENGTH/2+1; i++)
 	{
 		pANRState->fest_noise[i] = 0;
 		pANRState->spec_gain[i] = 0;
 	}

 	for (i = 0; i < FFTLENGTH/2+1; i++)
 	{
 		pANRState->fall_alpha[i] = ALPHA_MIN;
 		pANRState->fall_beta[i] = BETA_MIN;

 	}

 	pANRState->first_speech = 0 ;

 	pANRState->sigEn32 = 0 ;
 	pANRState->noiseEn32 = 0 ;
 	pANRState->sigStartCnt = 64 ;
 	pANRState->noiseStartCnt = 64 ;

 	pANRState->realNoiseEn32 = 0;
 	for (i=0; i<NUM_CRITICAL_BAND; i++)
 	{
 		pANRState->avgNoiseEdB[i] = 0;
 	}
 	pANRState->avgDiff = 0;

 	pANRState->NoiseState = 0; // 0: True Noise, 1: Artificial noise, 2: speech
 	pANRState->CntTnToAn = 3;  // 50ms
 	pANRState->CntAnToTn = 5;
 	// Initializing ANR deafult parameters
 	ANR_cfg_flt(pANRState,&anrCfg);
}


/************************************************************************
 *
 * FUNCTION NAME: ANR_process()
 *
 * PURPOSE:
 *
 *   Main function for Ditech noise suppression module.
 *
 * INPUTS/OUTPUTS:
 *
 *   *pANRState  :
 *   vad_flag    :
 *   *buff       : pointer of processing buffer with FFT input signal in complex
 *   *fmag       : input magnitudes
 *   ShiftIn     : input scale factor before FFT
 *   *pANRStateHc:
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/
void	ANR_process_flt (
ANR_STATE_FLT *pANRState,
FLOAT	vad_flag,
FLOAT	*buff,		// processing buffer with FFT of input signal in complex
FLOAT	*fmag,		// input magnitudes
ANR_STATE_FLT *pANRStateHc
)
{

   /*=========================================================*/
   /*	!!! Start Main ANR Processing !!!                     */
   /*=========================================================*/
    anr_flt (pANRState, vad_flag, buff, fmag, pANRStateHc, 0);



#if HC_ANR_PROCESS1
   /*=========================================================*/
   /*	!!! Start High-Complexity ANR Processing !!!          */
   /*=========================================================*/
    if ((pANRStateHc->anr_cfg).hc_flag)
    {
       //EANR_process (pANRStateHc, vad_flag, buff, v_vector, Shift_in);
       anr_flt (pANRState, vad_flag, buff, fmag, pANRStateHc, 1);
    }
#endif

}
