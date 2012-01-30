/*****************************************************************/
/* DITECH NETWORKS INC.                                          */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* StarCore DSP (SC140/3400) C Code Module                       */
/*                                                               */
/* File Name       : anr_tab.h                                   */
/*                                                               */
/* PURPOSE :                                                     */
/*                                                               */
/*                                                               */
/* Author          : Chunyan Li, JB Hong, Mahesh Godavarti       */
/* Date            : Aug/12/2006                                 */
/*---------------------------------------------------------------*/
/*   $Revision: 1.00 $ (SEP/16/2002) by Chunyan Li               */
/*              file created                                     */
/*   $Revision: 1.50 $ (AUG/12/2006) by JB HONG                  */
/*              Polished/refined codes for Freescale SC140.      */
/*   $Revision: 2.00 $ (MAY/21/2007) by JB HONG & M Godavarti    */
/*              Anysis/Synthesis windowing changed.              */
/*****************************************************************/
#ifndef _ANR_TAB_H
#define	_ANR_TAB_H

#if ZERO_PADDING_IN_FFT
extern const FLOAT  tri_win_flt[OLP_SIZE+1];
#else
extern const FLOAT tri_win_flt[FFTLENGTH/2];

extern const FLOAT  undo_plus_tri_win_flt[LOOK_AHEAD];
extern const FLOAT undo_win_flt[FRAME_SIZE-OLP_SIZE];
extern const FLOAT undo_plus_tri_win_ola_flt[OLP_SIZE];
#endif

extern const int  anr_max_snr_tab[4];
extern const FLOAT   anr_cng_thr_flt_tab[4];
extern const FLOAT   anr_Tbl_noise_level_flt[ANR_NOISE_LEVEL_RANGE+1];

extern const FLOAT  anr_cfg_tab_flt[6];

#if 1
extern const int  cband1[2*NC];
extern const FLOAT	cband1_log_offset_flt[NC];
extern const FLOAT  cband1_div_flt[NC];
#endif

extern const int  cband[2*NUM_CRITICAL_BAND];
extern const Word32	 cband_log_offset[NUM_CRITICAL_BAND];
extern const FLOAT  cband_div_flt[NUM_CRITICAL_BAND];


extern int Roffset[NUM_CRITICAL_BAND];
extern const FLOAT   Tnorm_flt[NUM_CRITICAL_BAND];

extern int v_array[90];

#if 0
extern const Word16  Table_SF[2*NUM_CRITICAL_BAND-1];
#else
extern const FLOAT  Table_SF_flt[8];
#endif

#endif
