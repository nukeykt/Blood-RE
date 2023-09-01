//***************************************************************************
//
//    UTIL_LIB.C - various utils
//
//***************************************************************************

#ifndef _util_lib_public
#define _util_lib_public
#ifdef __cplusplus
extern "C" {
#endif

#if (defined(__MSDOS__) && !defined(__FLAT__))
extern  int16    _argc;
#else
extern  int32    _argc;
#endif
extern  char **  _argv;

#ifdef __cplusplus
void RegisterShutdownFunction( void shutdown(void) );
#else
void RegisterShutdownFunction( void (* shutdown) (void) );
#endif

void   Error (char *error, ...);

char   CheckParm (char *check);

void   *SafeMalloc (int32 size);
int32  SafeMallocSize (void * ptr);
void   SafeFree (void * ptr);
void   SafeRealloc (void ** ptr, int32 newsize);
int32  ParseHex (char *hex);
int32  ParseNum (char *str);
int16  MotoShort (int16 l);
int16  IntelShort (int16 l);
int32  MotoLong (int32 l);
int32  IntelLong (int32 l);

void HeapSort(char * base, int32 nel, int32 width, int32 (*compare)(), void (*switcher)());

#ifdef __cplusplus
};
#endif
#endif
