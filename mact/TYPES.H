#ifndef _types_public
#define _types_public
#ifdef __cplusplus
extern "C" {
#endif


//***************************************************************************
//
//  Global Data Types (For portability)
//
//***************************************************************************

typedef unsigned char           uint8;
typedef uint8                   byte;
typedef signed char             int8;

typedef unsigned short int      uint16;
typedef uint16                  word;
typedef short int               int16;

typedef unsigned long           uint32;
typedef long                    int32;
typedef uint32                  dword;

typedef int32                   fixed;
typedef int32                   boolean;
typedef float                   float32;
typedef double                  float64;
typedef long double             float128;
typedef float64                 appfloat;
typedef uint32                  memhandle;

#define MAXINT32                0x7fffffff
#define MININT32                -0x80000000
#define MAXUINT32               0xffffffff
#define MINUINT32               0

#define MAXINT16                0x7fff
#define MININT16                -0x8000
#define MAXUINT16               0xffff
#define MINUINT16               0

//***************************************************************************
//
//  boolean values
//
//***************************************************************************

#define true ( 1 == 1 )
#define false ( ! true )

//***************************************************************************
//
//  BYTE ACCESS MACROS
//
//***************************************************************************

// WORD macros
#define Int16_HighByte( x ) ( (uint8) ((x)>>8) )
#define Int16_LowByte( x )  ( (uint8) ((x)&0xff) )

// DWORD macros
#define Int32_4Byte( x )   ( (uint8) ((x)>>24)&0xff )
#define Int32_3Byte( x )   ( (uint8) (((x)>>16)&0xff) )
#define Int32_2Byte( x )   ( (uint8) (((x)>>8)&0xff) )
#define Int32_1Byte( x )   ( (uint8) ((x)&0xff) )

#ifdef __NeXT__
#define stricmp strcasecmp
#define strcmpi strcasecmp
#endif

#ifdef __cplusplus
};
#endif
#endif
