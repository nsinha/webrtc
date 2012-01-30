/*********************************************************************
** File Name:
**   "fft.h"
**   Copyright (c) 1998 Nokia Research Center, Tampere, Finland
**   Nokia Confidential
**
** Function:
**   This is a header file for "fft.c"
**
** Note:
**   Test environment : CPU - Pentium II (266MHz)
**                      OS  - Windows 95 % Windows NT 4.0
**                      Compiler - Microsoft Visual C++ ver.5.0
**                                 with MSV C++ Service Pack 3.0
** Author:
**   JB Hong, Speech and Audio Systems Laboratory
**
** History:
**   Oct.01.1998/JB Hong   File crelated!
**   Feb.15.1999/JB Hong   Tested against Intel's FFT
*********************************************************************/
#ifndef _RFFT_FX_H_
#define _RFFT_FX_H_


#define Q14_COSxW      0
#define LOW_CYCLE_MODE 1

#define LEN_RFFT_256   256
#define LEN_RFFT_128   128

#define FORWARD        1
#define INVERSE       -1

#define	FFT_HEADROOM	2
#define	IFFT_HEADROOM	4


void bitrev_flt (float *src, float *dst, int *ADDR_BITREV_FFT, int lenCFFT);
void cfft_flt (float *data_i,float *data, int lenCFFT, int isign);
void rfft_flt (float *fft_buf, int lenRFFT, int isign);

#endif
