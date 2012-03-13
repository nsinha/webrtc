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

}



//this aec is simply downsampling the incoming streams by factor of  8 and trying to estimate the delay by working out a rough set of coefficients for filter
void WebRtcAec_ProcessFrame_Statistical(statistical_aec_t *aec, const short *farend,
                       const short *nearend,aec_t *aecp)
{
	int i;
	
	float dns_farFrame[DNS_FRAME_LEN],dns_nearFrame[DNS_FRAME_LEN];
	float temp;
	

	
	/* got a frame length of samples=80 at 16k or 8k.SR does not really matter*/
	

	dnsample(( short *)nearend,dns_nearFrame,DNS_RATE,FRAME_LEN);
	dnsample((short *)farend,dns_farFrame,DNS_RATE,FRAME_LEN);

	shift_and_insert_far_end(aec->farBuf,HUNDRED_MS_IN_16K_DNS+DNS_FRAME_LEN,DNS_FRAME_LEN,dns_farFrame);

	find_correlation(aec->farBuf,dns_nearFrame,DNS_FRAME_LEN,aec->instantaneous_cor_buff,HUNDRED_MS_IN_16K_DNS,1);

	for( i=0;i<HUNDRED_MS_IN_16K_DNS;i++)
	{
		aec->long_term_cor_buff[i]=aec->long_term_cor_buff[i]*.9+0.1*aec->instantaneous_cor_buff[i];
	}

	analyze_corr_buff(aec->long_term_cor_buff,aec->long_term_corr_buff_along_states,aec->long_term_corr_buff_stats_cntr,&aec->known_delay,&aec->known_delay_less_confidence,aecp);
	aec->processed_known_delay=((aec->known_delay_less_confidence-5)/10)*10;
	temp=aec->processed_known_delay;
	
	
	
	




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
	float temp_sum,max_seen,all_time_max;
	*known_delay=-1;//mark it not set
	*known_delay_less_confidence=-1;

	for(i=0;i<HUNDRED_MS_IN_16K_DNS;i++)
	{
		
		
		long_term_corr_buff_along_states[i]=abs(long_term_corr_buff[i]);

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
			float local_max=0,local_max_indx=0;
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
			float local_max=0,local_max_indx=0;
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
#endif