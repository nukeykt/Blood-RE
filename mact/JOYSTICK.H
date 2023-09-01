#ifndef _joystick_public
#define _joystick_public
#ifdef __cplusplus
extern "C" {
#endif

//***************************************************************************
//
// Public header for JOYSTICK.ASM.
//
//***************************************************************************

extern int32 Joy_x1;
extern int32 Joy_y1;
extern int32 Joy_x2;
extern int32 Joy_y2;
extern byte  Joy_mask;

void JoyStick_Vals( void );
#if defined(__FLAT__) || defined(__NT__)
#pragma aux JoyStick_Vals modify exact [eax ebx ecx edx esi edi]
#endif

#define JOYSTICK_GetPos( x1, y1, x2, y2 ) \
   { \
   JoyStick_Vals(); \
   *( x1 ) = Joy_x1; \
   *( y1 ) = Joy_y1; \
   *( x2 ) = Joy_x2; \
   *( y2 ) = Joy_y2; \
   }


#ifdef __cplusplus
};
#endif
#endif
