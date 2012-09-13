#ifndef _VQA_CONST_H
#define _VQA_CONST_H


#define PCM_FRAME_SIZE_20MS         160
#define PCM_FRAME_SIZE_10MS	         80
#define PCM_FRAME_SIZE              PCM_FRAME_SIZE_20MS
#define FRAME_SIZE                  PCM_FRAME_SIZE

/* test load for measuring system end2endDelay */
#define PCM_ISR_MEASURE_DELAY 	0

/* starting from Release4, NS is removed from PM
 * because the image size is over 160KB, can not fit into QVP flash.
 * Refer to setting  USE_HC_PROCESS_COMPILE_OUT	.set 1
 * in file DSP\ANR_VAD\ASM\anr_switch.inc
 */

//#if (VED_FEATURE_GROUP == 1)/* This is 4ch VQA only */
//#define MAX_CHAN		4
#define MAX_CHAN		1	/* currently testing in single channel */

#ifndef _MSC_VER
#define VED_LOWDELAY_ON 1	/* enable Low delay ISR */
#else
#define VED_LOWDELAY_ON 0
#endif
#define DTMF_20ms		1
#define AEC_10DB        1
/* define elements in the feature group */
#define VAD				1
#define ANR_20ms		1
#define ANR_S			1
#define ANR_R			1
#define ALC_S			1
#define ALC_R			1
#define DLC_S			0
#define DLC_R			0
#define AEC_R			1
#define EVI_S			1
#define DNC_S			0
#define TDR				1

#define HEC_S           0
#define HEC_R           0
#define PF_S			0
#define PF_R			0
#define NS_S			0
#define NS_R			0

#define EVI_R			1

#define VADPP_S			1
#define VADPP_R			1

#define DNC_R			0
#define DTX_S			0
#define DTX_R			0

#define AEC_S			1

#define MD              1
#define VQM				0
#define SHARE_HEC_TDR	1

#define VQMON			1
#define SDT				1
#define ASCD			1
#define OWSD            1
#define MOH				1

//#endif
#endif // _VQA_CONST_H
