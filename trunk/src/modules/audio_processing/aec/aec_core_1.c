#include "aec_core.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "aec_rdft.h"
#include "delay_estimator_wrapper.h"
#include "ring_buffer.h"
#include "system_wrappers/interface/cpu_features_wrapper.h"



#if (DITECH_VERSION==2)

void dnsample(short *sampleIn,float *dn_sampleout,short dns_rate,short len);
void shift_and_insert_far_end(float  *farEnd_buffer,short total_len,short dns_frame_len,float  *dns_farFrame_normalized);
void find_correlation(float *farEnd_buffer,float *dns_nearFrame,short dns_frame_len,float *instantaneous_cor_buff,short  totalNumOfCorrs,short spacing);
void analyze_corr_buff(float *long_term_corr_buff,float *long_term_corr_buff_along_states,float *long_term_corr_buff_stats_cntr,int *known_delay,int *known_delay_less_confidence,aec_t *aecp);
static void FilterFar_background(aec_t *aec, float yf[2][PART_LEN1]);
static void FilterAdaptation_background(aec_t *aec, float *fft, float ef[2][PART_LEN1]);

__inline static float MulRe(float aRe, float aIm, float bRe, float bIm)
{
    return aRe * bRe - aIm * bIm;
}

__inline static float MulIm(float aRe, float aIm, float bRe, float bIm)
{
    return aRe * bIm + aIm * bRe;
}

int WebRtcAec_CreateAecStatistical(statistical_aec_t **aecInst)
{
    statistical_aec_t *stats_aec = (statistical_aec_t *)malloc(sizeof(statistical_aec_t));
    *aecInst = stats_aec;
	printf("aec created\n");
    if (stats_aec == NULL) {
        return -1;
    }
	memset(stats_aec,0,sizeof(statistical_aec_t));

    if (WebRtcApm_CreateBuffer(&stats_aec->farFrBuf, FRAME_LEN + PART_LEN) == -1) {
        WebRtcAec_FreeAecStatistical(stats_aec);
        stats_aec = NULL;
        return -1;
    }

    if (WebRtcApm_CreateBuffer(&stats_aec->nearFrBuf, FRAME_LEN + PART_LEN) == -1) {
        WebRtcAec_FreeAecStatistical(stats_aec);
        stats_aec = NULL;
        return -1;
    }

    if (WebRtcApm_CreateBuffer(&stats_aec->outFrBuf, FRAME_LEN + PART_LEN) == -1) {
        WebRtcAec_FreeAecStatistical(stats_aec);
        stats_aec = NULL;
        return -1;
    }

    
    return 0;
}

int WebRtcAec_FreeAecStatistical(statistical_aec_t *stats_aec)
{
	WebRtcApm_FreeBuffer(stats_aec->farFrBuf);
    WebRtcApm_FreeBuffer(stats_aec->nearFrBuf);
    WebRtcApm_FreeBuffer(stats_aec->outFrBuf);
	return 1;

}



//this aec is simply downsampling the incoming streams by factor of  8 and trying to estimate the delay by working out a rough set of coefficients for filter
void WebRtcAec_ProcessFrame_Statistical(statistical_aec_t *aec, const short *farend,
                       const short *nearend,aec_t *aecp)
{
	int i;
	
	float dns_farFrame[DNS_FRAME_LEN],dns_nearFrame[DNS_FRAME_LEN];
	
	

	
	/* got a frame length of samples=80 at 16k or 8k.SR does not really matter*/
	

	dnsample(( short *)nearend,dns_nearFrame,DNS_RATE,FRAME_LEN);
	dnsample((short *)farend,dns_farFrame,DNS_RATE,FRAME_LEN);

	shift_and_insert_far_end(aec->farBuf,HUNDRED_MS_IN_16K_DNS+DNS_FRAME_LEN,DNS_FRAME_LEN,dns_farFrame);

	find_correlation(aec->farBuf,dns_nearFrame,DNS_FRAME_LEN,aec->instantaneous_cor_buff,HUNDRED_MS_IN_16K_DNS,1);

	for( i=0;i<HUNDRED_MS_IN_16K_DNS;i++)
	{
		aec->long_term_cor_buff[i]=(float)(aec->long_term_cor_buff[i]*.9+(float)0.1*aec->instantaneous_cor_buff[i]);
	}

	analyze_corr_buff(aec->long_term_cor_buff,aec->long_term_corr_buff_along_states,aec->long_term_corr_buff_stats_cntr,&aec->known_delay,&aec->known_delay_less_confidence,aecp);
	
	if(aec->known_delay_less_confidence>0)
	{
				aec->processed_known_delay=((aec->known_delay_less_confidence-5)/10)*10;
				if(aec->processed_known_delay<0)
					aec->processed_known_delay=0;
				aec->last_known_delay=aec->known_delay_less_confidence;
	}
	else
	{
			aec->processed_known_delay=-1;
			
	}


}


void dnsample(short *sampleIn,float *dn_sampleout,short dns_rate,short len)
{
	int i;
	for (i=0;i<len/dns_rate;i++)
	{
		dn_sampleout[i]=sampleIn[i*dns_rate];
	}
}

void shift_and_insert_far_end(float  *farEnd_buffer,short total_len,short dns_frame_len,float  *dns_farFrame_normalized)
{
	short i;

	for(i=total_len-1;i>=dns_frame_len;i--)
	{
		farEnd_buffer[i]=farEnd_buffer[i-dns_frame_len];
	}

	for(i=0;i<dns_frame_len;i++)
	{
		farEnd_buffer[i]=dns_farFrame_normalized[i];
	}

}

float sum_dot_product(float *buf1,float *buf2,short size)
{
	int i;
	float sum=0;
	for(i=0;i<size;i++)
	{
		sum+=buf1[i]*buf2[i];

	}
	return sum;

}

void find_correlation(float *farEnd_buffer,float *dns_nearFrame,short dns_frame_len,float *instantaneous_cor_buff,short  totalNumOfCorrs,short spacing)
{
	int i;
	for (i=0;i<totalNumOfCorrs;i++)
	{
		instantaneous_cor_buff[i]= ((float)sum_dot_product(dns_nearFrame,&farEnd_buffer[i],dns_frame_len)/dns_frame_len);
		
	}

}

void analyze_corr_buff(float *long_term_corr_buff,float *long_term_corr_buff_along_states,float *long_term_corr_buff_stats_cntr,int *known_delay,int *known_delay_less_confidence,aec_t *aecp)
{
	int i,j;
	float max_seen,all_time_max;
	*known_delay=-1;//mark it not set
	*known_delay_less_confidence=-1;

	for(i=0;i<HUNDRED_MS_IN_16K_DNS;i++)
	{
		
		
		long_term_corr_buff_along_states[i]=fabsf((float)long_term_corr_buff[i]);

	}

	
	all_time_max=0;
	for(i=0;i<HUNDRED_MS_IN_16K_DNS;i++)
	{
		/*find the max of next five and replace the existing value.This will help us in getting a gradient approximation
		of the delay enevlope
		*/
		max_seen=0;
		for(j=i;j<i+10 && j<HUNDRED_MS_IN_16K_DNS;j++)
		{
			if(max_seen<long_term_corr_buff_along_states[j])
				max_seen=long_term_corr_buff_along_states[j];
		}
		
		long_term_corr_buff_along_states[i]=max_seen;
		if(max_seen>all_time_max)
			all_time_max=max_seen;

	}
	if(all_time_max>0)
	{
		for(i=0;i<HUNDRED_MS_IN_16K_DNS;i++)
		{
			long_term_corr_buff_stats_cntr[i]+=long_term_corr_buff_along_states[i]/all_time_max;
		}
	}
	/*for(i=0;i<200;i++)
		fprintf(fout,"%f\n",long_term_corr_buff_stats_cntr[i]);*/


	//do analysis on the long_term_corr_buff_stats_cntr
	{
		float max_value=0,average_value=0,sum_value=0;
		int max_indx=0;
		for(i=0;i<HUNDRED_MS_IN_16K_DNS;i++)
		{
			if(max_value<long_term_corr_buff_stats_cntr[i])
			{
				max_value=long_term_corr_buff_stats_cntr[i];
				max_indx=i;
				
			}
			sum_value+=long_term_corr_buff_stats_cntr[i];
		}
		average_value=sum_value/HUNDRED_MS_IN_16K_DNS;
#define MAX_THRSHLD 1000
		if(max_value>MAX_THRSHLD /*&& max_value>2*average_value*/)
		{
			float local_max=0;
			int local_max_indx=0;
			//data may be useful analyze further
			//check if there are some more peaks in the delay enevelope by checking for local peaks in far regions from found maxima peak.
			//if they do ignore the result
			for(i=0;i<200;i++)
			{
				if((i>max_indx+10) ||(i<max_indx-10))
				{
					if(local_max<long_term_corr_buff_stats_cntr[i])
					{
						local_max=long_term_corr_buff_stats_cntr[i];
						local_max_indx=i;
					}
				}

			}
			//check if this local max is heavy enough
			if((max_value-local_max)>(max_value/10))
			{
				//use this result and set the known delay parameter
				*known_delay=max_indx;
			}
			
			for(i=0;i<200;i++)
			{
					//long_term_corr_buff_stats_cntr[i]-=max_value/2;
					//if(long_term_corr_buff_stats_cntr[i]<0)
						long_term_corr_buff_stats_cntr[i]=0;
						long_term_corr_buff_stats_cntr[max_indx]=MAX_THRSHLD/5;


			}

		}

		if(max_value>MAX_THRSHLD/3 /*&& max_value>2*average_value*/)
		{
			float local_max=0;
			int local_max_indx=0;
			//data may be useful analyze further
			//check if there are some more peaks in the delay enevelope by checking for local peaks in far regions from found maxima peak.
			//if they do ignore the result
			for(i=0;i<200;i++)
			{
				if((i>max_indx+10) ||(i<max_indx-10))
				{
					if(local_max<long_term_corr_buff_stats_cntr[i])
					{
						local_max=long_term_corr_buff_stats_cntr[i];
						local_max_indx=i;
					}
				}

			}
			//check if this local max is heavy enough
			if((max_value-local_max)>(max_value/10))
			{
				//use this result and set the known delay parameter
				*known_delay_less_confidence=max_indx;
			}
			
			

		}

		
	}


}



void ProcessBlock_background(aec_t *aec, const short *farend,
                         const short *nearend, short *output, short vadState,short *farFrameAtEchoDelay)
{
    int i;
    float d[PART_LEN], y[PART_LEN], e[PART_LEN], dH[PART_LEN];
    short eInt16[PART_LEN];
    float scale;

    float fft[PART_LEN2],fft_re[PART_LEN2],fft_im[PART_LEN2];
    float xf[2][PART_LEN1], yf[2][PART_LEN1], ef[2][PART_LEN1];
    complex_t df[PART_LEN1];
    float far_spectrum = 0.0f;
    float near_spectrum = 0.0f;
    float abs_far_spectrum[PART_LEN1];
    float abs_near_spectrum[PART_LEN1];

    const float gPow[2] = {0.9f, 0.1f};

    // Noise estimate constants.
    const int noiseInitBlocks = 500 * aec->mult;
    const float step = 0.1f;
    const float ramp = 1.0002f;
    const float gInitNoise[2] = {0.999f, 0.001f};


	
    memset(dH, 0, sizeof(dH));

    // ---------- Ooura fft ----------
    // Concatenate old and new farend blocks.
    for (i = 0; i < PART_LEN; i++) {
		aec->xBuf_background[i + PART_LEN] = (float)farend[i];
        d[i] = (float)nearend[i];
    }

    

	memcpy(fft, aec->xBuf_background, sizeof(float) * PART_LEN2);
    memcpy(aec->dBuf_background + PART_LEN, d, sizeof(float) * PART_LEN);
   
	
    aec_rdft_forward_128(fft);

	

    // Far fft
    xf[1][0] = 0;
    xf[1][PART_LEN] = 0;
    xf[0][0] = fft[0];
    xf[0][PART_LEN] = 0;//fft[1];

    for (i = 1; i < PART_LEN; i++) {
        xf[0][i] = fft[2 * i];
        xf[1][i] = fft[2 * i + 1];
    }


    // Near fft
	memcpy(fft, aec->dBuf_background, sizeof(float) * PART_LEN2);
    aec_rdft_forward_128(fft);
    df[0][1] = 0;
    df[PART_LEN][1] = 0;
    df[0][0] = fft[0];
    df[PART_LEN][0] = fft[1];

    for (i = 1; i < PART_LEN; i++) {
        df[i][0] = fft[2 * i];
        df[i][1] = fft[2 * i + 1];
    }


    // Power smoothing
    for (i = 0; i < PART_LEN1; i++) {
      far_spectrum = xf[0][i] * xf[0][i] + xf[1][i] * xf[1][i];
      aec->xPow[i] = gPow[0] * aec->xPow[i] + gPow[1] * NR_PART * far_spectrum;
      // Calculate absolute spectra
      abs_far_spectrum[i] = sqrtf(far_spectrum);

      near_spectrum = df[i][0] * df[i][0] + df[i][1] * df[i][1];
      aec->dPow[i] = gPow[0] * aec->dPow[i] + gPow[1] * near_spectrum;
      // Calculate absolute spectra
      abs_near_spectrum[i] = sqrtf(near_spectrum);
    }

    // Estimate noise power. Wait until dPow is more stable.
    if (aec->noiseEstCtr > 50) {
        for (i = 0; i < PART_LEN1; i++) {
            if (aec->dPow[i] < aec->dMinPow[i]) {
                aec->dMinPow[i] = (aec->dPow[i] + step * (aec->dMinPow[i] -
                    aec->dPow[i])) * ramp;
            }
            else {
                aec->dMinPow[i] *= ramp;
            }
        }
    }

    // Smooth increasing noise power from zero at the start,
    // to avoid a sudden burst of comfort noise.
    if (aec->noiseEstCtr < noiseInitBlocks) {
        aec->noiseEstCtr++;
        for (i = 0; i < PART_LEN1; i++) {
            if (aec->dMinPow[i] > aec->dInitMinPow[i]) {
                aec->dInitMinPow[i] = gInitNoise[0] * aec->dInitMinPow[i] +
                    gInitNoise[1] * aec->dMinPow[i];
            }
            else {
                aec->dInitMinPow[i] = aec->dMinPow[i];
            }
        }
        aec->noisePow = aec->dInitMinPow;
    }
    else {
        aec->noisePow = aec->dMinPow;
    }

   

    // Update the xfBuf block position.
    aec->xfBufBlockPos_background--;
    if (aec->xfBufBlockPos_background == -1) {
        aec->xfBufBlockPos_background = NR_PART - 1;
    }

    // Buffer xf
    memcpy(aec->xfBuf_background[0] + aec->xfBufBlockPos_background * PART_LEN1, xf[0],
           sizeof(float) * PART_LEN1);
    memcpy(aec->xfBuf_background[1] + aec->xfBufBlockPos_background * PART_LEN1, xf[1],
           sizeof(float) * PART_LEN1);

    memset(yf[0], 0, sizeof(float) * (PART_LEN1 * 2));

    // Filter far
    FilterFar_background(aec, yf);

	// Inverse fft to obtain echo estimate and error.
    fft[0] = yf[0][0];
    fft[1] = yf[0][PART_LEN];
    for (i = 1; i < PART_LEN; i++) {
        fft[2 * i] = yf[0][i];
        fft[2 * i + 1] = yf[1][i];
    }
    aec_rdft_inverse_128(fft);

    scale = 2.0f / PART_LEN2;
    for (i = 0; i < PART_LEN; i++) {
        y[i] = fft[PART_LEN + i] * scale; // fft scaling
    }

    for (i = 0; i < PART_LEN; i++) {
        e[i] = d[i] - y[i];
    }
    // Error fft
    memcpy(aec->eBuf_background + PART_LEN, e, sizeof(float) * PART_LEN);
    memset(fft, 0, sizeof(float) * PART_LEN);
    memcpy(fft + PART_LEN, e, sizeof(float) * PART_LEN);
    aec_rdft_forward_128(fft);

    ef[1][0] = 0;
    ef[1][PART_LEN] = 0;
    ef[0][0] = fft[0];
    ef[0][PART_LEN] = fft[1];
    for (i = 1; i < PART_LEN; i++) {
        ef[0][i] = fft[2 * i];
        ef[1][i] = fft[2 * i + 1];
    }


    // Scale error signal inversely with far power.
    WebRtcAec_ScaleErrorSignal(aec, ef);

    if(aec->framePowerAtProbableEchoDelay_shortTerm>1000000)
		FilterAdaptation_background(aec, fft, ef);

	

	aec->background_lt_filteredop_power=(float)(aec->background_lt_filteredop_power*.95+0.05*frame_power(aec->eBuf_background,PART_LEN2));
	for (i = 0; i < PART_LEN; i++) 
	{
				output[i] = (int16_t)WEBRTC_SPL_SAT(WEBRTC_SPL_WORD16_MAX, aec->eBuf_background[i],
				WEBRTC_SPL_WORD16_MIN);
	}

	memcpy(aec->xBuf_background, aec->xBuf_background + PART_LEN, sizeof(float) * PART_LEN);
	memcpy(aec->dBuf_background, aec->dBuf_background + PART_LEN, sizeof(float) * PART_LEN);
	memcpy(aec->eBuf_background, aec->eBuf_background + PART_LEN, sizeof(float) * PART_LEN);

}


void foreground_background_aec_analysis(aec_t *aec)
{
	float temp;
	if(aec->last_knownDelay_background!=aec->knownDelay_background)
	{
		aec->last_knownDelay_background=aec->knownDelay_background;
		memset(aec->wfBuf_background,0,sizeof(float)*NR_PART * PART_LEN1*2);
		aec->background_lt_filteredop_power=aec->lt_input_power;
		aec->foreground_lt_filteredop_power=aec->lt_input_power;

	}
	if((aec->background_lt_filteredop_power*1.2<aec->foreground_lt_filteredop_power))
	{
		//copy wcoefficinets
		temp=aec->foreground_lt_filteredop_power/aec->background_lt_filteredop_power;
		memcpy(aec->wfBuf,aec->wfBuf_background,sizeof(float)*NR_PART * PART_LEN1*2);
		aec->knownDelay=aec->knownDelay_background;
		aec->foreground_lt_filteredop_power=aec->background_lt_filteredop_power;
		//fwrite(&temp,sizeof(float),1,aec->filterFile1);
	}

	if(aec->background_lt_filteredop_power>aec->lt_input_power*3)
	{
		memset(aec->wfBuf_background,0,sizeof(float)*NR_PART * PART_LEN1*2);
		aec->background_lt_filteredop_power=aec->lt_input_power;
		aec->foreground_lt_filteredop_power=aec->lt_input_power;
	}

}



static void FilterFar_background(aec_t *aec, float yf[2][PART_LEN1])
{
  int i;
  for (i = 0; i < NR_PART; i++) {
    int j;
    int xPos = (i + aec->xfBufBlockPos_background) * PART_LEN1;
    int pos = i * PART_LEN1;
    // Check for wrap
    if (i + aec->xfBufBlockPos_background >= NR_PART) {
      xPos -= NR_PART*(PART_LEN1);
    }

    for (j = 0; j < PART_LEN1; j++) {
      yf[0][j] += MulRe(aec->xfBuf_background[0][xPos + j], aec->xfBuf_background[1][xPos + j],
                        aec->wfBuf_background[0][ pos + j], aec->wfBuf_background[1][ pos + j]);
      yf[1][j] += MulIm(aec->xfBuf_background[0][xPos + j], aec->xfBuf_background[1][xPos + j],
                        aec->wfBuf_background[0][ pos + j], aec->wfBuf_background[1][ pos + j]);
    }
  }
}


static void FilterAdaptation_background(aec_t *aec, float *fft, float ef[2][PART_LEN1]) {
  int i, j;
  for (i = 0; i < NR_PART; i++) {
    int xPos = (i + aec->xfBufBlockPos_background)*(PART_LEN1);
    int pos;
    // Check for wrap
    if (i + aec->xfBufBlockPos_background >= NR_PART) {
      xPos -= NR_PART * PART_LEN1;
    }

    pos = i * PART_LEN1;


    for (j = 0; j < PART_LEN1; j++) {
      aec->wfBuf_background[0][pos + j] += MulRe(aec->xfBuf_background[0][xPos + j],
                         -aec->xfBuf_background[1][xPos + j],
                         ef[0][j], ef[1][j]);
      aec->wfBuf_background[1][pos + j] += MulIm(aec->xfBuf_background[0][xPos + j],
                             -aec->xfBuf_background[1][xPos + j],
                             ef[0][j], ef[1][j]);
    }

  }


}

// Buffer the farend to account for knownDelay
void BufferFar_Arraywise(aec_t *aec, const short *farend, int farLen)
{
    int writeLen = farLen, writePos = 0;
	short farbuf_temp[FILT_LEN2 * 2];

	memcpy(farbuf_temp,aec->farBuf,sizeof(short)*FILT_LEN2*2);
	memcpy(&aec->farBuf[farLen],farbuf_temp,sizeof(short)*(FILT_LEN2*2-farLen));
	memcpy(aec->farBuf,farend,sizeof(short)*farLen);

    
}

void FetchFar_Arraywise(aec_t *aec, short *farend, int farLen, int knownDelay)
{
	memcpy(farend,&aec->farBuf[knownDelay],sizeof(short)*farLen);
    
}

float frame_power_short(short  *a,int size)
{
		float energy=0,power=0;
		int i;
		for(i=0;i<size;i++)
		{
			energy+=a[i]*a[i];
		}

		power=energy/size;
	
		return power;

}

float frame_power(float *a,int size)
{
		float energy=0,power=0;
		int i;
	

		for(i=0;i<size;i++)
		{
			energy+=a[i]*a[i];
		}

		power=energy/size;
	
		return power;

}



#endif