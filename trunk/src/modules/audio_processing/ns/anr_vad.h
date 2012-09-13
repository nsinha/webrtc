/*==================================================================*/
/*								                                    */
/*      Adaptive Noise Reduction     ANSI-C Source Code				*/
/*																	*/
/*      Version 1.0													*/
/*      Copyright (C) 2003, Ditech Communications Corportation		*/
/*      All rights reserved.										*/
/*										                            */
/*																	*/
/*		Created by: Chunyan Li @ April 2003							*/
/*==================================================================*/

#ifndef _VAD_ANR_H
#define	_VAD_ANR_H

#include <math.h>

#include "anr_const.h"

/*	Switches for Enhanced ANR module */
/*************************************/
#define	HC_ANR_PROCESS		1		// Default=1. Noise Silencer available
#define	ANR_POST_PROCESSING	1		// Default=1. Spectral shaping available
#define	SEPARATE_SYN		1	// default=1. seperate synthesis process out
#define	ADD_DNC				1
#define	ANR_CNG_IN			1	// default=1. switch to add CNG config to ANR
#define	VAD_PATTERN_IN		1	// default=1. switch to add IGNORE_PATTERN config to VAD
								// VAD_IGNORE_PATTERN need to be 1.

#define	ANR_SNR_ADAPTIVE	1	// =1, ANR adaptive mode is available

#define	ANR_IMPROVE_JAN_2005	1	// ANR changes made in Jan. 2005
									// (1) Configurable SNR adatiove mode
									// (2) Configurable CNF noise floor level
									// (3) Add 6dB nosie reduction option

#define	VAD_CHANGE_FEB_2005		1	// VAD frame loss handeling have three mode configurable
									// vad_pattern_flag=0: no frame loss handeling
									//	= 1: standard equivalent to enhanced pattern ignore after release 4
									// = 0: high: original pattern handling, the same as release 2 & 3. 
									// For this change, VAD_PATTERN_IN=1, VAD_IGNORE_PATTERN_ENHANCE=1.

#define	ANR_IMPROVE_FEB_2005	1	// improve ANR convergence performance when network DTX 
									// and other network device adding level-reduced artificial noise. 

#define	ANR_LOW_SNR_MARCH_2005	1

/*	Switches for Enhanced IGNORE PATTERN detection */
/****************************************************/
#if VAD_PATTERN_IN
#define	VAD_IGNORE_PATTERN_ENHANCE	1	// After 6/28/2004. It has to be 1 if VAD_PATTERN_IN=1 which 
										// means when VAD is configuraed as IGNORE_PATTERN_ON, 
										// use the enhancement of ignore patter solution
										// =0 for release 3. 
#else
#define	VAD_IGNORE_PATTERN_ENHANCE	1	// =1 only when VAD_IGNORE_PATTERN=1
#endif										

/* Constant define for ANR_CNG feature */
/***************************************/
#if ANR_CNG_IN
#define	ANR_NOISE_LEVEL_RANGE	21
#endif

/* Constant define for VAD_PATTERN_IN feature */
/***************************************/
#if VAD_PATTERN_IN

#if VAD_CHANGE_FEB_2005
#define	VAD_IGNORE_PATTERN_STANDARD	1
#define	VAD_IGNORE_PATTERN_HIGH		2
#else
#define	VAD_IGNORE_PATTERN_ON	1
#endif

#define	VAD_IGNORE_PATTERN_OFF	0
#endif

/* constant define for ANR/VAD 
*******************************/
#define FFTLENGTH		256
#define	LOOK_AHEAD		40//110 // 80 
#define	OLP_SIZE		40//110 //80
#define	NUM_CRITICAL_BAND	18
//#define	NC 				NUM_CRITICAL_BAND
#define	NC 				17		// IMPROVED_VAD


/* constant define for ANR/VAD/DLC
*************************************/
#define	LOG10_OFFSET	626255212		// (log10(LW_MAX) in Q.26)
#define	LOG10_32_Q26	101008905	// log10(32) in Q.26
#define	TEN_S5_10		10240			// 10 in Q.10
#define	FFT_BY_TWO_SHIFT		7		

/*	Configuration */
/******************/

#define	ANR_NOISE_LEVEL_DEFAULT		0		// default

#if HC_ANR_PROCESS
#define HC_ANR_NOISE_LEVEL_DEFAULT	1
#endif

/* 
IMPROVED_VAD
*/
#define	VAD_MODE_NORMAL		0		// default
#define	VAD_MODE_AGGRESSIVE	1	


/* ========================= */
/*	Prototypes				 */
/* ========================= */	


#ifdef _MSC_VER
void anr_flt (
ANR_STATE_FLT *pANRState,
FLOAT  vad_flag,
FLOAT  *buff,		// processing buffer with FFT of input signal in complex
FLOAT  *fmag,		// input magnitudes
ANR_STATE_FLT *pANRStateHc,
int  hc_flag
);
#else
void anr_flt (
ANR_STATE_FLT *pANRState,
FLOAT  vad_flag,
FLOAT  *buff,		// processing buffer with FFT of input signal in complex
FLOAT  *fmag,		// input magnitudes
ANR_STATE_FLT *pANRStateHc
);
#endif

#if ANR_POST_PROCESSING

FLOAT	VAD_process_FLT ( 
VAD_STATE_FLT *pVADState_flt, 
FLOAT *in_data, 
FLOAT *buff, 
FLOAT *fmag, 
FLOAT *EdB
) ;

#else

FLOAT	VAD_process_FLT ( 
VAD_STATE_FLT *pVADState_flt, 
FLOAT *in_data, 
FLOAT *buff, 
FLOAT *fmag, 
FLOAT *EdB,
) ;

#endif


void	ANR_cfg_flt ( ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *p_ANRCfg ) ;
void	ANR_FLT_init ( ANR_STATE_FLT *pANRState) ;
void	ANR_getCfg_flt ( ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg )  ;

void VAD_cfg_FLT (VAD_STATE_FLT *pVADState,  VAD_CFG_FLT *pVADCfg);
void VAD_getcfg_FLT(VAD_STATE_FLT *pVADState, VAD_CFG_FLT *pVADCfg);
void VAD_init_FLT (VAD_STATE_FLT *pVADState);

#if HC_ANR_PROCESS
void	NS_cfg_flt ( ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg );
void	NS_getCfg_flt ( ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg );
#endif

#if ANR_POST_PROCESSING
void	SS_cfg_flt ( ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg );
void	SS_getCfg_flt ( ANR_STATE_FLT *pANRState,  ANR_CFG_FLT *pANRCfg );
#endif

#if ANR_SNR_ADAPTIVE
#else
#define	ANR_run	ANR_process
#endif


#define	VAD_run_FLT	VAD_process_FLT

#endif

