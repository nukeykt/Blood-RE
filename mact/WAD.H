//***************************************************************************
//
//    WAD.C - Wad managment utilities
//
//***************************************************************************

#ifndef _wad_public
#define _wad_public
#ifdef __cplusplus
extern "C" {
#endif

#define WAD_LUMP_NOT_FOUND ((dword)0xffffffff )

word    WAD_GetWADCRC( void );                     // Calculates the CRC of the wad info

void    WAD_InitMultipleFiles( const char **filenames ); // Initialize multiple wads
void    WAD_InitFile( const char *filename );            // Init a single wad file

dword   WAD_CheckNumForName( const char *name );         // Check to see if the named lump exists
dword   WAD_GetNumForName( const char *name );           // Get the number for the named lump
char   *WAD_GetNameForNum( dword lump );           // Get the name for a number
int32   WAD_GetTypeForNum( dword lump );           // Get the type of the named lump
int32   WAD_GetTypeForName( const char *name );
         // Get the name of a numbered lump
int32   WAD_CountType( int32 lumptype );           // Get the number of lumps of the specified type

int32   WAD_GetLumpNumsOfType( int32 lumptype, dword **arrayptr );
// Allocates an array containing the lump numbers of all lumps with the
// specified lumptype.  Last element of the array is -1.  Array should
// be freed after it's done being used.

dword   WAD_NumLumps( void );                      // Get the current number of lumps managed
dword   WAD_LumpLength( dword lump );              // Get the length of the numbered lump
void    WAD_ReadLump( dword lump, void *dest );    // Read the numbered lump into a buffer
void    WAD_WriteLump( dword lump, void *src );

void   *WAD_CacheLumpNum( dword lump, int32 zone, int32 tag );             // Cache in the numbered lump with the appropriate memory tag
void   *WAD_CacheLumpName( const char *name, int32 zone, int32 tag );            // Cache in the named lump with the appropriate memory tag
void   *WAD_CacheLumpDec( const char *name, int32 num, int32 zone, int32 tag );  // Cache in the named lump with decimal the appropriate memory tag
void   *WAD_CacheLumpHex( const char *name, int32 num, int32 zone, int32 tag );  // Cache in the named lump with hexidecimal the appropriate memory tag

//void   WAD_OpenWadFile ( const char * filename, boolean quiet);	// old mact 5/31
void   WAD_OpenWadFile (const char * filename, boolean quiet );		// new mact 7/30

void WAD_WriteWadLump
   (
   byte * data,
   int32 size,
   const char * name,
   int32 lumptype,
   boolean quiet
   );
void   WAD_CloseWadFile ( boolean quiet );
void   WAD_ShootWAD( const char *filename, boolean quiet );
int32  WAD_LumpsInWad ( void );
char * WAD_LumptypeToString ( int32 lumptype );

#ifdef __cplusplus
};
#endif
#endif
