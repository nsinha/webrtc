/*==================================================================*/
/*								                                    */
/*      Adaptive Noise Reduction     ANSI-C Source Code				*/
/*																	*/
/*      Version 1.0													*/
/*      Copyright (C) 2003, Ditech Communications Corportation		*/
/*      All rights reserved.										*/
/*										                            */
/*																	*/
/*		Created by: Chunyan Li @ Nov. 2002							*/
/*==================================================================*/

#ifndef  __ANR_MATH_H
#define  __ANR_MATH_H

#include "mpu_types.h"

/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/

Word32 fnLog2(Word32 L_var1);
Word32 fnLog(Word32 L_var1);
Word32 fnLog10(Word32 L_var1);		// about 47 cycles
Word32 fnExp2(Word32 L_var1);
Word32 fnExp10(Word32 L_var1);		// about 22 cycles

Word32 L_mpy_ls(Word32 L_var2, Word16 var1);

#endif
