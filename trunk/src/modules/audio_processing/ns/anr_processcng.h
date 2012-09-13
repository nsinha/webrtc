#ifndef _ANR_PROCESSCNG_H
#define	_ANR_PROCESSCNG_H

#include	"anr_vad.h"

short	ANR_process_netCNG(
ANR_STATE *pANRState, 
Word16		vad_flag,
Word32	*framePwr,
Word16	*EdB
); 

#endif
