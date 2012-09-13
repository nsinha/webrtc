/*********************************************************************
** File Name:
**   "fft_fx.c"
**   Copyright (c) 2006 Ditech Networks Inc.
**
** Function:
**   This file contains utility functions of FFT/IFFT for real/complex
**   input sequence.
**
** Note:
**   Test environment : CPU - Pentium
**                      OS  - Windows XP
**                      Compiler - Microsoft Visual C++ ver.6.0
**
** Author:
**   JB Hong
**
** History:
**   Oct.01.1998/JB Hong   File crelated!
**   Feb.15.1999/JB Hong   Tested against Intel's FFT
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include  "anr_vad.h"
#include "r_fft.h"

FLOAT cfft_buf_flt[256];
/*------------------------------------------------------------------*/
/* Tables for 256-point Real FFT (128-point Complex FFT)            */
/*------------------------------------------------------------------*/

/*-- bit-reversal of address multiplied by 2 for 128-point FFT --*/
int ADDR_BITREV_256[LEN_RFFT_256>>1] = {
 0, 128, 64, 192, 32, 160,  96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
 8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
 4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
 2, 130, 66, 194, 34, 162,  98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
 6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246,
14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254
};

/*-- sine table of a 128-point full cycle from 0 to 2*PI --------------*/
FLOAT SINW_256_flt[LEN_RFFT_256>>1] = {
 (F) 0.0000000000, (F) 0.0490676761, (F) 0.0980171412, (F) 0.1467304677,
 (F) 0.1950903237, (F) 0.2429801822, (F) 0.2902846634, (F) 0.3368898630,
 (F) 0.3826834261, (F) 0.4275550842, (F) 0.4713967443, (F) 0.5141027570,
 (F) 0.5555702448, (F) 0.5956993103, (F) 0.6343932748, (F) 0.6715589762,
 (F) 0.7071067691, (F) 0.7409511209, (F) 0.7730104327, (F) 0.8032075167,
 (F) 0.8314695954, (F) 0.8577286005, (F) 0.8819212914, (F) 0.9039893150,
 (F) 0.9238795042, (F) 0.9415440559, (F) 0.9569403529, (F) 0.9700312614,
 (F) 0.9807852507, (F) 0.9891765118, (F) 0.9951847196, (F) 0.9987954497,
 (F) 1.0000000000, (F) 0.9987954497, (F) 0.9951847196, (F) 0.9891765118,
 (F) 0.9807852507, (F) 0.9700312614, (F) 0.9569403529, (F) 0.9415440559,
 (F) 0.9238795042, (F) 0.9039893150, (F) 0.8819212914, (F) 0.8577286005,
 (F) 0.8314695954, (F) 0.8032075167, (F) 0.7730104327, (F) 0.7409511209,
 (F) 0.7071067691, (F) 0.6715589762, (F) 0.6343932748, (F) 0.5956993103,
 (F) 0.5555702448, (F) 0.5141027570, (F) 0.4713967443, (F) 0.4275550842,
 (F) 0.3826834261, (F) 0.3368898630, (F) 0.2902846634, (F) 0.2429801822,
 (F) 0.1950903237, (F) 0.1467304677, (F) 0.0980171412, (F) 0.0490676761,
 (F) 0.0000000000, (F)-0.0490676761, (F)-0.0980171412, (F)-0.1467304677,
 (F)-0.1950903237, (F)-0.2429801822, (F)-0.2902846634, (F)-0.3368898630,
 (F)-0.3826834261, (F)-0.4275550842, (F)-0.4713967443, (F)-0.5141027570,
 (F)-0.5555702448, (F)-0.5956993103, (F)-0.6343932748, (F)-0.6715589762,
 (F)-0.7071067691, (F)-0.7409511209, (F)-0.7730104327, (F)-0.8032075167,
 (F)-0.8314695954, (F)-0.8577286005, (F)-0.8819212914, (F)-0.9039893150,
 (F)-0.9238795042, (F)-0.9415440559, (F)-0.9569403529, (F)-0.9700312614,
 (F)-0.9807852507, (F)-0.9891765118, (F)-0.9951847196, (F)-0.9987954497,
 (F)-1.0000000000, (F)-0.9987954497, (F)-0.9951847196, (F)-0.9891765118,
 (F)-0.9807852507, (F)-0.9700312614, (F)-0.9569403529, (F)-0.9415440559,
 (F)-0.9238795042, (F)-0.9039893150, (F)-0.8819212914, (F)-0.8577286005,
 (F)-0.8314695954, (F)-0.8032075167, (F)-0.7730104327, (F)-0.7409511209,
 (F)-0.7071067691, (F)-0.6715589762, (F)-0.6343932748, (F)-0.5956993103,
 (F)-0.5555702448, (F)-0.5141027570, (F)-0.4713967443, (F)-0.4275550842,
 (F)-0.3826834261, (F)-0.3368898630, (F)-0.2902846634, (F)-0.2429801822,
 (F)-0.1950903237, (F)-0.1467304677, (F)-0.0980171412, (F)-0.0490676761
};

/*-- cosine table of a quarter part for a 256-point full cycle (0 to 2*PI)--*/
FLOAT COSxW_256_flt[(LEN_RFFT_256>>2)-1] = {
 /*(F)1.0000000000,*/
 (F)0.9996988177, (F)0.9987954497, (F)0.9972904325, (F)0.9951847196,
 (F)0.9924795628, (F)0.9891765118, (F)0.9852776527, (F)0.9807852507,
 (F)0.9757021070, (F)0.9700312614, (F)0.9637760520, (F)0.9569403529,
 (F)0.9495281577, (F)0.9415440559, (F)0.9329928160, (F)0.9238795042,
 (F)0.9142097831, (F)0.9039893150, (F)0.8932242990, (F)0.8819212914,
 (F)0.8700869679, (F)0.8577286005, (F)0.8448535800, (F)0.8314695954,
 (F)0.8175848126, (F)0.8032075167, (F)0.7883464098, (F)0.7730104327,
 (F)0.7572088242, (F)0.7409511209, (F)0.7242470980, (F)0.7071067691,
 (F)0.6895405650, (F)0.6715589762, (F)0.6531728506, (F)0.6343932748,
 (F)0.6152315736, (F)0.5956993103, (F)0.5758081675, (F)0.5555702448,
 (F)0.5349976420, (F)0.5141027570, (F)0.4928981960, (F)0.4713967443,
 (F)0.4496113360, (F)0.4275550842, (F)0.4052413106, (F)0.3826834261,
 (F)0.3598950505, (F)0.3368898630, (F)0.3136817515, (F)0.2902846634,
 (F)0.2667127550, (F)0.2429801822, (F)0.2191012353, (F)0.1950903237,
 (F)0.1709618866, (F)0.1467304677, (F)0.1224106774, (F)0.0980171412,
 (F)0.0735645667, (F)0.0490676761, (F)0.0245412290
 /*(F)0.0000000000*/
};

/*------------------------------------------------------------------*/
/* Tables for 128-point Real FFT (64-point Complex FFT)             */
/*------------------------------------------------------------------*/
/*-- bit-reversal of address multiplied by 2 for 64-point FFT --*/
int ADDR_BITREV_128[LEN_RFFT_128>>1] =
{
 0,64,32,96, 16,80,48,112,8, 72,40,104,24,88,56,120,
 4,68,36,100,20,84,52,116,12,76,44,108,28,92,60,124,
 2,66,34,98, 18,82,50,114,10,74,42,106,26,90,58,122,
 6,70,38,102,22,86,54,118,14,78,46,110,30,94,62,126
};

/*-- sine table of a 64-point full cycle from 0 to 2*PI --------------*/
float SINW_128_flt[LEN_RFFT_128>>1] =
{
 (F)0.0000000000,(F)0.0980171412,(F)0.1950903237,(F)0.2902846634,
 (F)0.3826834261,(F)0.4713967443,(F)0.5555702448,(F)0.6343932748,
 (F)0.7071067691,(F)0.7730104327,(F)0.8314695954,(F)0.8819212914,
 (F)0.9238795042,(F)0.9569403529,(F)0.9807852507,(F)0.9951847196,
 (F)1.0000000000,(F)0.9951847196,(F)0.9807852507,(F)0.9569403529,
 (F)0.9238795042,(F)0.8819212914,(F)0.8314695954,(F)0.7730104327,
 (F)0.7071067691,(F)0.6343932748,(F)0.5555702448,(F)0.4713967443,
 (F)0.3826834261,(F)0.2902846634,(F)0.1950903237,(F)0.0980171412,
 (F)0.0000000000,(F)-0.0980171412,(F)-0.1950903237,(F)-0.2902846634,
 (F)-0.3826834261,(F)-0.4713967443,(F)-0.5555702448,(F)-0.6343932748,
 (F)-0.7071067691,(F)-0.7730104327,(F)-0.8314695954,(F)-0.8819212914,
 (F)-0.9238795042,(F)-0.9569403529,(F)-0.9807852507,(F)-0.9951847196,
 (F)-1.0000000000,(F)-0.9951847196,(F)-0.9807852507,(F)-0.9569403529,
 (F)-0.9238795042,(F)-0.8819212914,(F)-0.8314695954,(F)-0.7730104327,
 (F)-0.7071067691,(F)-0.6343932748,(F)-0.5555702448,(F)-0.4713967443,
 (F)-0.3826834261,(F)-0.2902846634,(F)-0.1950903237,(F)-0.0980171412,
};

/*-- cosine table of a quarter part for a 128-point full cycle (0 to 2*PI)--*/
float COSxW_128_flt[(LEN_RFFT_128>>2)-1] =
{
  /*(F)1.0000000000,*/
 (F)0.9987954497,(F)0.9951847196,(F)0.9891765118,(F)0.9807852507,
 (F)0.9700312614,(F)0.9569403529,(F)0.9415440559,(F)0.9238795042,
 (F)0.9039893150,(F)0.8819212914,(F)0.8577286005,(F)0.8314695954,
 (F)0.8032075167,(F)0.7730104327,(F)0.7409511209,(F)0.7071067691,
 (F)0.6715589762,(F)0.6343932748,(F)0.5956993103,(F)0.5555702448,
 (F)0.5141027570,(F)0.4713967443,(F)0.4275550842,(F)0.3826834261,
 (F)0.3368898630,(F)0.2902846634,(F)0.2429801822,(F)0.1950903237,
 (F)0.1467304677,(F)0.0980171412,(F)0.0490676761
 /*(F)0.0000000000*/
};

/*----------------------------------------------------------------------------
* Function Name: bitrev_flt()
*   Copyright (c) 2006 Ditech Networks Inc.
*
* Function:
*   This function implements packing & bit-reversal of real-valued input
*   sequence, g(n) of 2N points for decomposition-in-time FFT.
*       g(n)  : real-valued sequence of 2N points, data[].
*       x1(n) = g(2n)         , n=0,1,2,3,..,N-1.
*       x2(n) = g(2n)+1       , n=0,1,2,3,..,N-1.
*       x(n)  = x1(n)+j*x2(n) , n=0,1,2,3,..,N-1.
*
* Note:
*   Computational efficiency is mainly taken into consideration
*   by sacrificing ROM-table size.
*
* Parameters:
*   *src  : real  input sequence, g(n) of 2N-points
*   *dst  : empty buffer for packed & bit-reversed sequence for complex FFT
*   ADDR_BITREV_FFT[] : bit-reversed address table for N-point complex FFT
*                       (read-only static variable)
* Outputs:
*   *dst  : packed & bit-reversed sequence for N-point complex FFT
*
* Returns:
*   None
*
* Local functions:
*   None
*
* History:
*    Oct.01.1998/JB Hong   File crelated!
*---------------------------------------------------------------------------*/
void bitrev_flt (float *src, float *dst, int *ADDR_BITREV, int lenCFFT)
{
 int   i;
 float *ptr;
 
 ptr = dst;
 for (i=0; i<lenCFFT; i+=2)
 {
     *ptr++ = src[ADDR_BITREV[i]];
     *ptr++ = src[ADDR_BITREV[i]+1];

     *ptr++ = src[ADDR_BITREV[i+1]];
     *ptr++ = src[ADDR_BITREV[i+1]+1];
 }

 return;
}

/*--------------------------------------------------------------------
* Function Name: rfft_flt()
*   Copyright (c) 2006 Ditech Networks Inc.
*
* Function:
*   This function implements forward FFT
*   for real input sequence of 2N-points.
*
* Note:
*   1) Computational efficiency is mainly taken into consideration
*      by sacrificing ROM-table size.
*
*   2) Currently, this function works only for 256 and 128-point
*      real FFT, but this can be extended to any size of FFT by
*      adding sine/cosine tables and modifying codes initializing
*      pointers of sine/cosine tables.
*
*   3) In the cfft_flt function, the FFT values are divided by
*      2 after each stage of computation thus dividing the
*      final FFT values by 128.  No multiplying factor is used
*      for the IFFT.  This is somewhat different from the usual
*      definition of FFT where the factor 1/N, i.e., 1/128, is
*      used for the IFFT and not the FFT.  No factor is used in
*      the rfft_flt function.
*
*   4) Much of the code for the FFT and IFFT parts in rfft_fx
*      and cfft_flt functions are similar and can be combined.
*      They are, however, kept separate here to save cycles
*      (execution speed).
*
* Parameters:
*   *fft_buf: (1) 2N-point(lenRFFT) real input sequence in natural order
*                 for Forwad FFT
*             (2) N-point(lenCFFT) complex input sequence in natural order
*                 for Inverse FFT
*   lenRFFT : length of Real FFT = 2N-point (double size of complex FFT)
*   isign   : FFT mode (FORWARD:1, INVERSE:-1)
*   COSxW[] : a quarter part(0~0.5pi)of a 2N-point(lenRFFT)
*             full period cosine (read-only static variable)
* Outputs:
*   *fft_buf: (1) For forward FFT,
*                 2N-point forward FFT output with complex values in place.
*                 (Only a half cycle from 0 to PI, is saved due to
*                  Hermitian Symmetry!)
*                 For Intel format
*                   fft_buf[0:2N+1] =
*                   [Re(0),Im(0),Re(1),Im(1),Re(2),Im(2),...,Re(N),Im(N)]
*                 For Other format
*                   fft_buf[0:2N-1] =
*                   [Re(0),Re(N),Re(1),Im(1),Re(2),Im(2),...,Re(N-1),Im(N-1)]
*                   Note: Im(0) = Im(N) = 0
*             (2) For inverse FFT,
*                 2N-point inverse FFT output with real values in place.
*                 fft_buf[0:2N-1] is valid & available regardless of formats.
*
* Returns:
*   None
*
* Local functions:
*   cfft_flt()
*
* History:
*    Oct.01.1998/JB Hong   File crelated!
*--------------------------------------------------------------------*/

void rfft_flt (float *fft_buf, int lenRFFT, int isign)
{

  int     i, j, lenCFFT;
  float   c1, c2;
  float   real_add, real_sub;
  float   imag_add, imag_sub;
  float   *p_sin, *p_cos;
  float   *cfft_buf;


 /*-- Dynamic memory allocation ------------------------------------------*/
/*  len = lenRFFT + 2;

  if ((cfft_buf = (float *) malloc (sizeof(float) * len)) == NULL)
  {
     printf (" #Error in memory alloc : cfft_buf!.\n");
     exit (EXIT_FAILURE);
  }*/
  cfft_buf = cfft_buf_flt;

 /*------------------------------------------------------------------------*/

  lenCFFT = (short)(lenRFFT >> 1);

 /*========================================================================*/
 /* Initialization of Sine & Cosine Table pointers according to FFT size   */
 /*------------------------------------------------------------------------*/
 /* Currently, this function works only for 256 or 128-point Real FFT.     */
 /* However, this can be extended to any size of FFT by modifying the      */
 /* pointer initialization of the following table.                         */
 /*========================================================================*/
  p_sin = &COSxW_256_flt[(lenRFFT>>2)-2];
  p_cos = &COSxW_256_flt[0];
  if (lenRFFT == LEN_RFFT_128)
  {
     p_sin = &COSxW_128_flt[(lenRFFT>>2)-2];
     p_cos = &COSxW_128_flt[0];
  }

 /*========================================================================*/
 /*  Forward Real FFT                                                      */
 /*========================================================================*/
  if (isign == FORWARD)
  {

    /*
     |---------------------------------------------------------------------|
     |< Stage 1 >                                                          |
     |Packing & Bit-Reversal of 256-point real input sequence, g(n) to     |
     | complex 128-point input signal, x(n) for FFT.                       |
     |   g(n)  : real-valued input sequence of 2N points.                  |
     |   x1(n) = g(2n)         , n=0,1,2,3,..,N-1.                         |
     |   x2(n) = g(2n+1)       , n=0,1,2,3,..,N-1.                         |
     |   x(n)  = x1(n)+j*x2(n) , n=0,1,2,3,..,N-1, in bit-reversed order   |
     |                                                                     |
     |cfft_buf[2n]   = bit-reversal for fft_buf[2n]   = x1(n),n=0,1,..,N-1,|
     |cfft_buf[2n+1] = bit-reversal for fft_buf[2n+1] = x2(n),n=0,1,..,N-1.|
     |---------------------------------------------------------------------|
     |< Stage 2 >                                                          |
     |Compute N(128)-point radix-2 DIT(Decomsition-In-Time) complex FFT.   |
     |Using the property that DFT is linear operation,                     |
     |                              DFT                                    |
     |  x(n) = x1(n) + j*x2(n)  <---------> X(k) = X1(k) + j*X2(k),        |
     |   x1(n);even index real value <--->  X1(k);complex FFT value,       |
     |   x2(n);odd  index real value <--->  X2(k);complex FFT value,       |
     |         N-1                                                         |
     |  X(k) = SUM [x(n)*{ cos(2*pi*k*n/128)-j*sin(2*pi*k*n/N) }]          |
     |         n=0                                                         |
     |       = X1(k) + j*X2(k) ;X1(k),X2(k) are complex value              |
     |       = Xr(k) + j*Xi(k) ;Xr(k),Xi(k) are real & imaginary parts     |
     |                                                          of X(k)    |
     |---------------------------------------------------------------------|
    */
     cfft_flt (fft_buf, cfft_buf, lenCFFT, isign);

    /*
     |---------------------------------------------------------------------|
     |< Stage 3 >                                                          |
     |Express the 2N-point DFT, G(k) in terms of the two N-point DFT's,    |
     | X1(k) and X2(k).                                                    |
     |Compute/Recover G(k), FFT output of original 2N-point real input     |
     |                                                sequence of g(n).    |
     | # x(n) = x1(n) + j*x2(n) ; x(n)is complex value                     |
     | # x1(n) =  0.5*  [x(n)+x'(n)] ; x(n),x'(n) are conjugate pair       |
     | # x2(n) = -0.5*j*[x(n)-x'(n)] ; x(n),x'(n) are conjugate pair       |
     | -> Using Linear property of DFT                                     |
     | # X1(k) =  0.5*  [ DFT{x(n)} + DFT{x'(n)} ]                         |
     | # X2(k) = -0.5*j*[ DFT{x(n)} - DFT{x'(n)} ]                         |
     | -> Using Complex-Conjugate Property, x'(n) <-----> X'(N-k)          |
     | # X1(k) =  0.5*  [X(k) + X'(k)]                                     |
     |         =  0.5* { [Xr(k)+Xr(N-k)] + j*[Xi(k)-Xi(N-k)] }             |
     | # X2(k) = -0.5*j*[ X(k) - X'(k) ]                                   |
     |         =  0.5* { [Xi(k)+Xi(N-k)] - j*[Xr(k)-Xr(N-k)] }             |
     | -> Additioanlly using following property, X1(k), X2(k) are more     |
     |    efficiently computed;                                            |
     |    If x1(n), x2(n) are real sequences,                              |
     |    X1r(k)=X1r(N-k),  X2r(k)=X2r(N-k);  k = 1,2,...,(N/2)-1 for even |
     |    X1i(k)=-X1i(N-k), X2i(k)=-X2i(N-k); k = 1,2,...,(N/2)-1 for odd  |
     |    X(0)=X(N); X(0),X(N) are real due to sampling theorem            |
     |                                            of N-point FFT.          |
     | -> Express 2N-point DFT, G(k) in terms of the two N-point DFT's,    |
     |    X1(k) and X(k) and then finally interms of X(k).                 |
     |                                                                     |
     |    W(kn,N) = e^(-j*2*pi*k*n/N).                                     |
     |                                                                     |
     |           N-1                   N-1                                 |
     |    G(k) = SUM g(2n)*W(2kn,2N) + SUM g(2n+1)*W(k(2n+1),2N)           |
     |           n=0                   n=0                                 |
     |           N-1                             N-1                       |
     |         = SUM x1(n)*W(kn,N)   + W(k,2N) * SUM x2(n)*W(kn,N)         |
     |           n=0                             n=0                       |
     |    Thus,                                                            |
     |    G(k)   = X1(k) + W(k,2N)*X2(k) ; k = 0,1,2,...,N-1,              |
     |    G(k+N) = X1(k) - W(k,2N)*X2(k) ; k = 0,1,2,...,N-1.              |
     |---------------------------------------------------------------------|
    */
    /*-- k = 0 ----------------------------*/
#if INTEL_FORMAT
     fft_buf[0] = cfft_buf[0] + cfft_buf[1];
     fft_buf[1] = (F)0.0;
#else
     fft_buf[0] = cfft_buf[0] + cfft_buf[1];
	 fft_buf[1] = (F)0.0;
#endif

    /*-- k = 1~N-1 ------------------------*/
     j = lenRFFT; /* 2N = 256 */
     for (i=2; i<lenCFFT; i+=2)
     {
         j -= 2;

		 /*real_add = cfft_buf[i]   + cfft_buf[j];
         real_sub = cfft_buf[i]   - cfft_buf[j];
         imag_add = cfft_buf[i+1] + cfft_buf[j+1];
         imag_sub = cfft_buf[i+1] - cfft_buf[j+1];*/
         real_add = (F)0.5 * (cfft_buf[i]   + cfft_buf[j]);
         real_sub = (F)0.5 * (cfft_buf[i]   - cfft_buf[j]);
         imag_add = (F)0.5 * (cfft_buf[i+1] + cfft_buf[j+1]);
         imag_sub = (F)0.5 * (cfft_buf[i+1] - cfft_buf[j+1]);
		 
        /*---------------------------------------------------------------------------
         fft_buf[i]   =  real_add + [(imag_add * COSxW[n]) - (real_sub * SINxW[n])];
         fft_buf[i+1] =  imag_sub - [(real_sub * COSxW[n]) + (imag_add * SINxW[n])];

         fft_buf[j]   =  real_add - [(imag_add * COSxW[n]) - (real_sub * SINxW[n])];
         fft_buf[j+1] = -imag_sub - [(real_sub * COSxW[n]) + (imag_add * SINxW[n])];
         ---------------------------------------------------------------------------*/
         c1 = ( imag_add * (*p_cos)  ) - ( real_sub * (*p_sin)  );
         c2 = ( real_sub * (*p_cos++)) + ( imag_add * (*p_sin--));

         fft_buf[i]   =  real_add + c1;
         fft_buf[i+1] =  imag_sub - c2;
         fft_buf[j]   =  real_add - c1;
         fft_buf[j+1] = -imag_sub - c2; //-(imag_sub + c2);
		 /*fft_buf[i]   =  (F)0.5 * (real_add + c1);
         fft_buf[i+1] =  (F)0.5 * (imag_sub - c2);
         fft_buf[j]   =  (F)0.5 * (real_add - c1);
         fft_buf[j+1] =  (F)0.5 * (-imag_sub - c2); //-(imag_sub + c2);*/
     }

    /*-- k = N/2 at PI/2 for fft_buf[128], fft_buf[129] --*/
     fft_buf[i]   =  cfft_buf[i];
     fft_buf[i+1] = -cfft_buf[i+1];


    /*-- k = N at PI for fft_buf[256],fft_buf[257] --*/
#if INTEL_FORMAT
     fft_buf[lenRFFT]   = cfft_buf[0] - cfft_buf[1];
     fft_buf[lenRFFT+1] = (F)0.0;
#else
     fft_buf[1] = cfft_buf[0] - cfft_buf[1];
#endif

  }

 /*========================================================================*/
 /*  Inverse Real FFT                                                      */
 /*========================================================================*/
  else
  {
    /*
     |---------------------------------------------------------------------|
     |< Stage 1 >                                                          |
     |Express the 2N-point DFT, G(k) in terms of the two N-point DFT's,    |
     | X1(k) and X2(k) and finally onterms of X(k).                        |
     |                                                                     |
     |(1) Definition of X(k), X1(k), X2(k) :                               |
     |  # g(n) : 2N-point real sequence (inverse FFT output of G(K).       |
     |  # x1(n) = g(2n)  ; n=0,1,...,N-1 ; even sequence of g(n)           |
     |  # x2(n) = g(2n+1); n=0,1,...,N-1 ; odd  sequence of g(n)           |
     |  # x1(n), x2(n) are real sequences.                                 |
     |  # x(n) = x1(n) + j*x2(n) ; x(n)is complex value (not real!)        |
     |  # x1(n) =  0.5*  [x(n)+x'(n)] ; x(n),x'(n) are conjugate pair      |
     |  # x2(n) = -0.5*j*[x(n)-x'(n)] ; x(n),x'(n) are conjugate pair      |
     |                                                                     |
     |(2)Using Linear property of DFT :                                    |
     | # X(k) = X1(k)+jX2(k) <---------> x(n) = x1(n)+jx2(n)               |
     | # X1(k) =  0.5*  [ DFT{x(n)} + DFT{x'(n)} ]                         |
     | # X2(k) = -0.5*j*[ DFT{x(n)} - DFT{x'(n)} ]                         |
     |                                                                     |
     | (3)Using Complex-Conjugate Property, x'(n) <-----> X'(N-k)          |
     | # X1(k) =  0.5*  [X(k) + X'(k)]                                     |
     |         =  0.5* { [Xr(k)+Xr(N-k)] + j*[Xi(k)-Xi(N-k)] }             |
     | # X2(k) = -0.5*j*[ X(k) - X'(k) ]                                   |
     |         =  0.5* { [Xi(k)+Xi(N-k)] - j*[Xr(k)-Xr(N-k)] }             |
     |                                                                     |
     | (4)Additioanlly using following property, X1(k), X2(k) are more     |
     |    efficiently computed;                                            |
     |    If x1(n), x2(n) are real sequences,                              |
     |    # X1r(k)=X1r(N-k), X2r(k)=X2r(N-k);  k = 1,2,..,N-1              |
     |      X1i(k)=-X1i(N-k),X2i(k)=-X2i(N-k); k = 1,2,..,N-1              |
     |    # X1(0),X2(0) are always real because x1(n),x2(n) are real.      |
     |      Thus, X1(N),X2(N) are real and X1(0)=X1(N),X2(0)=X2(N).        |
     |                                                                     |
     | (5)Express 2N-point DFT, G(k) in terms of the two N-point DFT's,    |
     |    X1(k) and X(k) and finally in terms of X(k).                     |
     |                                                                     |
     |    # W(kn,N) = e^(-j*2*pi*k*n/N).                                   |
     |             N-1                   N-1                               |
     |    # G(k) = SUM g(2n)*W(2kn,2N) + SUM g(2n+1)*W(k(2n+1),2N)         |
     |             n=0                   n=0                               |
     |              N-1                             N-1                    |
     |            = SUM x1(n)*W(kn,N)   + W(k,2N) * SUM x2(n)*W(kn,N)      |
     |              n=0                             n=0                    |
     |      Thus,                                                          |
     |       G(k)   = X1(k) + W(k,2N)*X2(k) ; k = 0,1,2,...,N-1,           |
     |       G(k+N) = X1(k) - W(k,2N)*X2(k) ; k = 0,1,2,...,N-1.           |
     |    # X1(k) = 0.5*[G(k)+G'(N-k)]          ; k = 0,1,2,...,N-1,       |
     |      X2(k) = 0.5*W(-k,2N)*[G(k)-G'(N-k)] ; k = 0,1,2,...,N-1.       |
     |    # if A=PI*k/N, G(k)=Gr(k)+jGi(k),                                |
     |      X(k) = X1(k) + jX2(k)                                          |
     |           = 0.5*[   [ {Gr(k}+Gr(N-k)} - {Gi(k}+Gi(N-k)}*cos(A)      |
     |                                     - {Gr(k}-Gr(N-k)}*sin(A) ]      |
     |                  +j*[ {Gi(k}-Gi(N-k)} + {Gr(k}-Gr(N-k)}*cos(A)      |
     |                                     - {Gi(k}+Gi(N-k)}*sin(A) ]   ]  |
     |    # G(0),G(N) are all real values!                                 |
     |      G'(0)=G(0), G'(N)=G(N)                                         |
     |    # X(0) = 0.5*[G(0) +G'(N)]  + 0.5*j*[G(0) -G'(N)]                |
     |           = 0.5*[Gr(0)+Gr'(N)] + 0.5*j*[Gr(0)-Gr'(N)]               |
     |---------------------------------------------------------------------|
    */

    /*-- k = 0 ----------------------------*/
#if INTEL_FORMAT
     cfft_buf[0] = (F)0.5 * (fft_buf[0] + fft_buf[lenRFFT]);
     cfft_buf[1] = (F)0.5 * (fft_buf[0] - fft_buf[lenRFFT]);
#else
     cfft_buf[0] = (F)0.5 * (fft_buf[0] + fft_buf[1]);
     cfft_buf[1] = (F)0.5 * (fft_buf[0] - fft_buf[1]);
#endif

    /*-- k = 1~N-1 ------------------------*/
     j = lenRFFT; /* 2N = 256 */
     for (i=2; i<lenCFFT; i+=2)
     {
         j -= 2;

         real_add = (F)0.5 * (fft_buf[i]   + fft_buf[j]);
         real_sub = (F)0.5 * (fft_buf[i]   - fft_buf[j]);
         imag_add = (F)0.5 * (fft_buf[i+1] + fft_buf[j+1]);
         imag_sub = (F)0.5 * (fft_buf[i+1] - fft_buf[j+1]);
		 /*real_add = fft_buf[i]   + fft_buf[j];
         real_sub = fft_buf[i]   - fft_buf[j];
         imag_add = fft_buf[i+1] + fft_buf[j+1];
         imag_sub = fft_buf[i+1] - fft_buf[j+1];*/

        /*--------------------------------------------------------------------------
         cfft_buf[i]  =  real_add - [(imag_add * COSxW[n]) + (real_sub * SINxW[n])];
         cfft_buf[i+1]=  imag_sub + [(real_sub * COSxW[n]) - (imag_add * SINxW[n])];
         cfft_buf[j]  =  real_add + [(imag_add * COSxW[n]) + (real_sub * SINxW[n])];
         cfft_buf[j+1]= -imag_sub + [(real_sub * COSxW[n]) - (imag_add * SINxW[n])];
         ---------------------------------------------------------------------------*/
         c1 = ( imag_add * (*p_cos)  ) + ( real_sub * (*p_sin)  );
         c2 = ( real_sub * (*p_cos++)) - ( imag_add * (*p_sin--));

		 cfft_buf[i]   =  real_add - c1;
         cfft_buf[i+1] =  imag_sub + c2;
         cfft_buf[j]   =  real_add + c1;
         cfft_buf[j+1] = -imag_sub + c2; //-(imag_sub - c2);
         /*cfft_buf[i]   =  (F)0.5 * (real_add - c1);
         cfft_buf[i+1] =  (F)0.5 * (imag_sub + c2);
         cfft_buf[j]   =  (F)0.5 * (real_add + c1);
         cfft_buf[j+1] = (F)0.5 * (-imag_sub + c2); //-(imag_sub - c2);*/
     }

    /*-- k = N/2 at PI/2 for cfft_buf[128], cfft_buf[129] --*/
     cfft_buf[i]   =  fft_buf[i];
     cfft_buf[i+1] = -fft_buf[i+1];

    /*
     |-----------------------------------------------------------------------|
     | < Stage 2 >                                                           |
     | Packing & Bit-Reversal of 256-point input sequence, X(k)              |
     |  for inverse FFT.                                                     |
     |                                                                       |
     | fft_buf[2n]   = bit-reversal of cfft_buf[2n]   = x1(n),n=0,1,2,.,N-1, |
     | fft_buf[2n+1] = bit-reversal of cfft_buf[2n+1] = x2(n),n=0,1,2,.,N-1. |
     |-----------------------------------------------------------------------|
     | < Stage 3 >                                                           |
     | Compute N(128)-point radix-2 DIT(Decomsition-In-Time) complex inverse |
     |                                                              FFT.     |
     | Using the property that DFT is linear operation,                      |
     |                               DFT                                     |
     |   x(k) = x1(k) + j*x2(k)  <---------> X(n) = X1(n) + j*X2(n),         |
     |    x1(k);even index real value <--->  X1(n);complex FFT value,        |
     |    x2(k);odd  index real value <--->  X2(n);complex FFT value,        |
     |                N-1                                                    |
     |   x(k) = (1/N)*SUM [X(n)*{ cos(2*pi*k*n/64)+j*sin(2*pi*k*n/N) }]      |
     |                n=0                                                    |
     |-----------------------------------------------------------------------|
    */
     cfft_flt (cfft_buf, fft_buf, lenCFFT, isign);

  }

 // free (cfft_buf);
  return;
}

/*----------------------------------------------------------------------------
* Function Name: cfft_flt()
*   Copyright (c) 2006 Ditech Networks Inc.
*
* Function:
*   This function implements N-point radix-2 Decomposition-In-Time
*   Complex FFT.
*
* Note:
*   1) Computational efficiency is mainly taken into consideration
*      by sacrificing ROM-table size.
*
*   2) Currently, this function works only for 128 and 64-point
*      complex FFT, but this can be extended to any size of FFT by
*      adding sine/cosine tables and modifying codes initializing
*      pointers of sine/cosine tables.
*
*   3) In the cfft_fx function, the FFT values are divided by
*      2 after each stage of computation thus dividing the
*      final FFT values by 128.  No multiplying factor is used
*      for the IFFT.  This is somewhat different from the usual
*      definition of FFT where the factor 1/N, i.e., 1/128, is
*      used for the IFFT and not the FFT.  No factor is used in
*      the rfft_fx function.
*
*   4) Much of the code for the FFT and IFFT parts in rfft_fx
*      and cfft_fx functions are similar and can be combined.
*      They are, however, kept separate here to save cycles
*      (execution speed).
*
* Parameters:
*   *data_i : complex data input in bit-reversed order for FFT
*   lenCFFT : length of N-point Complex FFT (hlaf size of Real FFT)
*   isign   : FFT mode (FORWARD:1, INVERSE:-1)
*   SINW[]  : a full cycle for sine term of N-point
*            (read-only static variable)
* Outputs:
*   *data  : FFT/IFFT output of complex / real sequence
*
* Returns:
*   None
*
* Local functions:
*   bitrev_flt()
*
* History:
*    Oct.01.1998/JB Hong   File crelated!
*---------------------------------------------------------------------------*/
void cfft_flt (float *data_i,float *data, int lenCFFT, int isign)
{

  int     i, j, m, k, kk, mmax, istep, lenRFFT;
  float   tmp_real, tmp_imag;
  float   *p_sin, *p_cos;
  int     *p_ADDR_BITREV;
  float   scale;

  lenRFFT = (short) (lenCFFT << 1); /* 256 = 128*2 */

 /*---------------------------------------------------------------------*/
 /* Pointer Initialization for sine,cosine tables according to FFT size */
 /*---------------------------------------------------------------------*/
  p_ADDR_BITREV = ADDR_BITREV_256;
  p_cos = &SINW_256_flt[lenCFFT>>2];                     // = SINW[32]
  if (isign==FORWARD) p_sin = &SINW_256_flt[0];
  else                p_sin = &SINW_256_flt[lenCFFT>>1]; // = SINW[64]

  if (lenRFFT==128)
  {
     p_ADDR_BITREV = ADDR_BITREV_128;
     p_cos = &SINW_128_flt[lenCFFT>>2];                     // = SINW[16]
     if (isign==FORWARD) p_sin = &SINW_128_flt[0];
     else                p_sin = &SINW_128_flt[lenCFFT>>1]; // = SINW[32]
  }

 /*---------------------------------------------------------------------*/
 /* Packing & Bit-Reversal of 256-point real input sequence, g(n) to    */
 /* complex 128-point input signal, x(n) for FFT.                       */
 /*   g(n)  : real-valued input sequence of 2N points.                  */
 /*   x1(n) = g(2n)         , n=0,1,2,3,..,N-1.                         */
 /*   x2(n) = g(2n+1)       , n=0,1,2,3,..,N-1.                         */
 /*   x(n)  = x1(n)+j*x2(n) , n=0,1,2,3,..,N-1, in bit-reversed order   */
 /*                                                                     */
 /* cfft_buf[2n]   = bit-reversal for fft_buf[2n]   = x1(n),n=0,1,.,N-1,*/
 /* cfft_buf[2n+1] = bit-reversal for fft_buf[2n+1] = x2(n),n=0,1,.,N-1.*/
 /*---------------------------------------------------------------------*/
  bitrev_flt (data_i, data, p_ADDR_BITREV, lenCFFT);

 /*=====================================================================*/
 /* Forward Complex FFT (Radix-2 Decomposition-In-Tim Complex FFT)      */
 /*=====================================================================*/
  if (isign==FORWARD)
  {
    /*-- scaling in forward complex FFT ------*/
     scale = (F)0.5;
    /*-- initialization of idices for FFT -------------*/
     kk = lenCFFT >> 1; /* 64 */
     mmax = 2;

    /*---<1> 1st stage (No complex multiplication in twiddle factor) ----------*/
     istep = 4; /* istep = mmax<<1 */
     for (i=0; i<lenRFFT; i+=istep)
     {
         j = i+mmax;

         tmp_real = data[j];
         tmp_imag = data[j+1];
         data[j]   = scale * (data[i]   - tmp_real); /* bottom branch */
         data[j+1] = scale * (data[i+1] - tmp_imag);
         data[i]   = scale * (data[i]   + tmp_real); /* upper branch  */
         data[i+1] = scale * (data[i+1] + tmp_imag);

     }

    /*---<2> middle stages ---------------------------------------------------*/
    /*
     |----------------------------------------------------------------------|
     | 1st Loop : Outer Loop of 5 times for log2 N (= log2 128 = 7) times.  |
     |----------------------------------------------------------------------|
    */
     mmax = istep;  // istep = 4
     while (lenCFFT > mmax)
     {
        istep = mmax<<1; /* istep: 4, --  8, 16, 32, 64, 128  --- 256 */
        kk >>= 1;        /* kk   : 64,-- 32, 16,  8,  4,   2, ---   1 */
        k = 0;           /* addressing sine & cos table  */

       /*
        |------------------------------------------------------------------|
        | 2nd Loop : Inner loop according the phase                        |
        |------------------------------------------------------------------|
       */
       /*-- For phase=0 (k=0) -------*/
        for ( i=0; i<lenRFFT; i+= istep )
        {
            j=i+mmax;

            tmp_real = data[j];
            tmp_imag = data[j+1];
            data[j]   = scale * (data[i]   - tmp_real);/* bottom branch */
            data[j+1] = scale * (data[i+1] - tmp_imag);
            data[i]   = scale * (data[i]   + tmp_real);/* upper branch */
            data[i+1] = scale * (data[i+1] + tmp_imag);
        }

        k += kk;

       /*-- For phase!=0 (k!=0) -----*/
        for (m=2; m<mmax; m+=2)
        {
           /*
            |-------------------------------------------------------------|
            | 3rd Loop : Inner-most loop for basic butterfly computation  |
            |-------------------------------------------------------------|
           */
            for (i=m; i<lenRFFT; i+=istep)
            {
                j = i + mmax;

               /*-- Complex Multiplication --------------*/
               /*-- ( d[n]+j*d[n+1] )*( cosW -j*sinW ) --*/
                tmp_real = data[j]*p_cos[k]   + data[j+1]*p_sin[k];
                tmp_imag = data[j+1]*p_cos[k] - data[j]*p_sin[k];

               /*-- Complex Butterfly Addition ----------*/
                data[j]   = scale * (data[i]   - tmp_real);/* bottom branch */
                data[j+1] = scale * (data[i+1] - tmp_imag);
                data[i]   = scale * (data[i]   + tmp_real);/* upper branch */
                data[i+1] = scale * (data[i+1] + tmp_imag);
            } /*--- End of 3rd Loop : Inner-most loop for butterfly ------*/

            k += kk;

        } /*------- End of 2nd : Inner loop -------------------------------*/

        mmax = istep;

     } /*--- End of 1st Loop : Outer loop (while loop) --------------------*/

    /*---<3> Last stage ------------------------------------------------------*/
    /*-- For phase=0 (k=0) -------*/
     tmp_real = data[mmax];
     tmp_imag = data[mmax+1];
     data[mmax]   = scale * (data[0] - tmp_real);   /* bottom branch */
     data[mmax+1] = scale * (data[1] - tmp_imag);
     data[0]      = scale * (data[0] + tmp_real);   /* upper branch */
     data[1]      = scale * (data[1] + tmp_imag);

    /*-- For phase!=0 (k!=0) -----*/
     k=1;
     for ( i=2; i<mmax; i+=2 )
     {
         j = i + mmax;

        /*-- Complex Multiplication --------------*/
        /*-- ( d[n]+j*d[n+1] )*( cosW -j*sinW ) --*/
         tmp_real = data[j]*p_cos[k]   + data[j+1]*p_sin[k];
         tmp_imag = data[j+1]*p_cos[k] - data[j]*p_sin[k];

        /*-- Complex Butterfly Addition ----------*/
         data[j]   = scale * (data[i]   - tmp_real);   /* bottom branch */
         data[j+1] = scale * (data[i+1] - tmp_imag);
         data[i]   = scale * (data[i]   + tmp_real);   /* upper branch */
         data[i+1] = scale * (data[i+1] + tmp_imag);

         k++;
     }

  } /* End of forward FFT */

 /*=====================================================================*/
 /* Inverse Complex FFT (Radix-2 Decomposition-In-Tim Complex FFT)      */
 /*=====================================================================*/
  else
  {
    /*-- No scaling in forward complex FFT ---*/
     scale = (F)1.0;

    /*-- initialization of idices for FFT -------------*/
     kk = lenCFFT >> 1; /* 64 */

     mmax = 2;

    /*---<1> 1st stage (No complex multiplication in twiddle factor) ----------*/
     istep = 4; /* istep = mmax<<1 */
     for (i=0; i<lenRFFT; i+=istep)
     {

         j = i+mmax;

         tmp_real = data[j];
         tmp_imag = data[j+1];

         data[j]   = scale * (data[i]   - tmp_real); /* bottom branch */
         data[j+1] = scale * (data[i+1] - tmp_imag);
         data[i]   = scale * (data[i]   + tmp_real); /* upper branch  */
         data[i+1] = scale * (data[i+1] + tmp_imag);
     }

    /*---<2> middle stages ---------------------------------------------------*/
    /*
     |----------------------------------------------------------------------|
     | 1st Loop : Outer Loop of 5 times for log2 N (= log2 128 = 7) times.  |
     |----------------------------------------------------------------------|
    */
     mmax = istep;  // istep = 4
     while (lenCFFT > mmax)
     {
        istep = mmax<<1; /* istep: 4, --  8, 16, 32, 64, 128  --- 256 */
        kk >>= 1;        /* kk   : 64,-- 32, 16,  8,  4,   2, ---   1 */
        k = 0;           /* addressing sine & cos table  */

       /*
        |------------------------------------------------------------------|
        | 2nd Loop : Inner loop according the phase                        |
        |------------------------------------------------------------------|
       */
       /*-- For phase=0 (k=0) -------*/
        for ( i=0; i<lenRFFT; i+= istep )
        {
            j=i+mmax;

            tmp_real = data[j];
            tmp_imag = data[j+1];

            data[j]   = scale * (data[i]   - tmp_real);/* bottom branch */
            data[j+1] = scale * (data[i+1] - tmp_imag);
            data[i]   = scale * (data[i]   + tmp_real);/* upper branch */
            data[i+1] = scale * (data[i+1] + tmp_imag);
        }

        k += kk;

       /*-- For phase!=0 (k!=0) -----*/
        for (m=2; m<mmax; m+=2)
        {
           /*
            |-------------------------------------------------------------|
            | 3rd Loop : Inner-most loop for basic butterfly computation  |
            |-------------------------------------------------------------|
           */
            for (i=m; i<lenRFFT; i+=istep)
            {
                j = i + mmax;

               /*-- Complex Multiplication --------------*/
               /*-- ( d[n]+j*d[n+1] )*( cosW -j*sinW ) --*/
                tmp_real = data[j]*p_cos[k]   + data[j+1]*p_sin[k];
                tmp_imag = data[j+1]*p_cos[k] - data[j]*p_sin[k];

               /*-- Complex Butterfly Addition ----------*/
                data[j]   = scale * (data[i]   - tmp_real);/* bottom branch */
                data[j+1] = scale * (data[i+1] - tmp_imag);
                data[i]   = scale * (data[i]   + tmp_real);/* upper branch */
                data[i+1] = scale * (data[i+1] + tmp_imag);
            } /*--- End of 3rd Loop : Inner-most loop for butterfly ------*/

            k += kk;

        } /*------- End of 2nd : Inner loop -------------------------------*/

        mmax = istep;

     } /*--- End of 1st Loop : Outer loop (while loop) --------------------*/

    /*---<3> Last stage ------------------------------------------------------*/
    /*-- For phase=0 (k=0) -------*/
     tmp_real = data[mmax];
     tmp_imag = data[mmax+1];

     data[mmax]   = scale * (data[0] - tmp_real);   /* bottom branch */
     data[mmax+1] = scale * (data[1] - tmp_imag);
     data[0]      = scale * (data[0] + tmp_real);   /* upper branch */
     data[1]      = scale * (data[1] + tmp_imag);

    /*-- For phase!=0 (k!=0) -----*/
     k=1;
     for ( i=2; i<mmax; i+=2 )
     {
         j = i + mmax;

        /*-- Complex Multiplication --------------*/
        /*-- ( d[n]+j*d[n+1] )*( cosW -j*sinW ) --*/
         tmp_real = data[j]*p_cos[k]   + data[j+1]*p_sin[k];
         tmp_imag = data[j+1]*p_cos[k] - data[j]*p_sin[k];

        /*-- Complex Butterfly Addition ----------*/
         data[j]   = scale * (data[i]   - tmp_real);   /* bottom branch */
         data[j+1] = scale * (data[i+1] - tmp_imag);
         data[i]   = scale * (data[i]   + tmp_real);   /* upper branch */
         data[i+1] = scale * (data[i+1] + tmp_imag);

         k++;

     }
  } /* End of inverse FFT */

  return;
}







