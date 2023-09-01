/////////////////////////////////////////////////////////////////////////////
//
//      LBMLIB.H
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _lbmlib_public_
#define _lbmlib_public_
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
   {
   LoadLBM_Okay,
   LoadLBM_badsize,
   LoadLBM_stencil
   } LoadLBM_Status;

int32 LoadLBM
   (
   byte * compressed,
   byte * uncompressed,
   byte * palette,
   int32 * lbmwidth,
   int32 * lbmheight
   );

int32 SaveLBM
   (
   byte * uncompressed,
   byte * compressed,
   byte * palette,
   int32 lbmwidth,
   int32 lbmheight
   );
#ifdef __cplusplus
};
#endif
#endif
