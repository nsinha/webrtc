/*****************************************************************/
/* DITECH NETWORKS INC.						                     */
/* Copyright. 2006. Ditech Networks Inc. All rights reserved.	 */
/*---------------------------------------------------------------*/
/* StarCore DSP (SC140/3400) C Code Module                       */
/*                                                               */
/* File Name       : syn_process.c                               */
/* Author          : Chunyan Li, JB Hong, M Godavarti            */
/* Date            : Aug/12/2006                                 */
/*---------------------------------------------------------------*/
/* Revision History:                                             */
/*   $Revision: 1.00 $ (NOV/15/2002) by Chunyan Li               */
/*              file created                                     */
/*   $Revision: 1.50 $ (AUG/12/2006) by JB HONG                  */
/*              Polished/refined codes for Freescale SC140.      */
/*   $Revision: 2.00 $ (MAY/21/2007) by JB HONG & M Godavarti    */
/*              Anysis/Synthesis windowing changed.              */
/*****************************************************************/
#include  "anr_vad.h"
#include "syn_process.h"
#include "anr_tab.h"
#include "r_fft.h"

/***************************************************************************
 *
 * FUNCTION NAME: SYN_init()
 *
 * PURPOSE:
 *
 *   Initialize state variables or memory of SYN_process module.
 *
 * INPUTS/OUTPUTS:
 *
 *   *pSYNState:(I/O) pointer of state variable structure of SYN_process
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/

void SYN_init_FLT (SYN_STATE_FLT *pSYNState)
{
  int	i;

  for (i=0; i < OLP_SIZE; i++)
  {
      pSYNState->OlpData[i] = (F)0.0;
  }

  return;
}

/************************************************************************
 *
 * FUNCTION NAME: SYN_process()
 *
 * PURPOSE:
 *
 *   Do dynamic block scaling and inverse FFT.
 *   Synthesize data in time domain using Overlap-and-Add mathod.
 *
 * INPUTS/OUTPUTS:
 *
 *   *old_data:(I/O) pointer of previous frame data (40 samples) in time domain
 *   *buff    :(I/O) pointer of data in frequency (FFT) and time domain (IFFT)
 *   Shift_in :(I)   dynamic scaling factor
 *   *out_data:(O)   pointer of synthesized output (80 samples) in time domain
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/
void SYN_process_FLT (SYN_STATE_FLT	*pSYNState, FLOAT *buff, FLOAT *out_data)
{

	  int  lenRFFT;
    lenRFFT = 256; //256-point RIFFT
    rfft_flt (buff, lenRFFT, -1);	

   /* overlap-add */
    syn_ovlp_add_FLT(pSYNState->OlpData, buff, out_data);
}

/***************************************************************************
 *
 * FUNCTION NAME: syn_ovlp_add()
 *
 * PURPOSE:
 *
 *   Synthesize data in time domain using Overlap-and-Add mathod.
 *
 * INPUTS/OUTPUTS:
 *
 *   *old_buff:(I/O) pointer of previous frame data (40 samples) in time domain
 *   *new_buff:(I)   pointer of data in time domain through IFFT
 *   *out_buff:(O)   pointer of synthesized output (80 samples) in time domain
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/
void syn_ovlp_add_FLT (FLOAT *old_buff, FLOAT *new_buff, FLOAT *out_buff)
{
    int	i;
	FLOAT  acc;


/* FOR 10ms operation */
   /*--------------------------------------------------------------------------*/
   /* Two levels of synthesis windowing :                                      */
   /*--------------------------------------------------------------------------*/
   /* ANALYSIS                                                                 */
   /*  (1) Half cycle of triangular window for Analysis:                       */
   /*       N=128=FFTLENGTH/2, i=0,1,...N-1                                    */
   /*       tri_win[i] = (i+1/N)*2^15;                                         */
   /*      Analysis windowing:                                                 */
   /*       new_data[i]   = new_data[i] *   tri_win[i];                        */
   /*       new_data[N+i] = new_data[N+i] * tri_win[N-1-i];                    */
   /*                                                                          */
   /* SYNTHESIS                                                                */
   /*  The last 80 samples of new_data[0:255] are the actual output            */
   /*      of current frame.                                                   */
   /*                                                                          */
   /*  (1) Undo analysis triangular windowing and                              */
   /*      apply a new trapezodal windowing of rising end.                     */
   /*      Add to the old data (the 40 samples of previous frame).             */
   /*       N = 128, i=0,1,2,..39                                              */
   /*       new_data[136+i] = 1/tri_win[119-i] * i/39 * new_data[136+i]        */
   /*       new_data[136+i] = new_data[136+i] * old_data[i]                    */
   /*  (2) Undo analysis triangular windowing at the flat region of the        */
   /*      trapezoidal window.                                                 */
   /*       N = 128, i=0,1,2,..39                                              */
   /*       new_data[136+40+i] = 1/tri_win[79-i] * new_data[136+40+i]      */
   /*  (3) Undo analysis triangular windowing and                              */
   /*      apply a new trapezodal windowing of falling                         */
   /*      for next frame processing.                                          */
   /*       N = 128, i=0,1,2...39                                              */
   /*       old_data[i] = 1/tri_win[39-i] * {1-i/39} * new_data[136+80+i]  */
   /*--------------------------------------------------------------------------*/

/* FOR 20ms operation */
   /*--------------------------------------------------------------------------*/
   /* Two levels of synthesis windowing :                                      */
   /*--------------------------------------------------------------------------*/
   /* ANALYSIS                                                                 */
   /*  (1) Half cycle of triangular window for Analysis:                       */
   /*       N=128=FFTLENGTH/2, i=0,1,2,... N-1                                 */
   /*       tri_win[i] = (i+1/N)*2^15;                                         */
   /*      Analysis windowing:                                                 */
   /*       new_data[i]   = new_data[i] *   tri_win[i];                        */
   /*       new_data[N+i] = new_data[N+i] * tri_win[N-1-i];                    */
   /*                                                                          */
   /* SYNTHESIS                                                                */
   /*  The last 160 samples of new_data[0:255] are the actual output           */
   /*      of current frame.                                                   */
   /*                                                                          */
   /*  (1) Undo analysis triangular windowing and                              */
   /*      apply a new trapezodal windowing of rising end.                     */
   /*      Add to the old data (the 40 samples of previous frame).             */
   /*       N = 128, i=0,1,2..39                                               */
   /*       new_data[56+i] = 1/tri_win[56+i] * i/39 * new_data[56+i]           */
   /*       new_data[56+i] = new_data[56+i] * old_data[i]                      */
   /*  (2) Undo analysis triangular windowing at the flat region of the        */
   /*      trapezoidal window.                                                 */
   /*      The 120 samples in this region is divided into two segments         */
   /*      32 and 88.First 32 samples have rising edge of the traingle window  */
   /*        and the last 88 have the falling edge of the triangle window      */
   /*       N = 128, i=0,1,2,..31                                              */
   /*       new_data[56+40+i] = 1/tri_win[56+40+i] * new_data[56+40+i]         */
   /*       i = 0,1,2,...87                                                    */
   /*       new_data[56+40+32+i] = 1/tri_win[127-i] *new_data[56+40+32+i]      */
   /*  (3) Undo analysis triangular windowing and                              */
   /*      apply a new trapezodal windowing of falling                         */
   /*      for next frame processing.                                          */
   /*       N = 128, i=0,..39                                                  */
   /*     old_data[i] = 1/tri_win[39-i] * {1-i/39} * new_data[56+160+i]        */
   /*--------------------------------------------------------------------------*/

    for (i=0; i<LOOK_AHEAD; i++)
    {
      acc = undo_plus_tri_win_flt[i] * new_buff[i+HIST_SIZE];
      new_buff[i+HIST_SIZE] = acc + old_buff[i];
    }

   /* undo the long triangular window for 40 samples */
    for (i=0; i<(FRAME_SIZE-OLP_SIZE); i++)
    {
      new_buff[i+HIST_SIZE+LOOK_AHEAD] = undo_win_flt[i] * new_buff[i+LOOK_AHEAD+HIST_SIZE];
    }

   /* Updating data buffer */
   /* update old buffer by undoing the tail end of the full triangular window */
   /* and applying the new triangular window                                  */
    for (i=0; i<OLP_SIZE; i++)
    {
		old_buff[i] = undo_plus_tri_win_ola_flt[i] * new_buff[i+FRAME_SIZE+HIST_SIZE];
    }

   /* output data for current frame */
    for (i=0; i<FRAME_SIZE; i++)
    {
        out_buff[i] = new_buff[i+HIST_SIZE];
    }

    return;
}


/***************************************************************************
 *
 * FUNCTION NAME: down_sample_spectrum()
 *
 * PURPOSE:
 *
 *   downsampling from 256-point to 128-point in frequency domain
 *
 * INPUTS/OUTPUTS:
 *
 *   *buff:(I/O) pointer of sample buffer (256==>128 samples) in freq. domain
 *   len  :(I)   index
 *
 * RETURN VALUE:
 *
 *   None
 *
 ************************************************************************/
void down_sample_spectrum_flt (FLOAT *buff, int len)
{
  int  i, cnt;
  FLOAT  *xp, *yp;

  cnt = len >> 1;
  yp = buff + 2;
  xp = buff + 4;

  for (i=1; i<cnt; i++)
  {
	  *yp++ = *xp++;
	  *yp++ = *xp++;

	  xp++; 
	  xp++;
  }

}


