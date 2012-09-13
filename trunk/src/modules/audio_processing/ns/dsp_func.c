/***************************************************************************
 *
 *   FUNCTION NAME: sqroot
 *
 *   PURPOSE:
 *
 *     The purpose of this function is to perform a single precision square
 *     root function on a int32
 *
 *   INPUTS:
 *
 *     L_SqrtIn
 *                     input to square root function
 *
 *   OUTPUTS:
 *
 *     none
 *
 *   RETURN VALUE:
 *
 *     swSqrtOut
 *                     output to square root function
 *
 *   DESCRIPTION:
 *
 *      Input assumed to be normalized
 *
 *      The algorithm is based around a six term Taylor expansion :
 *
 *        y^0.5 = (1+x)^0.5
 *             ~= 1 + (x/2) - 0.5*((x/2)^2) + 0.5*((x/2)^3)
 *                - 0.625*((x/2)^4) + 0.875*((x/2)^5)
 *
 *      Max error less than 0.08 % for normalized input ( 0.5 <= x < 1 )
 *
 *************************************************************************/

#include "dsp_func.h"


//--------------------------------------
//
// Function: computeFramePwr 
//	This function is shared by ALC and PWR
//  
//--------------------------------------

void computeFramePwr_FLT(FLOAT *inbuf, FLOAT *framePwr,int16 framelen)
{

    int16 i;
    FLOAT temp=(F)0.0;
	FLOAT pcm,temp2,*buf;
    FLOAT inv_framelen;

	if (framelen==80) /* 10 ms*/
		inv_framelen = (F)0.0125;
	if (framelen==160) /* 20 ms */
		inv_framelen = (F)0.00625;

    buf = inbuf;
    for (i=0; i< framelen; i++){
        pcm = *buf++;
        temp2 = pcm *pcm;
		temp = temp + (temp2 * inv_framelen);
    }

	*framePwr = temp;
}

void applyLinGain_FLT(FLOAT *receiveInbuf, FLOAT *receiveOutbuf, FLOAT lin) 
{

	Word16 i;
    for (i=0; i < 80; i++)
	{
		receiveOutbuf[i] = receiveInbuf[i] * lin;
    }
}
