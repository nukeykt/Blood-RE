#ifndef _clrmap_public_
#define _clrmap_public_
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
   {
   double r,g,b;
   } rgb_t;

extern byte egacolor[16];

#define ColorLevel(colormap,color,level) \
           (  \
              *(\
              colormap + \
              ( (level) << (8) ) + \
              color \
              )\
           )
#define CMAP_EGAColor(which) \
    ( \
	 egacolor[which] \
    )
/*
===============
=
= CMAP_BestColor
=
===============
*/

byte CMAP_BestColor
   (
   double r,
   double g,
   double b,
   byte *palette,
   int32 low,
   int32 high
   );

/*
=====================
=
= CMAP_CalculateTranslucentMap
=
=
= weight = 0..255 translucent weighting
=====================
*/

int32 CMAP_CalculateTranslucentMap
   (
   byte  *colormap,
   byte  * pal,
   byte    weight,
   byte    startcolor,
   byte    stopcolor,
   byte    lowcolor,
   byte    highcolor
   );

/*
=====================
=
= CMAP_CalculateColorMap
=
= reverse order == false
=   0           = is source color
=   numlevels-1 = is dest color
=
= reverse order == true
=   numlevels-1 = is source color
=   0           = is dest color
=
= weight = 0..255 how close to dest shoule we get
=====================
*/

void CMAP_CalculateColorMap
   (
   byte  *colormap,
   byte  * pal,
   byte    numlevels,
   rgb_t * dest,
   byte    weight,
   byte    startcolor,
   byte    stopcolor,
   byte    lowcolor,
   byte    highcolor,
   boolean reverseorder
   );

/*
=====================
=
= CMAP_BuildColorMap
=
= reverse order == false
=   0           = is low rgb
=   numlevels-1 = is high rgb
=
= reverse order == true
=   numlevels-1 = is low rgb
=   0           = is high rgb
=
= numdark = numdark levels
= darkweight = how close to lowrgb should we get
= lightweight = how close to highrgb should we get
=====================
*/
void CMAP_BuildColorMap
   (
   byte  **colormap,
   byte  * pal,
   byte    numlevels,
   byte    numdark,
   byte    darkweight,
   byte    lightweight,
   rgb_t * lowrgb,
   rgb_t * highrgb,
   byte    startcolor,
   byte    stopcolor,
   byte    lowcolor,
   byte    highcolor,
   boolean reverseorder
   );

/*
=====================
=
= CMAP_BuildMonochromeMap
=
=====================
*/

void CMAP_BuildMonochromeMap
   (
   byte  **monomap,
   byte  * pal,
   byte    red,
   byte    green,
   byte    blue,
   byte    startcolor,
   byte    stopcolor,
   byte    lowcolor,
   byte    highcolor
   );

/*
=====================
=
= CMAP_GetIntensity
=
=====================
*/
byte CMAP_GetIntensity
   (
   byte basecolor,
   byte color,
   byte * pal,
   int32 numlevels
   );
/*
====================
=
= CMAP_FindEGAColors
=
====================
*/

void CMAP_FindEGAColors ( char * palette );

#ifdef __cplusplus
};
#endif
#endif
