/////////////////////////////////////////////////////////////////////////////
//
//      GIFLIB.H
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _giflib_public_
#define _giflib_public_
#ifdef __cplusplus
extern "C" {
#endif

void SaveGIF
   (
   char * filename,
   byte * uncompressed,
   byte * palette,
   int32 width,
   int32 height
   );

#ifdef __cplusplus
};
#endif
#endif
