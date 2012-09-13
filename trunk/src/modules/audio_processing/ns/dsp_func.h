#ifndef _DSP_FUNC_H__
#define _DSP_FUNC_H__

#include "mpu_types.h"

void computeFramePwr_FLT(FLOAT *inbuf, FLOAT *framePwr,int16 framelen);
void applyLinGain_FLT(FLOAT *receiveInbuf, FLOAT *receiveOutbuf, FLOAT lin); 

#endif // _DSP_FUNC_H__
