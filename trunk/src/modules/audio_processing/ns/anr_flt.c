/*****************************************************************/
/* DITECH NETWORKS INC.						                     */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* C Code Module for StarCore DSP (SC140/3400)/TMS320C54x        */
/*                                                               */
/* File Name       : anr.c                                       */
/* Author          : Chunyan Li, JB Hong                         */
/* Date            : Aug/12/2006                                 */
/*---------------------------------------------------------------*/
/* Revision History:                                             */
/*   $Revision: 1.00 $ (NOV/06/2002) by Chunyan Li               */
/*              file created                                     */
/*   $Revision: 1.50 $ (AUG/12/2006) by JB HONG                  */
/*              Polished/refined codes for Freescale SC140.      */
/*****************************************************************/
#include  "anr_vad.h"
#include "anr_const.h"
#include "anr_auditory.h"
#include "vad.h"
#include "anr_tab.h"
#include "anr_math.h"

void anr_flt (
ANR_STATE_FLT *pANRState,
FLOAT  vad_flag,
FLOAT  *buff,		// processing buffer with FFT of input signal in complex
FLOAT  *fmag,		// input magnitudes
ANR_STATE_FLT *pANRStateHc,
int  hc_flag
)
{
    int   i;
    FLOAT  stemp1;
    FLOAT  ltemp, ltemp1, ltemp2;
   
	  

    FLOAT  fThreshold[NUM_CRITICAL_BAND]; 
    FLOAT  flamda, falpha, fbeta;
    FLOAT  ftmp_mag[FFTLENGTH/2+1];

    FLOAT  *pfmag;
    ANR_STATE_FLT *ptrANRState;


   /* pointer initialization by default */
    pfmag = fmag;
    ptrANRState = (ANR_STATE_FLT *)pANRState;

   /* Update the new power spectral */
    if (hc_flag)
    {
      /* pointer initialization for HC_ANR_PROCESS mode */
       pfmag = ftmp_mag;
       ptrANRState = (ANR_STATE_FLT *)pANRStateHc;

       compute_power_spectrum_FLT (buff, pfmag, FFTLENGTH);
      /*----------------------------------------------------------
       ftmp_mag[0] = L_mult (buff[0], buff[0]);
       ftmp_mag[FFTLENGTH/2] = L_mult (buff[1], buff[1]);
       for (i=1; i < FFTLENGTH/2; i++)
       {
           ltemp = L_mult (buff[2*i],buff[2*i]);
           pfmag[i] = L_mac (ltemp, buff[2*i+1], buff[2*i+1]);
       }
      ----------------------------------------------------------*/
    }


    flamda = NOISE_EST_LAMDA * vad_flag;
    
   /*---------------------------------------------------*/
   /* Noise spectral estimation for non speech period   */
   /*                                                   */
   /*  vad_flag = 32767 ==> flamda = 1*0.25    = 0.25   */
   /*  vad_flag = 24576 ==> flamda = 0.75*0.25 = 0.1875 */
   /*---------------------------------------------------*/
    if (vad_flag > (F)0.0)
    {
      /* if non active speech (silence or background noise) : vad_flag = 32768 or 24576 */
      /* pANRState->fest_noise[i] = pANRState->fest_noise[i]*(1-flamda) + fmag[i]*flamda */

       stemp1 = (F)1.0 - flamda;
       for (i=0; i< FFTLENGTH/2+1; i++)
       {
           ltemp1  = ptrANRState->fest_noise[i] * stemp1;
           ltemp2 =  pfmag[i] * flamda;
           ptrANRState->fest_noise[i] = ltemp1 + ltemp2;
       }
    }

    if (!hc_flag)
    {
      /*-------------------------------*/
      /* Initial Masking Curve         */
      /*-------------------------------*/
       for (i= 0; i < FFTLENGTH/2+1; i++ )
       {
           ltemp1 = fmag[i] - ptrANRState->fest_noise[i];
		       ftmp_mag[i] = (F)0.0;
           if (ltemp1 > (F)0.0)
           {
              ftmp_mag[i] = fmag[i] - ptrANRState->fest_noise[i];
           }
       }
    }

    fix_compute_mask_threshold_flt (ftmp_mag, fThreshold);

   /* compute gain function parameters */
    fix_get_alpha_beta_flt (fThreshold, ptrANRState->fall_alpha, ptrANRState->fall_beta);

   /* apply gain function */
    for (i=0; i< FFTLENGTH/2+1; i++)
    {
        falpha = ptrANRState->fall_alpha[i];
        fbeta  = ptrANRState->fall_beta[i];

        stemp1 = falpha + fbeta;
        ltemp1 = ptrANRState->fest_noise[i] * stemp1;

        ltemp  = ltemp1 - pfmag[i];

        if (ltemp < (F)0.0)
        {
           ltemp1 = ptrANRState->fest_noise[i] * falpha; 
           pfmag[i] -= ltemp1;
        }
        else
        {
           pfmag[i] = ptrANRState->fest_noise[i] * fbeta;
        }
    }

   /* 2nd masking curve computation */
    fix_compute_mask_threshold_flt (pfmag, fThreshold);

   /* apply the final gain function */
    fix_post_gain_flt (fThreshold, ptrANRState->fest_noise, ptrANRState->spec_gain,
                   ptrANRState->band_noise_level, (ptrANRState->anr_cfg).ss_flag,
                   &(ptrANRState->active_cnt), vad_flag,&(ptrANRState->first_speech),
				   ptrANRState->cng_flag);

   /*----------------------------------------------------*/
   /* Spectral Synthesis                                 */
   /*----------------------------------------------------*/
    if (! ptrANRState->bypass_flag)
	{
       if ((ptrANRState->anr_cfg).ss_flag)
       {
          buff[0] = ptrANRState->spec_gain[0] * buff[0];
          buff[1] = ptrANRState->spec_gain[FFTLENGTH/2] * buff[1];
          for (i = 1; i < FFTLENGTH/2; i++)
          {
              buff[2*i]   = ptrANRState->spec_gain[i] * buff[2*i];
              buff[2*i+1] = ptrANRState->spec_gain[i] * buff[2*i+1];
          }
       }
       else
       {
          buff[0] = ptrANRState->spec_gain[0] * buff[0];
          buff[1] = ptrANRState->spec_gain[FFTLENGTH/2] * buff[1];
          for (i = 1; i < FFTLENGTH/2; i++)
          {
              buff[2*i]   = ptrANRState->spec_gain[i] * buff[2*i];
              buff[2*i+1] = ptrANRState->spec_gain[i] * buff[2*i+1];
          }
       }
	}

    return;
}
