/*****************************************************************/
/* DITECH NETWORKS INC.						                     */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* C Code Module											     */
/*                                                               */
/* File Name       : vad_flt.c                                   */
/* Author          :								             */
/* Date            :											 */
/*---------------------------------------------------------------*/
/* Description:                                                  */
/*              Ditech voice activity detection module for       */
/*              detecting speech pause for noise reduction.      */
/*---------------------------------------------------------------*/
/* Revision History:                                             */
/*																 */
/*****************************************************************/
#include <math.h>
#include  "anr_vad.h"

#include "vad.h"
//#include "anr_math.h"
#include "anr_tab.h"

FLOAT	vad_FLT (
int	fno,
float	*fmag,	// scaled by shift
VAD_STATE_FLT *pVADState_flt,
float	*EdB
)
{
    int   i, j;

    int		voicing;
    float   fEdb[NC]; 
    float   flag;
    float	fEtot;    
    float	Etot_dB;  
    float	fdelta_E; 

    float	salpha;
    float	channel_ener, ltemp1, ltemp2;
    float	stemp, stemp1;

    float	ltemp1_s;

    float	fEdb_s[NC]; 
    float	fdelta_E_s;

    float	fEntot;
    float	Edb_min, Edb_max;

    float	IgnoreTHR;
    int		updateFlag;

	int		stemp3;

   /*----------------------------------------------*/
   /* compute channel energy                       */
   /*----------------------------------------------*/
    
    fEtot = (F)0.0 ;
    for (i=0; i<NC; i++)
	{
       /*-----------------------------------------------------------*/
       /* compute Subband Channel Energy in each Critical Subband   */
       /*-----------------------------------------------------------*/
        channel_ener = (F)0.0;
        for (j=cband1[2*i]; j<=cband1[2*i+1]; j++)
		{
            channel_ener += fmag[j];
		}

       /*-----------------------------------------------------------*/
       /* pVADState->fEch[i]                                        */
       /*                                                           */
       /* compute Long-term Subband Channel Energy                  */
       /* fEch[i] = fEch[i]*0.45 + new_subband_energy(ltemp)*0.55   */
       /*-----------------------------------------------------------*/
        pVADState_flt->fEch_flt[i] = pVADState_flt->fEch_flt[i]*(F)0.45 + channel_ener*(F)0.55;
/*		stemp = (F)cband1[2*i+1] - cband1[2*i];
        stemp += (F)1.0;
        stemp *= (F)0.5;

		if (pVADState_flt->fEch_flt[i] < stemp) pVADState_flt->fEch_flt[i] = stemp;*/ //ECH_MIN= 1/2
       /*--------------------------------------------------------------------*/
       /* fEdb[i]                                                            */
       /*                                                                    */
       /* compute Average Long-term Subband Channel Energy per Frequency Bin */
       /*       in dB : 10*log10(subband energy/number of freq. bins) in Q.5 */
       /*--------------------------------------------------------------------*/
       /* Output of L_add() is log10(x) scaled by 2^26 for Q.26 */
        ltemp1 = (F)log10(pVADState_flt->fEch_flt[i]);// + (F)9.331929;
		ltemp1 += cband1_log_offset_flt[i];
        fEdb[i] = (F)10.0 * ltemp1;

       /*------------------------------------------------------------------------*/
       /* fEtot : compute Total Average Long-term Energy of Whole Frequency Band */
       /*------------------------------------------------------------------------*/
		//fEtot = L_add (fEtot, L_mpy_ls(pVADState->fEch[i], cband1_div[i]));//Q.26
		fEtot +=  pVADState_flt->fEch_flt[i] * cband1_div_flt[i]; 

//#if ADD_SHORT_CH_EN
       /*-------------------------------------------------------------------*/
       /* pVADState->fEch_s[i]                                              */
       /*                                                                   */
       /* compute Short-term Subband Channel Energy (time constant is 11ms) */
       /* fEch_s[i] = fEch_s[i]*0.1 + new_subband_energy(ltemp)*0.9         */
       /*-------------------------------------------------------------------*/
        pVADState_flt->fEch_s_flt[i] =  pVADState_flt->fEch_s_flt[i]*(F)0.1 + channel_ener * (F)0.9;	   
//		if (pVADState_flt->fEch_s_flt[i] < stemp) pVADState_flt->fEch_s_flt[i] = stemp; //ECH_MIN= 1/2
       /*---------------------------------------------------------------------*/
       /* fEdb_s[i]                                                           */
       /*                                                                     */
       /* compute Average Short-term Subband Channel Energy per Frequency Bin */
       /*       in dB : 10*log10(subband energy/number of freq. bins) in Q.5  */
       /*---------------------------------------------------------------------*/
       /* Output of L_add() is log10(x) scaled by 2^26 for Q.26 */
        ltemp1_s = (F)log10(pVADState_flt->fEch_s_flt[i]);// + (F)9.331929;
        ltemp1_s += cband1_log_offset_flt[i];
        fEdb_s[i] = (F)10.0 * ltemp1_s;
//#endif
	}

   /*-------------------------------------------------------*/
   /* compute average channel energy and energy deviations  */
   /*-------------------------------------------------------*/
    ltemp1 = (F)log10(fEtot);// + (F)9.331929;
	ltemp1 += (F)1.505149;
    
	Etot_dB = (F)10.0 * ltemp1;	

    if (fno == 0)
	{
       for (i=0; i<NC; i++)
	   {
           pVADState_flt->favgEdb_flt[i] = fEdb[i];
	   }
	}
    else
	{
       if (Etot_dB > (F)100.0)
	   {
          stemp = (F)0.97;
	   }
       else
       {
          stemp  = (F)85.0 - Etot_dB;   
		  stemp1 = stemp * (F)0.01225;
		  stemp = (F)0.99 - stemp1;   	
          
          if (stemp > (F)0.99)       stemp = (F)0.97;
          else if (stemp < (F)0.5)  stemp = (F)0.5;

       }

       salpha = stemp;
       stemp1 = (F)1.0 - salpha;

      /*------------------------------------------------------------------*/
      /* favgEdb is 10,5 (Q.5)                                            */
      /*                                                                  */
      /* favgEdb[i] = favgEdb[i]*alpha + fEdb[i]*(1-alpha)                */
      /*------------------------------------------------------------------*/
       for (i=0; i<NC; i++)
	   {
		   pVADState_flt->favgEdb_flt[i] = pVADState_flt->favgEdb_flt[i]*salpha + fEdb[i]*stemp1;	
	   }
	}

    fdelta_E = (F)0.0;		
    for (i=0; i<NC; i++)
	{
        stemp = fEdb[i] - pVADState_flt->favgEdb_flt[i];
        stemp = (F)fabs(stemp);
        fdelta_E += stemp;
	}

    fdelta_E_s = (F)0.0 ;		
    for (i = 0; i<NC; i++)
	{
        stemp = fEdb_s[i] - pVADState_flt->favgEdb_flt[i];
        fdelta_E_s += stemp;
	}

   /*----------------------------------------*/
   /* compute voice metric                   */
   /*----------------------------------------*/
    if (pVADState_flt->vad_op_mode == VAD_MODE_NORMAL)
	{
      /*----------------------------------------------------*/
      /* VAD_MODE_NORMAL                                    */
      /* VAD should be set as normal operating mode, when   */
      /*     idle code detection is not available. In this  */
      /*     mode, the VAD state variables, fEn[i] is built */
      /*     up exponentially;                              */
      /*     pVADState->fEn[i] = pVADState->fEn[i]*(15/16)  */
      /*                       + pVADState->fEch[i]*(1/16); */
      /*----------------------------------------------------*/
       if (fno < 1)
	   {
          for (i=0; i<NC; i++)
		  {
              pVADState_flt->fEn_flt[i] = pVADState_flt->fEch_flt[i] * cband1_div_flt[i];
			  if (pVADState_flt->fEn_flt[i] < (F)1.0) pVADState_flt->fEn_flt[i] = (F)1.0;//E_INIT = 2 (16 scaled by 8)*/
		  }
	   }
       else if (fno < 4)
	   {
		  for (i = 0; i < NC; i++)
		  {
			  pVADState_flt->fEn_flt[i] = pVADState_flt->fEn_flt[i] * (F)0.9375 
				  + pVADState_flt->fEch_flt[i] * cband1_div_flt[i];
			  if (pVADState_flt->fEn_flt[i] < (F)1.0) pVADState_flt->fEn_flt[i] = (F)1.0;//E_INIT = 2 (16 scaled by 8)*/
		  }
	   }
	}
    else
	{
       if (fno < 10)
	   {
          for (i=0; i<NC; i++)
		  {
              pVADState_flt->fEn_flt[i] = pVADState_flt->fEch_flt[i] * cband1_div_flt[i];
			  if (pVADState_flt->fEn_flt[i] < (F)1.0) pVADState_flt->fEn_flt[i] = (F)1.0;//E_INIT = 2 (16 scaled by 8)*/

/*			  ltemp1 = L_mpy_ls (pVADState->fEch[i], cband1_div[i]);
              pVADState->fEn[i] = L_shl(ltemp1, 2);//Q.28 to increase resolution
              if (pVADState->fEn[i] < 2) pVADState->fEn[i] = 2;//E_INIT = 2 (16 scaled by 8)*/
		  }
	   }
	}

    voicing = 0 ;
    for (i = 0; i<NC; i++)
	{
        ltemp1 = pVADState_flt->fEn_flt[i];	

        ltemp1 = (F)log10(ltemp1);// + (F)9.331929;
		ltemp1 += (F)0.903089;
        ltemp1 += (F)0.6;

		stemp1 = (F)10.0 * ltemp1;

        stemp1 = fEdb[i] - stemp1;

		//stemp3 = (int)(stemp1 * (F)0.083333);
		stemp3 = (int)(stemp1 * (F)2.666666);
        
        if (stemp3 > 89) stemp3 = 89;
        if (stemp3 < 0 ) stemp3 = 0;

        voicing = voicing + v_array[stemp3];
	}

   /*-------------------------------*/
   /* decision rules                */
   /*-------------------------------*/
    flag = (F)0.0;
    if (voicing <= UPDATE_THLD)
	{
      /* normal update logic */
       flag = (F)1.0;
       pVADState_flt->silence_cnt = 0;
	}
	else if ( (Etot_dB > (F)0.0625) && //NOISE_FLOOR_DB_Q5 = 16
              (fdelta_E < (F)31.0 ) )
	{
      /* find the minimum and maximum */
       Edb_max = fEdb_s[0];
       Edb_min = fEdb_s[0];
       for (i=1; i<(NC-1); i++)
	   {	// not include the last band
           if (Edb_max < fEdb_s[i]) Edb_max = fEdb_s[i];
           if (Edb_min > fEdb_s[i]) Edb_min = fEdb_s[i];
	   }

       stemp = Edb_max - Edb_min;

     /* get Entot */
       fEntot = (F)0.0;
       for (i=0; i<NC; i++)
	   {
			fEntot += pVADState_flt->fEn_flt[i];
	   }

       if ( !((voicing >= PURE_TONE_V_THR) && // PURE_TONE_V_THR  = 845
		     (fEntot < PURE_TONE_EN_THR)   && // PURE_TONE_EN_THR = 17
			 (stemp > Edb_min) ) )
	   {
         /* forced update logic */
          pVADState_flt->silence_cnt ++;
          if (pVADState_flt->silence_cnt >= UPDATE_CNT_THLD) //UPDATE_CNT_THLD=40
		  {
             flag = (F)1.0;   //flag = 1;
		  }
		  else
		  {
             flag = (F)0.75; //flag = 0.75;
		  }
	   }
	}

  // dramatic energy changes
    if (fdelta_E_s < (F)-255.0)		//if (fdelta_E_s < EN_DROP_THR)
    {
		flag = (F)0.75;			//flag = VAD_FLAG_P75;
    }
    //else if ((fdelta_E_s < EN_DROP_THR1) && (voicing <= VOICING_CHECK))
	else if ((fdelta_E_s < (F)-218.0) && (voicing <= VOICING_CHECK))
	{
       flag = (F)0.75;			//flag = VAD_FLAG_P75;
	}

    if (pVADState_flt->silence_cnt == pVADState_flt->last_silence_cnt)
	{
       pVADState_flt->burst_cnt = (short)(pVADState_flt->burst_cnt + 1);
	}
    else
	{
       pVADState_flt->burst_cnt = 0;
	}
    pVADState_flt->last_silence_cnt = pVADState_flt->silence_cnt;

    if (pVADState_flt->burst_cnt > BURST_CNT_THLD)
	{
       pVADState_flt->silence_cnt = 0;
	}

   /*--------------------------------------------------------------*/
   /* VAD hang-over for improve clean speech input @5/1/2003       */
   /*--------------------------------------------------------------*/
	flag = vad_smoothing_FLT (pVADState_flt, flag);

	if (Etot_dB > pVADState_flt->Etot_dB_max_flt)
	{
       pVADState_flt->Etot_dB_max_flt = Etot_dB;
	}

    IgnoreTHR = (F)62.5;//IgnoreTHR = 2000;

    /*if ((pVADState->Etot_dB_max_flt < 2500) &&
        (pVADState->vad_pattern_flag== VAD_IGNORE_PATTERN_STANDARD))*/
	if ((pVADState_flt->Etot_dB_max_flt < (F)78.125) &&
        (pVADState_flt->vad_pattern_flag== VAD_IGNORE_PATTERN_STANDARD))
	{
        IgnoreTHR = (F)31.25; //IgnoreTHR = 1000;
	}

   /*----------------------------------------------------------------*/
   /* update noise energies                                          */
   /*----------------------------------------------------------------*/
   /*===========================================================================*/
   /* The following code included inside #if VAD_PATTERN_IN...#else ...#endif   */
   /* is due to IGNORE_PATTERN feature of VAD and is configurable in release 4. */
   /* This availability is switchable by VAD_PATTERN_IN. In this case,          */
   /* VAD_IGNORE_PATTERN=1 and VAD_IGNORE_PATTERN_ENHANCE=1                     */
   /*                                                                           */
   /* By Chunyan @ 6/28/2004                                                    */
   /*===========================================================================*/
   /*---------------------------------------------------------------
      After 6/28/2004, the IGNORE_PATTERN is run-time configurable
      and
      VAD_IGNORE_PATTERN=1 and VAD_IGNORE_PATTERN_ENHANCE=1
    ----------------------------------------------------------------*/
    if (pVADState_flt->vad_pattern_flag != VAD_IGNORE_PATTERN_OFF)
	{
       updateFlag = 0;
       if (Etot_dB > IgnoreTHR) updateFlag = 1;
	}
    else  updateFlag = 1;

    if ((flag>(F)0.0) && (fno>=4) && updateFlag)
	{
       pVADState_flt->pattern_cnt = 0;
       salpha = (F)0.9; //salpha = ALPHA_N_Q15;
       stemp = (F)0.1;
       for (i=0; i<NC; i++)
	   {
            ltemp1 = pVADState_flt->fEn_flt[i] * salpha;
			stemp1 = stemp * cband1_div_flt[i];	
			ltemp2 = pVADState_flt->fEch_flt[i] * stemp1;
			ltemp2 += ltemp1;
			if (ltemp2 < (F)0.5) ltemp2 = (F)0.5;
            pVADState_flt->fEn_flt[i] = ltemp2; 
	   }

       pVADState_flt->active_cnt = 0;
	}

    if ((flag>0) && (fno>=4)  && (Etot_dB<=IgnoreTHR) &&
        (pVADState_flt->vad_pattern_flag == VAD_IGNORE_PATTERN_STANDARD))
	{
       pVADState_flt->pattern_cnt ++;

       if (pVADState_flt->pattern_cnt > VAD_PATTERN_CNT1)
	   {
         /*------------------------------------------------------------*/
         /* Update pVADState->fEn[i].                                  */
         /* pVADState->fEn[i] = pVADState->fEn[i]*alpha                */
         /*                   + pVADState->fEch[i]*((1-alpha)/c2-c1+1) */
         /*------------------------------------------------------------*/
          salpha = (F)0.225;	//salpha = ALPHA_N_Q15 >> 2;
          if (pVADState_flt->pattern_cnt > VAD_PATTERN_CNT3)
             salpha = (F)0.9;//salpha = ALPHA_N_Q15  ;
          else if (pVADState_flt->pattern_cnt > VAD_PATTERN_CNT2)
             salpha = (F)0.45;//salpha = ALPHA_N_Q15 >> 1 ;

          stemp = (F)1.0-salpha;
          for (i=0; i<NC; i++)
		  {
				ltemp1 = pVADState_flt->fEn_flt[i] * salpha; 	
				stemp1 = stemp * cband1_div_flt[i];
				ltemp2 = pVADState_flt->fEch_flt[i] * stemp1;
				//pVADState_flt->fEn_flt[i] = ltemp1 + ltemp2;
				ltemp2 += ltemp1;
				if (ltemp2 < (F)0.5) ltemp2 = (F)0.5;
				pVADState_flt->fEn_flt[i] = ltemp2; 
		  }

          pVADState_flt->active_cnt = 0 ;
	   }
	}

    if (fno < 1)
	{
       for (i =0; i < NC; i++)
	   {
           pVADState_flt->prev_Ech_flt[i] = pVADState_flt->fEch_flt[i] ;
	   }
	}

    if ((flag == (F)0.0)  && (fno >= 4))
	{
     //salpha  = TRACKING_ALPHA;  //= (Word16) (0.998 * 32768);
     //salpha1 = TRACKING_BETA;   //= (Word16) (0.05 * 32768);
     //salpha2 = TRACKING_GAMMA;  //= (Word16) (0.048 * 32768);

       pVADState_flt->active_cnt ++ ;

       if (pVADState_flt->active_cnt >= SW_MAX)
	   {
          pVADState_flt->active_cnt = 200 ;
	   }

       if (pVADState_flt->active_cnt >= 200)
	   {
			for ( i = 0; i < NC; i++ )
			{
				ltemp1 = pVADState_flt->fEch_flt[i] * cband1_div_flt[i];
				if (pVADState_flt->fEn_flt[i] >= ltemp1)		 	
				{
					pVADState_flt->fEn_flt[i] = ltemp1;
				}
				else
				{

					ltemp1 = ltemp1 * (F)0.049987;
					stemp1 = (F)0.047973 * cband1_div_flt[i];
					ltemp2 = pVADState_flt->prev_Ech_flt[i] * stemp1;
					ltemp1 -= ltemp2;
					ltemp2 = pVADState_flt->fEn_flt[i] * (F)0.997985;
					ltemp2 += ltemp1;
					if (ltemp2 < (F)0.5) ltemp2 = (F)0.5;
					pVADState_flt->fEn_flt[i] = ltemp2; 
			  }
		  }
	   } // end of if
	}

	for (i=0; i<NC; i++)
	{
		pVADState_flt->prev_Ech_flt[i] = pVADState_flt->fEch_flt[i];
	}

   /*-----------------------------------------*/
   /* Aded one more output for ALC @ 4/16/03  */
   /*-----------------------------------------*/
	for (i=0; i<NC; i++)
	{
		EdB[i] = fEdb[i];
	}

   /*-------------------------------------------------------------------*/
   /* Add one more output for VADPP @ 11/8/04                           */
   /* Use the same array as EdB. Just add an extra element to the array */
   /*-------------------------------------------------------------------*/
	EdB[NC] = fdelta_E;

	return (flag);
}



FLOAT vad_smoothing_FLT (VAD_STATE_FLT *pVADState_flt, FLOAT in_flag)
{
    FLOAT out_flag;

   /*-----------------------------------------------------------*/
   /*  FSM:                                                     */
   /*   vad_state: VAD_NON_ACTIVE, VAD_ACTIVE                   */
   /*   In_flag:  Detect active (=0), detect non-active (!=0)   */
   /*   Out_flag: Speech (=0), non-speech (!=0)                 */
   /*-----------------------------------------------------------*/
    if (pVADState_flt->vad_state == VAD_NON_ACTIVE )
	{
      /* non-active state */
       if ( (in_flag == (F)0.0) && (pVADState_flt->onset_cnt >= VAD_ONSET_CNT) )
	   {
         /* state hangover */
          out_flag = (F)0.0;
          pVADState_flt->vad_state = VAD_ACTIVE;
          pVADState_flt->offset_cnt = 0; // reset offset_cnt
	   }
	   else
	   {
         /* stay the same state */
          if (in_flag == (F)0.0)
		  {
             pVADState_flt->onset_cnt ++;
             out_flag = (F)0.75;	//out_flag = VAD_FLAG_P75;
		  }
		  else
		  {
             pVADState_flt->onset_cnt = 0;
             out_flag = in_flag;
		  }
	   }
	}
	else
	{
      /* active state */
       if ( (in_flag != (F)0.0) && (pVADState_flt->offset_cnt >= VAD_OFFSET_CNT ) )
	   {
         /* state hangover */
          out_flag = in_flag;
          pVADState_flt->vad_state = VAD_NON_ACTIVE;
          pVADState_flt->onset_cnt = 0; // reset onset_cnt
	   }
	   else
	   {
         /* stay the same state */
          if (in_flag != (F)0.0)
		  {
             pVADState_flt->offset_cnt ++;
             out_flag = (F)0.0;
		  }
		  else
		  {
             pVADState_flt->offset_cnt = 0;
             out_flag = in_flag;
		  }
	   }
	}

    return (out_flag);
}

/* construct a frame (256 samples) for 256-point forward RFFT */
void construct_FFTframe_FLT (FLOAT *new_data, FLOAT *curr_data,
                         FLOAT *old_data,
#if !ZERO_PADDING_IN_FFT
                         FLOAT *old_data1,
#endif
                         int lenRFFT)
{
    int   i;
    FLOAT  *ptr_dst, *ptr_src;

#if !ZERO_PADDING_IN_FFT
    int  N;
#endif
   /* initialization of pointers */


   /*---------------------------------------------------------------*/
   /* Construct FFT frame using data from current & previous frames */
   /*---------------------------------------------------------------*/
#if ZERO_PADDING_IN_FFT

   /* Increasing frequency resolution using zero-padding */

   /* 40 samples from previous frame */
    ptr_dst = new_data;
    ptr_src = old_data;
    for (i=0; i<LOOK_AHEAD; i++)
    {
        *ptr_dst++ = *ptr_src++;
    }

   /* 80 samples of current frame   */
    ptr_src = curr_data;
    for (i = 0; i<FRAME_SIZE; i++)
    {
        *ptr_dst++ = *ptr_src++;
    }

   /* zero padding of 136 samples   */
    for (i=0; i<(lenRFFT-FRAME_SIZE-LOOK_AHEAD); i++)
    {
        *ptr_dst++ = 0;
    }

   /* update history for next frame processing */
    ptr_dst = old_data;
    ptr_src = new_data + FRAME_SIZE;
    for (i = 0; i < LOOK_AHEAD; i++)
    {
        *ptr_dst++ = *ptr_src++;
    }

#else

  /* 136 samples from previous frame (older frame) */
    ptr_dst = new_data;
    ptr_src = old_data1;
    for (i=0; i<HIST_SIZE; i++)
    {
        *ptr_dst++ = *ptr_src++;
    }

   /* 40 samples  from previous frame (old frame)  */
    ptr_src = old_data;
    for (i=0; i<LOOK_AHEAD; i++)
    {
        *ptr_dst++ = *ptr_src++;
    }

   /* 80 samples  from current frame (current frame) */
    ptr_src = curr_data;
    for (i=0; i<FRAME_SIZE; i++)
    {
        *ptr_dst++ = *ptr_src++;
    }

   /* update history for next frame processing */
    ptr_dst = old_data1;
    ptr_src = new_data + FRAME_SIZE;
    for (i=0; i<HIST_SIZE; i++)
    {
        *ptr_dst++ = *ptr_src++;
    }

    ptr_dst = old_data;
    for (i=0; i<LOOK_AHEAD; i++)
    {
        *ptr_dst++ = *ptr_src++;
    }

   /*---------------------------------------------------------------*/
   /* Triangular Analysis Windowing prior to Forward RFFT           */
   /*    coefficient table : tri_win[i] = (i/128)*2^15              */
   /*---------------------------------------------------------------*/
    N = lenRFFT >> 1;
    for (i=0; i<N; i++)
    {
		new_data[i]   = tri_win_flt[i] * new_data[i];
		new_data[i+N] = tri_win_flt[N-1-i] * new_data[i+N];
    }
#endif

    return;
}

/* Compute power spectrum in frequency domain : fmag[0:128] */

void compute_power_spectrum_FLT (FLOAT buff[], FLOAT fmag[], int len)
{
   int i;

   fmag[0] = buff[0] * buff[0];
   fmag[len/2] = buff[1] * buff[1];
   for (i = 1; i < len/2; i++)
   {
	   fmag[i] = buff[2*i] * buff[2*i] + buff[2*i+1] * buff[2*i+1];
   }

   return;
}

