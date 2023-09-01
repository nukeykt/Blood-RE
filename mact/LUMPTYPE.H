#ifndef _lumptype_public
#define _lumptype_public
#ifdef __cplusplus
extern "C" {
#endif

#define MAXLUMP      0x80000 // biggest possible lump (256k)
#define MAXMIPMAPLEVELS  8
#define MAXMIPMAPLEVELS2 4
#define MAX_TEXTURE_NAME_LENGTH 16
#define MAX_PARENT_MODEL_NAME_LENGTH 16

typedef enum
   {
   lumptype_unknown,
   lumptype_pic,
   lumptype_lpic,
   lumptype_post,
   lumptype_post_raw,
   lumptype_raw,
   lumptype_patch255,
   lumptype_patch,
   lumptype_palette,
   lumptype_patch255_all,
   lumptype_patch_all,
   lumptype_mipmap,
   lumptype_font,
   lumptype_color_font,
   lumptype_intensity_font,
   lumptype_sound,
   lumptype_label,
   lumptype_colormap,
   lumptype_transmap,
   lumptype_tileset,
   lumptype_model,
   lumptype_modelframe
   } lumptype_t;

typedef struct
   {
   int16 width,height;
   byte  data;
   } pic_t;

typedef struct
   {
   int16 width,height;
   int16 orgx,orgy;
   byte  data;
   } lpic_t;

typedef struct
   {
   int16 width,height;
   byte  data;
   } post_t;

typedef struct
   {
   byte  height;
   byte  width[256];
   int16 charofs[256];
   byte  data;       // as much as required
   } font_t;

typedef struct
   {
   byte  height;
   byte  width[256];
   int16 charofs[256];
   byte  pal[0x300];
   byte  data;       // as much as required
   } cfont_t;

typedef struct
   {
   byte  height;
   byte  width[256];
   int16 charofs[256];
   byte  numlevels;
   byte  data;       // as much as required
   } ifont_t;

typedef struct
   {
   byte origx;         // the orig width and height
   byte origy;
   byte width;            // bounding box size
   byte height;
   byte leftoffset;       // pixels to the left of origin
   byte topoffset;        // pixels above the origin
   int16 collumnofs[256]; // only [width] used, the [0] is &collumnofs[width]
   } patch_t;

typedef struct
   {
   int16 width;
   int16 height;
   int32 numlevels;
   int32 mapofs[MAXMIPMAPLEVELS];
   } mipmap_t;

typedef struct
   {
   char name[16];
   uint32 width;
   uint32 height;
   uint32 mapofs[MAXMIPMAPLEVELS2];
   } mipmap2_t;

typedef struct
   {
   boolean reverse;
   byte numlevels;
   byte numdark;
   byte data;
   } colormap_t;

typedef struct
   {
   byte weight;
   byte startcolor;
   byte stopcolor;
   byte data;
   } transmap_t;

typedef struct
   {
   word numtiles;
   byte tilewidth;
   byte tileheight;
   byte data;
   } tileset_t;

#define MODELNORMALIZESCALE 32767
#define TEXTURENORMALIZESCALE 65535
typedef int16 wadvalue_t;
typedef uint16 uwadvalue_t;
typedef uint16 wadtex_t;
typedef struct
   {
   wadvalue_t x;
   wadvalue_t y;
   wadvalue_t z;
   wadvalue_t extra;
   }wadvertex_t;

typedef struct
   {
   wadvalue_t x,y,z,extra;
   }wadvector_t;

typedef struct
   {
   wadvalue_t x,y,z;
   wadvalue_t radius;
   }wadsphere_t;

typedef struct
   {
   wadvalue_t x,y,z,sdist;
   }wadplane_t;

typedef struct
   {
   uwadvalue_t num;
   wadtex_t u;
   wadtex_t v;
   wadtex_t extra;
   }wadpolyvertex_t;
typedef struct
   {
   wadpolyvertex_t vertex[3];
	}wadpoly_t;

typedef struct
   {
   uwadvalue_t numpolys;
   uwadvalue_t polystart;
	}wadpart_t;

typedef struct
   {
   byte         parentmodel[MAX_PARENT_MODEL_NAME_LENGTH];
   byte         texturename[MAX_TEXTURE_NAME_LENGTH];
   uint32       partspheresoffset;
   uint32       verticesoffset;
   uint32       normalsoffset;
   }wadframe_t;

typedef struct
   {
   int32        numvertices;
   int32        numparts;
   int32        numpolygons;
   uint32       polygonsoffset;
   uint32       partsoffset;
   }wadmodel_t;

#ifdef __cplusplus
};
#endif
#endif
