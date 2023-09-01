#ifndef __rox_public__
#define __rox_public__
#ifdef __cplusplus
extern "C" {
#endif

void ROX_Startup ( void );
void ROX_Shutdown ( void );

/*
====================
=
= ROX_SetDataDir
=
= sets the subdirectory used by all ROX files
=
====================
*/

void ROX_SetDataDir ( char * dirname );

/*
====================
=
= ROX_Open
=
= returns a handle to the requested file
=
====================
*/

int32 ROX_Open
   (
   const char * name
   );

/*
====================
=
= ROX_Close
=
= Close a file opened by ROX_Open takes a handle
=
====================
*/

void ROX_Close
   (
   int32 handle
   );

/*
====================
=
= ROX_Length
=
= get file length of a file denoted by a handle
=
====================
*/

int32 ROX_Length
   (
   int32 handle
   );

/*
====================
=
= ROX_Tell
=
= get position of a file denoted by a handle
=
====================
*/

int32 ROX_Tell
   (
   int32 handle
   );

/*
====================
=
= ROX_Seek
=
= set the position of a file denoted by a handle
=
====================
*/

void ROX_Seek
   (
   int32 handle,
   int32 position
   );

/*
====================
=
= ROX_Read
=
= read in some data from the file denoted by a handle
=
====================
*/

void ROX_Read
   (
   int32 handle,
   void * buf,
   int32 size
   );

/*
====================
=
= ROX_Write
=
= write out some data from the file denoted by a handle
=
====================
*/

void ROX_Write
   (
   int32 handle,
   void * buf,
   int32 size
   );

/*
====================
=
= ROX_Cache
=
= cache a specific data file, the name is the pathname of the data
= directories are separated by either '\' or '/', it is your preference
= The delimiter will be properly resolved for your system
= zonenum refers to the memory zone in which you would like this piece of
= data to exist.
= tag refers to the memory tag to give the piece of data (refer to zone.h)
=
====================
*/

void * ROX_Cache
   (
   const char * name,
   int32 zonenum,
   int32 tag
   );

/*
====================
=
= ROX_Add
=
= add a file to the internal tables
=
====================
*/
void ROX_Add
   (
   const char * name
   );

/*
====================
=
= ROX_Dump
=
= extract all files from a ROX file
=
====================
*/
void ROX_Dump
   (
   const char * name,
   boolean quiet
   );

//========================================================================
//
// ROX Creation
//
//========================================================================


/*
=================
=
= ROX_Create
=
= creates a ROX file header
= should be called before subsequent calls to ROX_WriteFile
=
=================
*/

void ROX_Create (const char * filename, boolean quiet);

/*
=================
=
= ROX_WriteFile
=
= Save out a file to a ROX file
=
=================
*/

void ROX_WriteFile
   (
   byte * data,
   int32 size,
   const char * name,
   boolean quiet
   );

/*
=================
=
= ROX_FilesInROX
=
= returns number of files in ROX dir
=
=================
*/

int32 ROX_FilesInROX ( void );

/*
=================
=
= ROX_CloseROXFile
=
= Closes a rox file being written to.
=
=================
*/

void ROX_CloseROXFile ( boolean quiet );

#ifdef __cplusplus
};
#endif
#endif
