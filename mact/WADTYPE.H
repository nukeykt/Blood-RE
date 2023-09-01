#ifndef _wadtype_public
#define _wadtype_public
#ifdef __cplusplus
extern "C" {
#endif

#define WAD_LUMP_NAME_SIZE 8
#define WADEXTENSION "wad"

#define ID_STRING   "IWAD"
//===============
//   TYPES
//===============

typedef struct
   {
   char    name[ WAD_LUMP_NAME_SIZE ];
   int16   handle;
   int16   lumptype;
   uint32  position;
   uint32  size;
   } lumpinfo_t;

typedef struct
   {
   char    identification[ 4 ]; // should be ID_STRING
   uint32  numlumps;
   uint32  infotableofs;
   } wadinfo_t;

typedef struct
   {
   uint32  filepos;
   uint32  size;
   int32   lumptype;
   char    name[ WAD_LUMP_NAME_SIZE ];
   } filelump_t;


#ifdef __cplusplus
};
#endif
#endif
