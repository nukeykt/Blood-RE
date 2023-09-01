//***************************************************************************
//
//    RGB.C - Creates a best fit color table for RGB values.
//
//***************************************************************************

#ifndef _rgb_public
#define _rgb_public
#ifdef __cplusplus
extern "C" {
#endif

#define NUM_RED   64
#define NUM_GREEN 64
#define NUM_BLUE  64

#define REAL_RED( r )   ( ( r ) << 2 )
#define REAL_GREEN( g ) ( ( g ) << 2 )
#define REAL_BLUE( b )  ( ( b ) << 2 )

#if    !defined(__MSDOS__) || defined(__FLAT__)
extern byte RGB_Lookup[ NUM_RED ][ NUM_GREEN ][ NUM_BLUE ];
#else
extern byte huge RGB_Lookup[ NUM_RED ][ NUM_GREEN ][ NUM_BLUE ];
#endif
extern byte RGB_EGATable[ 16 ];

#define RGB_GetColor( r, g, b ) \
   ( RGB_Lookup[ ( r ) >> 2 ][ ( g ) >> 2 ][ ( b ) >> 2 ] )

#define RGB_EGAColor( c ) \
   ( RGB_EGATable[ ( c ) ] )

void RGB_FindEGAColors( void );
byte RGB_FindBestColor( int16 r, int16 g, int16 b, byte *palette );
void RGB_GetPalette( byte *pal );
byte RGB_GetBestColor( int16 r, int16 g, int16 b );
void RGB_MakeColorTable( void );
#ifdef __cplusplus
};
#endif
#endif
