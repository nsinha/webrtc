/*****************************************************************/
/* DITECH NETWORKS INC.                                          */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* StarCore DSP (SC140/3400) C Code Module                       */
/*                                                               */
/* File Name       : anr_const.h                                 */
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
/*   $Revision: 2.00 $ (MAY/21/2007) by JB HONG & M Godavarti    */
/*              Compilation flag for a new analysis/synthesis    */
/*              windowings added and triggered                   */
/*              when ZERO_PADDING_IN_FFT == 0.                   */
/*****************************************************************/
#ifndef	_ANR_H
#define	_ANR_H
//#include "pcm_fg.h"
#include  "vqa_const.h"
#include "mpu_types.h"


#define ANR_20ms		1
/*=============================== */
/* Compilation Flag.              */
/*=============================== */
#define ZERO_PADDING_IN_FFT       0 /* zero-padding used or Not in FFT */
#define LOW_COMPLEXITY_SYNTHESIS  0  /* used in SYN_process() */
#define RFFT_DITECH               1

/*=============================== */
/* Basic types.                   */
/*=============================== */


#define LW_SIGN (long)0x80000000       /* sign bit */
#define LW_MIN  (long)0x80000000
#define LW_MAX  (long)0x7fffffff

#define SW_SIGN (short)0x8000          /* sign bit for Word16 type */
#define SW_MIN  (short)0x8000
#define SW_MAX  (short)0x7fff


/* =============================== */
/*	General Constant Definition	   */
/* =============================== */
#define	ONE_Q5			32
#define	ONE_Q6			64
#define	ONE_Q7			128
#define	ONE_Q8			256
#define ONE_Q9          512        /* (1 << 9)         */
#define	ONE_Q10			1024
#define ONE_Q11         2048       /* (1 << 11)        */
#define ONE_Q12         4096       /* (1 << 12)        */
#define ONE_Q13         8192       /* (1 << 13)        */
#define ONE_Q14         16384      /* (1 << 14)        */
#define ONE_Q15         32767      /* ((1 << 15) - 1)  */
#define ONE_Q19         524288L    /* (1 << 19)        */
#define TWO_Q3          16         /* 2 * (1 << 3)     */
#define TWO_Q19         1048576L   /* 2 * (1 << 19)    */
#define THREE_Q11       6144       /* 3 * (1 << 11)    */
#define FIFTY_Q0        50         /* 50 * (1 << 0)    */
#define FIFTY_Q7        6400       /* 50 * (1 << 7)    */
#define SIX_Q8          1536       /* 6 * (1 << 8)     */
#define X01_Q15         3277       /* 0.1 * (1 << 15)  */
#define X02_Q15         6554       /* 0.2 * (1 << 15)  */
#define X03_Q15         9830       /* 0.3 * (1 << 15)  */
#define X008_Q15        2621       /* 0.08 * (1 << 15) */
#define X015_Q15        4915       /* 0.15 * (1 << 15) */
#define X05_Q6          32         /* 0.5 * (1 << 6)   */
#define X05_Q14         8192       /* 0.5 * (1 << 14)  */
#define X05_Q15         16384      /* 0.5 * (1 << 15)  */
#define X07_Q15         22938      /* 0.7 * (1 << 15)  */
#define X08_Q10         819        /* 0.8 * (1 << 10)  */
#define X08_Q15         26214      /* 0.8 * (1 << 15)  */
#define X09_Q15         29491      /* 0.9 * (1 << 15)  */
#define X60_Q9          30720      /* 60 * (1 << 9)    */
#define MONE_Q15       -32768      /* (-(1 << 15))     */



/*
	To share with DLC => anr_vad.h
*/
/*
#define	LOG10_OFFSET	626255212		// (log10(LW_MAX) in Q.26)
#define	LOG10_32_Q26	101008905	// log10(32) in Q.26
#define	TEN_S5_10		10240			// 10 in Q.10
*/

#define	ONE_Q26			67108864
#define	TEN_S4_10		20480			// 10 in Q.11
//#define	LOG10_8_Q26		60605343	// log10(8) in Q.26
#define	LOG10_2_Q31		646456993
#define	LOG10_2_10		(F)3.01030	//10 * log10(2)	
#define	LOG10_1024_Q26	202017810	// log10(1024) in Q.26
#define	LOG10_1024  	(F)3.0103		// log10(1024) 
#define	Q10_SHIFT		10
#define	TWELVE_Q8		3072

#define	NATURE_E	    2.302585092994 //ln(10)
#define	PI			    3.141592654

/* ================================= */
/*	Definition of General Constants  */
/* ================================= */
//#define FRAME_SIZE          PCM_FRAME_SIZE /* 20 mSec */
#define	LOOK_AHEAD          40
#define OLP_SIZE            40 /* size of window overlap for overlap-and-add synthesis */

#define FFTLENGTH           256
#define	FFT_HEADROOM        2
#define	IFFT_HEADROOM       4

#define HIST_SIZE          (FFTLENGTH - FRAME_SIZE - OLP_SIZE) /* 56 = 256-160-40 */


#define	FFT_BY_TWO_SHIFT    7
//#define	LOG10_OFFSET        626255212 // (log10(LW_MAX) in Q.26)
#define	LOG10_1024_Q26      202017810 // log10(1024) in Q.26
//#define	LOG10_32_Q26        101008905 // log10(32) in Q.26
#define	LOG10_8_Q26         60605343  // log10(8) in Q.26
//#define	LOG10_2_Q31         646456993
#define	TEN_S5_10           10240     // 10 in Q.10
#define	TEN_S4_10           20480     // 10 in Q.11

#define	WINDOW_SIZE		(FRAME_SIZE+LOOK_AHEAD)

/* Auditory masking */
#define	ALPHA_MAX_Q12       24576	// 6 in Q12
#define	ALPHA_MIN_Q12       12288	// 3 in Q12
#define	ALPHA_MIN	        (F)3.0
#define	ALPHA_DIFF_Q12      12288	// ALPHA_MAX-ALPHA_MIN

#define	QUITE_THR_Q8        5120	// 20 in Q.8	20dBSPL
#define	BETA_MAX_Q15        3277		//0.1
#define	BETA_MAX            (F)0.1
#define	BETA_MIN_Q15        328		//0.01
#define	BETA_MIN	        (F)0.01
#define	BETA_DIFF_Q15       2949	// BETA_MAX-BETA_MIN

#define	ALPHA_BETA_SMOOTHING	4096	//0.125

/* gain function noise level */
#define	NOISE_LEVEL_Q15	3277	//0.1//0.025 //0.05 // -20dB //0.1 -17dB //0.1778 //-15dB
#define	NOISE_LEVEL_21dB_Q15	2200 //1000(-23dB)	// default
#define	NOISE_LEVEL_21dB		(F)0.067138 //1000(-23dB)	// default
#define	NOISE_LEVEL_18dB_Q15	3277 //2200// 1000 //3277
#define	NOISE_LEVEL_18dB		(F)0.100006 //2200// 1000 //3277
#define	NOISE_LEVEL_15dB_Q15	5000
#define	NOISE_LEVEL_15dB   		(F)0.152587
#define	NOISE_LEVEL_12dB_Q15	7000 //15000(6dB for Mahesh testing)
#define	NOISE_LEVEL_12dB		(F)0.213623 //15000(6dB for Mahesh testing)
#define	NOISE_LEVEL_9dB_Q15		9000
#define	NOISE_LEVEL_9dB			(F)0.274658

// adding 6dB configuration
#define	NOISE_LEVEL_6dB_Q15		16416
#define	NOISE_LEVEL_6dB			(F)0.500976

// configurable ANR_CNG feature. The noise floor is configurable
#define	ANR_CNG_THR_43dBm_Q5	(-46*32)	// -49dBov (need 3dB adjustment)
#define	ANR_CNG_THR_55dBm_Q5	(-61*32)	// -61dBov

/*	Add ANR_CNG feature */
#define	OFFSET_DBOV_Q5			(-77*32)
#define	OFFSET_DBOV				-77
#define	ANR_CNG_THR_Q5			(-55*32)	// -55dBov
#define	ANR_CFG_HIGH_DB_Q5		(21*32)
#define	ANR_CFG_HIGH_DB		    21
#define	ANR_CFG_LOW_DB_Q5		(0*32)
#define	ANR_CFG_LOW_DB		    0
#define	ANR_NOISE_LEVEL_RANGE	21

/* ============================================ */
/*	Spectral Smoothing							*/
/* ============================================ */
#if ANR_20ms
	#define	NOISE_EST_LAMDA_Q15	14336	// 0.4375
	#define	NOISE_EST_LAMDA 	(F)0.4375
#else
	#define	NOISE_EST_LAMDA_Q15	8192	// 0.25
	#define	NOISE_EST_LAMDA  	(F)0.25
#endif

#define	SSMOOTH_LAMDA_Q15	2048	//0.0625 //0.125  //0.25	//0.5 // smooth mag across frames
#define	SSMOOTH_LAMDA		(F)0.0625

#define	ANR_NOISE_LEVEL_DEFAULT		0 // default
#define HC_ANR_NOISE_LEVEL_DEFAULT	1

/* ================== */
/*		VAD			  */
/* ================== */
#define	ECH_MIN             0.0625
#define	E_INIT              2	//16 scaled by 8
#define	UPDATE_CNT_THLD     40 //20 //50
#define	BURST_CNT_THLD      6
#define	UPDATE_THLD	        38 //35 //37
#define	NC                  17
#define	NUM_CRITICAL_BAND	18


#define	E_H_Q5           2720	// 85
#define	ALPHA_H_Q15      32440	// 0.99
#define	ALPHA_L_Q15      16383	//0.5
#define	ALPHA_H_Q10      1014	//1023	0.99
#define	ALPHA_L_Q10      512    //511	0.5
#define	ALPHA_SLOPE      401    // (ALPHA_H-ALPHA_L)/(E_H-E_L) in Q.15. where E_L=45, E_H=85
#define	PURE_TONE_V_THR	 845
#define	PURE_TONE_EN_THR 17
#define	ALPHA_H1_Q15     31785	//32112	// 0.97
#define	ALPHA_H2_Q15     31785	// 0.97
#define	E_H1_Q5          3200	// 100


#define	NOISE_FLOOR_DB_Q5  16
#define	DEV_THLD_Q5	       992   // 31 in Q5 (<<5)
#define	VOICING_COEF       2731	 // 1/(ONE_Q5*0.375) in Q15
#define	ALPHA_CH_Q15       14746 //0.45
#define	ALPHA_N_Q15	       29491 //0.9
#define	ALPHA_N_Q15_BY_4   7372  //0.225 : ALPHA_N_Q15/4
#define	ALPHA_N_Q15_BY_2   14745 //0.45  : ALPHA_N_Q15/2


#define	ALPHA_SHORT_CH_Q15	3277 // 0.1
#define	EN_DROP_THR			-8160
#define	EN_RISE_THR			8160
#define	VOICING_CHECK		45
#define	EN_DROP_THR1		-7000
#define	INIT_ALPHA_Q15		0x77ff	// 15/16

#define	VAD_MODE_NORMAL             0 // default
#define	VAD_MODE_AGGRESSIVE         1
#define	VAD_IGNORE_PATTERN_OFF      0
#define	VAD_IGNORE_PATTERN_STANDARD	1
#define	VAD_IGNORE_PATTERN_HIGH		2

#define	TRACKING_ALPHA		32702
#define	TRACKING_BETA		1638
#define	TRACKING_GAMMA		1572

#define	VAD_FLAG_1		SW_MAX
#define	VAD_FLAG_P75	24576	//0.75

// added VAD hang-over for improve clean speech input @5/1/2003
#define	VAD_NON_ACTIVE		0
#define	VAD_ACTIVE			1
#define	VAD_ONSET_CNT		0		// already taken cared of by BURST_CNT_THLD
#define	VAD_OFFSET_CNT		5		//10
#define	VAD_PATTERN_CNT1	30
#define	VAD_PATTERN_CNT2	60
#define	VAD_PATTERN_CNT3	100

#define VAD_SPEECH_FLT			(F)0.0
#define VAD_TRANSITION_FLT		(F)0.75
#define VAD_NON_SPEECH_FLT		(F)1.0

#define VED_SND_DIR				0
#define VED_RCV_DIR				1

/*---------------------------------------------------------------------------*/
/* Sructure Definition for VAD/ANR/SYS Processes.                            */
/*---------------------------------------------------------------------------*/

typedef	struct
{
    int	vad_op_mode ;
    int	vad_pattern_flag ;	// =1, ignore pattern mode on to ignore frame loss

} VAD_CFG_FLT ;


typedef	struct
{
    int noise_level;
    int	hc_flag;             // high-complexity ANR flag
    int	ss_flag;             // spectral shaping flag
    int	anc_cng_flag;        // ANR internal CNG flag
    int	ns_cng_flag;
    int	anc_adaptive_mode;   // switch for adaptive mode
    //Word16	anc_alignment_dummy; // dummy word for make ANR_CNG size odd

} ANR_CFG_FLT;


typedef	struct
{
	FLOAT	fEch_flt[NC];	
    FLOAT	fEch_s_flt[NC];    
	FLOAT	fEn_flt[NC];        
	FLOAT	prev_Ech_flt[NC];	//IMPROVED_VAD
	FLOAT  Data_flt[LOOK_AHEAD];
#if !ZERO_PADDING_IN_FFT
	FLOAT  Data1_flt[HIST_SIZE]; //136 = HIST_SIZE = FFTLENGTH-LOOK_AHEAD-FRAME_SIZE
#endif

    FLOAT  favgEdb_flt[NC];	
	FLOAT  Etot_dB_max_flt;
	
	int		FrCnt;
    int  silence_cnt;
    int  last_silence_cnt;
    int  burst_cnt;
    int  vad_state;
    int  onset_cnt;
    int  offset_cnt;
    int  active_cnt;   // IMPROVED_VAD
    int  vad_op_mode;  // IMPROVED_VAD
    int  pattern_cnt;
    int  vad_pattern_flag;

} VAD_STATE_FLT;


typedef	struct
{
    ANR_CFG_FLT	anr_cfg;

    FLOAT	fest_noise[FFTLENGTH/2+1];	//8,23
    FLOAT	sigEn32;
    FLOAT	noiseEn32;
    FLOAT	realNoiseEn32;

    FLOAT	spec_gain[FFTLENGTH/2+1];	//Q.15
    FLOAT	fall_alpha[FFTLENGTH/2+1];
    FLOAT	fall_beta[FFTLENGTH/2+1];	//Q.15
    FLOAT	band_noise_level[NUM_CRITICAL_BAND];
    FLOAT	sigEnDB;
    FLOAT	noise_level;
	FLOAT	max_snr;
    FLOAT	avgNoiseEdB[NUM_CRITICAL_BAND];
    FLOAT	avgDiff;
    FLOAT	NoiseState;

    int		bypass_flag;
	int		active_cnt;
    int		first_speech;
    int		cng_flag;
    int		sigStartCnt;
    int		noiseStartCnt;
    int		CntTnToAn;
    int		CntAnToTn;

} ANR_STATE_FLT;

typedef	struct
{
    FLOAT  OlpData[OLP_SIZE];

} SYN_STATE_FLT;

#endif
