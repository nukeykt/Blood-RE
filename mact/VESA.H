#ifndef __vesa_h__
#define __vesa_h__

extern int VESA_bytesperline;      /* Bytes per scanline for mode      */

byte *VESA_SetMode( int32 width, int32 height );
void VESA_TextMode(void);
void VESA_Shutdown( void );

#endif /* __vesa_h__ */
