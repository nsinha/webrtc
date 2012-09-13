/*****************************************************************/
/* DITECH NETWORKS INC.                                          */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* C Code Module for StarCore DSP (SC140/3400) & TMS320C54x      */
/*                                                               */
/* File Name       : anr_auditory.h                              */
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
/*****************************************************************/
#ifndef	_ANR_AUDITORY_H
#define	_ANR_AUDITORY_H

Word32 convolve_32by16 (Word32 *x, const Word16 *y, Word16 size);
Word32 convolve_flt (FLOAT *x, const FLOAT *y, int size);

void fix_compute_mask_threshold (Word32 *fmag, Word16 shift, Word16 *fth);
void fix_compute_mask_threshold_flt (FLOAT *fmag, FLOAT *fth);

void fix_get_alpha_beta (Word16 *fThr, Word16 *falpha, Word16 *fbeta);
void fix_get_alpha_beta_flt (FLOAT *fThr, FLOAT *falpha, FLOAT *fbeta);

void fix_post_gain (Word16 *fthr, Word32 *fnoise, Word16 *in_out, Word16 *noise_level,
                    Word16 ss_flag, Word16 *v_vector, Word16 *active_cnt, Word16 flag,
                    Word16 *first_speech, Word16 cng_flag);

void fix_post_gain_flt (FLOAT *fthr, FLOAT *fnoise, FLOAT *in_out, FLOAT *noise_level,
                    int ss_flag, int *active_cnt, FLOAT flag,
                    int *first_speech, int cng_flag);

#endif
