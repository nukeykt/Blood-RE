/**********************************************************************
   module: INTERRUP.H


   author: James R. Dos‚
   phone:  (214)-271-1365 Ext #221
   date:   March 31, 1994


   Inline functions for disabling and restoring the interrupt flag.

   (c) Copyright 1994 James R. Dos‚.  All Rights Reserved.
**********************************************************************/



#ifndef __INTERRUPT_H
#define __INTERRUPT_H


unsigned long DisableInterrupts( void );
void          RestoreInterrupts( unsigned long flags );


#pragma aux DisableInterrupts = \
   "pushfd",                    \
   "pop    eax",                \
   "cli"                        \
   modify [ eax ];

#pragma aux RestoreInterrupts = \
   "push   eax",                \
   "popfd"                      \
   parm [ eax ];

#endif

