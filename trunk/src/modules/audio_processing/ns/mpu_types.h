/********************************************************************************
 * File			:		mpu_types.h
 * Written by	:		Naveen
 * Description	:		This include file defines several type definitions
 * ******************************************************************************/

#ifndef MPU_TYPES_H_
#define MPU_TYPES_H_
//#include <rpc/types.h>

// Generic type definitions here
typedef short int 			INT16;
typedef unsigned short int 	UINT16;
typedef long int 	 		INT32;
typedef unsigned long int 	UINT32;
typedef char INT8;
typedef unsigned char UINT8;
typedef void Void;
typedef float		float32;
typedef double		float64;


#ifndef TRUE1
#define FALSE1 ((long)0)
#define TRUE1  ((long)1)
#endif

typedef unsigned short UWord16;
typedef short	Word16;

typedef unsigned long UWord32;
typedef long	Word32;

typedef unsigned short int 	uint16;
typedef short int int16;
typedef long int  int32;
typedef unsigned int	uint32;
typedef unsigned char	uint8;
typedef char int8;
typedef float FLOAT;

#define F  float

# define __BIGSTACKVARIABLE__

//typedef enum {FALSE, TRUE} BOOL;
//typedef bool bool;

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

typedef union anytype_s {
    void *ptr;
    long i;
    unsigned long ui;
    double fl;
} anytype_t;

#define	MAX_UINT32			0xFFFFFFFFL
#define	MAX_INT32			0x7FFFFFFFL
#define	MAX_INT16			0x7FFF
#define	MAX_UINT16			0xFFFF
#define	MAX_INT8			0x7F
#define	MAX_UINT8			0xFF

#define MAX_NEG_INT32		((int32) 0x80000000)
#define MAX_NEG_INT16		((int16) 0xffff8000)
#define MAX_NEG_INT8		((int8)  0xffffff80)

/* The following are approximate; IEEE floating point standards might quibble! */
#define MAX_POS_FLOAT32		3.4e+38f
#define MIN_POS_FLOAT32		1.2e-38f	/* But not 0 */
#define MAX_POS_FLOAT64		1.8e+307
#define MIN_POS_FLOAT64		2.2e-308

#define MAX_IEEE_NORM_POS_FLOAT32        3.4e+38f
#define MIN_IEEE_NORM_POS_FLOAT32        1.2e-38f
#define MIN_IEEE_NORM_NEG_FLOAT32       -3.4e+38f
#define MAX_IEEE_NORM_POS_FLOAT64        1.8e+307
#define MIN_IEEE_NORM_POS_FLOAT64        2.2e-308
#define MIN_IEEE_NORM_NEG_FLOAT64       -1.8e+307

/* Will the following really work?? */
#define MAX_NEG_FLOAT32		((float32) (-MAX_POS_FLOAT32))
#define MIN_NEG_FLOAT32		((float32) (-MIN_POS_FLOAT32))
#define MAX_NEG_FLOAT64		((float64) (-MAX_POS_FLOAT64))
#define MIN_NEG_FLOAT64		((float64) (-MIN_POS_FLOAT64))

/* 
 * The type "ERROR" below can be used by function prototypes to return a variable of this enum type
 * Define all system level errors here. 
 * 1001 is a generic system error. All the system level errors should fall between 1001 to 1999
 * What can be other kinds of system errors?
 * May be thread creation fails, mutex init fails etc...
 */
enum ERROR {
	MPU_SUCCESS = 0,
	MPU_ERROR	= 1001,					// Generic system error
	MPU_THREAD_CREATE_ERROR,
	MPU_MUTEX_INIT_ERROR,
	MPU_COND_VAR_INIT_ERROR,
	MPU_QUE_INIT_ERROR,
	MPU_QUE_CREATE_ERROR,
	MPU_QUE_APPEND_ERROR,
	MPU_QUE_INSERT_AFTER_ERROR,
	MPU_QUE_TIME_ORDER_INSERT_ERROR,
	MPU_QUE_DESTROY_ERROR,
	MP_MEMALLOC_ERROR,
	MP_MEMFREE_ERROR,
	MP_MEM_ALREADY_INITIALIZED,

	/* DTMF module (relay and generator/detector) error codes */
	DTMF_RELAY_SUCCESS = 0,
	DTMF_RELAY_MUTEX_INIT_ERROR = 1101,
	DTMF_RELAY_MUTEX_LOCK_ERROR,
	DTMF_RELAY_MUTEX_UNLOCK_ERROR,

	/* RTP related error codes */
	RTP_SUCCESS = 0,
	RTP_VERSION_ERROR = 1201,
	RTP_PBIT_ERROR,
	RTP_XBIT_ERROR,
	RTP_CC_ERROR,
	RTP_SEQ_ERROR,
	RTP_PAYLOAD_TYPE_ERROR,
	RTP_PAYLOAD_LENGTH_ERROR,
	RTP_PAYLOAD_PACK_ERROR,	// too many frames are packed in one packet
	RTP_SETUP_ERROR,
	RTP_ERROR
};

// Global codecs enumeration
typedef enum
{
    G711,
    G729,
    G729D,
    G729E,
    G723_1,
    G726,
    T38,
    ILBC,
    G722,
    G722_1,
    GSM_AMR_WB, /* wide band GSM AMR codec, also known as G722.2 */
    GSM_EFR,
    GSM_FR,
    GSM_HR,
    GSM_AMR,
    EVRC,
    SMV,
    ISAC,
    SPEEX,
    NUM_CODECS
} CODEC_TYPE;

// Codec rate (applicable only for multi-rate codecs)
typedef enum
{
    DEFAULT_RATE = 0,
    G722_64 = 0, G722_56, G722_48,
    G723_6_4 = 0, G723_5_3,
    GSM_AMR_4_75 = 0, GSM_AMR_5_15, GSM_AMR_5_90, GSM_AMR_6_70, GSM_AMR_7_40, GSM_AMR_7_95, GSM_AMR_10_2, GSM_AMR_12_2
} CODEC_RATE;

// Identify the current playout frame
typedef enum
{
    // !!! WARNING !!! Do not change NOISE, ACTIVE, SID enumeration values - they are required by codec
    NOISE, ACTIVE, SID, TONE_EVENT, TONE_EVENT_END, TONE_EVENT_BEGIN, FIRST_NOISE, ZERO_FRAME
} JB_OUTPUT_FRAME_TYPE;

#endif /*MPU_TYPES_H_*/
