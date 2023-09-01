#ifndef _mipmap_public_
#define _mipmap_public_
#ifdef __cplusplus
extern "C" {
#endif

int32 CreateMipMap
   (
   int32 origw,
   int32 origh,
   int32 level,
   byte * srcbitmap,
   byte * destbitmap,
   byte * pal,
   boolean usediffusion
   );

#ifdef __cplusplus
};
#endif
#endif
