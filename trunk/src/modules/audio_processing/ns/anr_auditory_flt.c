/*****************************************************************/
/* DITECH NETWORKS INC.						                     */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* C Code Module for StarCore DSP (SC140/3400)/TMS320C54x        */
/*                                                               */
/* File Name       : anr_auditory.c                              */
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
#include "anr_tab.h"
#include "anr_math.h"

static double   log10_safe(double arg);

FLOAT convolve_flt (FLOAT *x, const FLOAT *y, int size)
{
    int i;
    FLOAT accA;

    accA = *x-- * *y++;		// fC[i] = Table_SF[0] * fB[i+2];
    for (i=1; i<size; i++)
    {
        accA += *x-- * *y++;
    }

	return (accA);
}

//
// input fmag[] is scaled by shift.
// output th[] is Q.8 (7,8)
//
//void fix_compute_mask_threshold_flt (Word32 *fmag, Word16 shift, Word16 *fth)
void fix_compute_mask_threshold_flt (FLOAT *fmag, FLOAT *fth)
{
    int	i, j, k;
    FLOAT	fB[NUM_CRITICAL_BAND], fC[NUM_CRITICAL_BAND];
    FLOAT  ltemp1;

    for (i=0; i<NUM_CRITICAL_BAND; i++)
	  {
        fB[i] = (F)0.0;
        for (k=cband[2*i]; k<=cband[2*i+1]; k++)
		    {
            fB[i] += fmag[k];
		    }
	  }

 /*-----------------------------------------*/
    /* Convolve power spectrum, fB[i] with spreading function, Table_SF[i] */
    fC[0] = convolve_flt (&fB[2], Table_SF_flt, 3);
    fC[1] = convolve_flt (&fB[3], Table_SF_flt, 4);
    fC[2] = convolve_flt (&fB[4], Table_SF_flt, 5);
    fC[3] = convolve_flt (&fB[5], Table_SF_flt, 6);
    fC[4] = convolve_flt (&fB[6], Table_SF_flt, 7);
    /* fC[5]~fC[15] */
    for (i=5; i<16; i++)
    {
		    ltemp1 = fB[i+2] * Table_SF_flt[0];
		    for (j=1; j<8; j++)
		    {
		    	  ltemp1 += fB[i+2-j] * Table_SF_flt[j];
		    }
		    fC[i] = ltemp1;
	  }
    fC[16] = convolve_flt (&fB[17], &Table_SF_flt[1], 7);
    fC[17] = convolve_flt (&fB[17], &Table_SF_flt[2], 6);

/*    stemp1 = shl (shift,  Q10_SHIFT);	     //Q.10
    ltemp2 = L_mpy_ls (LOG10_2_Q31, stemp1); //Q.26
*/
    for ( i = 0; i < NUM_CRITICAL_BAND; i++ )
	  {

        ltemp1 = (F)log10_safe(fC[i]) + (F)1.203089;//(F)0.3 + (F)0.903089;  // fC[i] is scaled by shift
		    ltemp1 += LOG10_2_10;
        ltemp1 = (F)10.0 * ltemp1;
		    ltemp1 += Roffset[i]; 
        ltemp1 -= Tnorm_flt[i]; 
		    fth[i] = ltemp1;
        if (ltemp1 < (F)20.0) fth[i] =  (F)20.0;	//(F)20.0 is floating pt equivalent of QUITE_THR_Q8
    }
}
//
// Input fThr[] is in Q.8
// Output: alpha[] in Q.12
// Output beta[] in Q.15
//


void fix_get_alpha_beta_flt (FLOAT *fThr, FLOAT *falpha, FLOAT *fbeta)
{
    int   i, j;

    FLOAT	fminT, fmaxT;
    FLOAT	fJNT;
    FLOAT	faa[NUM_CRITICAL_BAND], fbb[NUM_CRITICAL_BAND];

    FLOAT	stemp1, stemp2;//, stemp4, stemp5;
    FLOAT	fa, fb;

    FLOAT  ltemp;


    fminT = fThr[0];
    fmaxT = fThr[0];

    for (i=1; i<NUM_CRITICAL_BAND; i++)
	  {
        if (fminT > fThr[i]) fminT = fThr[i];
        if (fmaxT < fThr[i]) fmaxT = fThr[i];
	  }

    if (fminT == fmaxT)
	  {
       for ( i = 0; i < NUM_CRITICAL_BAND; i++)
	     {
           faa[i] = (F)3.0;//faa[i] = ALPHA_MIN_Q12;
           fbb[i] = (F)0.01;//fbb[i] = BETA_MIN_Q15 ;
	     }
	  }
    else
	  {
		   stemp1 = fmaxT - fminT;           
       for (i = 0; i<NUM_CRITICAL_BAND; i++)
	     {
           fJNT = fThr[i];

           stemp2 = fJNT - fminT;

		       stemp2 = stemp2/stemp1;

           faa[i] = (F)3.0 * stemp2;		//(F)3.0 is floating pt equivalent of ALPHA_DIFF_Q12
           fbb[i] = (F)0.09 * stemp2;		//(F)0.09 is floating pt equivalent of BETA_DIFF_Q15

           faa[i] = (F)6.0 - faa[i];		//(F)6.0 is floating pt equivalent of ALPHA_MAX_Q12
           fbb[i] = (F)0.1 - fbb[i];		//(F)0.1 is floating pt equivalent of BETA_MAX_Q15
	     }
	  }

    stemp1 = (F)0.125;				//(F)0.125 is floating pt equivalent of ALPHA_BETA_SMOOTHING
    stemp2 = (F)0.875;				//(F)0.875 is floating pt equivalent of 1 - ALPHA_BETA_SMOOTHING

    for (j=0; j<NUM_CRITICAL_BAND; j++)
	  {
        fa = *falpha;
        fb = *fbeta;

       /*------------------------------
        stemp3 = mult (fa, stemp2);
        stemp4 = mult (faa[j], stemp1);
        fa = add (stemp4, stemp3);
        -------------------------------*/
        ltemp = faa[j] * stemp1;
		    ltemp += fa * stemp2;
		    fa    = ltemp;

       /*------------------------------
        stemp3 = mult (fb, stemp2);
        stemp5 = mult (fbb[j], stemp1);
        fb = add (stemp5, stemp3);
        -------------------------------*/
        ltemp = fbb[j] * stemp1;
		    ltemp += fb * stemp2;
		    fb    = ltemp;

        for (i=cband[2*j]; i<=cband[2*j+1] ; i++)
		    {
            *falpha++ = fa;
            *fbeta++  = fb;
		    }
	  }
}


/*=============================================================================*/
void fix_post_gain_flt (FLOAT *fthr, FLOAT *fnoise, FLOAT *in_out, FLOAT *noise_level,
                    int ss_flag, int *active_cnt, FLOAT flag,
                    int *first_speech, int cng_flag)
{
    int	i, k, temp ;

	FLOAT	fB[NUM_CRITICAL_BAND];
    FLOAT	stemp1, stemp2, stemp3 ;
    FLOAT	ltemp1, ltemp2 ;
    FLOAT	fgain[NUM_CRITICAL_BAND];
    FLOAT   fprev_gain ;
    FLOAT	flamda, flamda1 ;
    FLOAT	noise_level_true ;

    if (ss_flag)
    {
         /* Only if SS feature is turned on */
         if (!(*first_speech) && flag == (F)0.0)
	       {
            /* initialization */
            *first_speech = 1 ;
            *active_cnt = 0 ;
	       }
         else if (flag != (F)0.0) *first_speech = 0 ;
         
         if (flag == (F)0.0)
         {
              (*active_cnt) ++ ;
              if ( *active_cnt >= 32767 ) *active_cnt = 10 ;
         }
         else  *active_cnt = 0;
    }

    for (i = 0; i < NUM_CRITICAL_BAND; i++)
    {
        ltemp1 = (F)0.0 ;
		    for (k=cband[2*i]; k<=cband[2*i+1]; k++)
        {
            ltemp1 += fnoise[k];	
        }

        ltemp1 *= cband_div_flt[i];

		    fB[i] = (F)log10_safe(ltemp1) + (F)1.203089;//	 + (F)9.331929; 
    }

    if (cng_flag)
    {
       for (i = 0; i < NUM_CRITICAL_BAND; i++)
       {
           
		       ltemp1 = fB[i];
		       stemp1 = (F)10.0 * ltemp1;
		       
		       stemp1 += OFFSET_DBOV;
           stemp1 -= anr_cng_thr_flt_tab[cng_flag];
		       temp = (int) stemp1;	
		       if (temp > ANR_CFG_HIGH_DB) temp = ANR_CFG_HIGH_DB; //21dB
           if (temp < ANR_CFG_LOW_DB)  temp = ANR_CFG_LOW_DB;  // 0dB

           noise_level_true = noise_level[i] ;
           if (noise_level_true < anr_Tbl_noise_level_flt[temp])
		       {
              noise_level_true = anr_Tbl_noise_level_flt[temp] ;
		       }

           ltemp1 += LOG10_1024;
           ltemp2 = fthr[i] * (F)0.100006; //Q.8//stemp1 = mult (fthr[i], (Word16)3277); //Q.8
           ltemp2 -= ltemp1;

           if (ltemp2 < (F)0.0)
           {
              ltemp1 = (F)exp(ltemp2);   //Q.31 about 22ops
              fgain[i] = ltemp1 + noise_level_true;
           }
           else
           {
              fgain[i] = (F)1.0;
           }
       }
	}
	else
	{
       for (i = 0; i < NUM_CRITICAL_BAND; i++)
       {
           ltemp1 = fB[i];
           noise_level_true = noise_level[i];
           ltemp1 += LOG10_1024;
           ltemp2 = fthr[i] * (F)0.100006; 
           ltemp2 -= ltemp1;

           if (ltemp2 < (F)0.0)
           {
              ltemp1 = (F)exp(ltemp2);   
              fgain[i] = ltemp1 + noise_level_true;
           }
           else
           {
              fgain[i] = (F)1.0;
           }
       }
	}

   /*----------------------------------------*/
   /* Only if SS feature is turned on        */
   /*----------------------------------------*/
	/** Not Required for Floating Point manipulation **/
/*    if (ss_flag)
    {
       for (i = 0; i < NUM_CRITICAL_BAND; i++)
       {
           fgain[i] = shr (fgain[i], 1); //use Q.14 for fgain
       }
*/
      /*--------------------------------------------------------*/
      /*  Additional boosting                                   */
      /*  Note the difference between NC and NUM_CRITICAL_BAND  */
      /*--------------------------------------------------------*/
/*       if (*active_cnt >= 10)
       {
          for (i = 1; i < NUM_CRITICAL_BAND; i++)
          {
              if (v_vector[i-1] >= 30) fgain[i] = add (fgain[i], shr(fgain[i], 8));// 1.125
              if (v_vector[i-1] <= 8)  fgain[i] = mult (fgain[i], (short)(29127)); // /1.125
          }
       }
    }
*/
   /*----------------------------------------*/
   /* Gain interpolations                    */
   /*----------------------------------------*/
    flamda = SSMOOTH_LAMDA;
    flamda1 = (F)0.9375; //(Word16)(ONE_Q15 - SSMOOTH_LAMDA_Q15);
    fprev_gain = fgain[0];
    ltemp1 = flamda1 * fprev_gain;
    for (k = cband[0]; k<=cband[1]; k++)
    {
		in_out[k] = ltemp1 + (flamda * in_out[k]);
    }

    for (i = 1; i < NUM_CRITICAL_BAND; i++)
    {
		    stemp3 = fgain[i] - fprev_gain;
		    stemp3 = stemp3 * cband_div_flt[i];

        for (k = cband[2*i]; k<=cband[2*i+1]; k++)
        {
            stemp2 = (F)(k-cband[2*i]);
            stemp2 *= stemp3;  
            stemp2 += fprev_gain;

            ltemp1 = flamda1 * stemp2;
            in_out[k] = ltemp1 + (flamda * in_out[k]);
        }
        fprev_gain = fgain[i];
    }

    return;
}
/*=============================================================================*/
//this is done to solve the problem of log being sent 0
double   log10_safe(double arg)
{
	double log_float_offset=0.000001;
	return log10(arg+log_float_offset);

}