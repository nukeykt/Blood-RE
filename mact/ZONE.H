//***************************************************************************
//
//    Zone Memory management Constants
//
//***************************************************************************


#ifndef _zone_public
#define _zone_public
#ifdef __cplusplus
extern "C" {
#endif

// tags < 100 are not overwritten until freed
#define PU_STATIC               1   // static entire execution time
#define PU_GAME                 20  // static until game completed
#define PU_LEVELSTRUCT          49  // start of static until level exited
#define PU_LEVEL                50  // start of static until level exited
#define PU_LEVELEND             51  // end of static until level exited

// tags >= 100 are purgable whenever needed
#define PU_PURGELEVEL           100
#define PU_CACHE                101
#define PU_CACHEWALLS           155
#define PU_CACHESPRITES         154
#define PU_CACHEBJWEAP          153
#define PU_CACHEACTORS          152
#define PU_CACHESOUNDS          120
#define PU_FLAT                 102
#define PU_PATCH                103
#define PU_TEXTURE              104

#define URGENTLEVELSTART        PU_LEVEL


//***************************************************************************
//
//    ZONE.C - Carmack's Memory manager for protected mode
//
//***************************************************************************

int32 Z_Init( int32 size );                      // Allocates a zone of memory
void  Z_ShutDown( void );                        // Shutdown zone memory
int32 Z_GetSize( void *ptr);                     // Get size of memory pointed to
void  Z_Free( void *ptr );                       // Free a pointer in Z_Zone's domain
void  Z_PurgeZone( int32 zonenum );               // Releases all tags in a zone
void *Z_Malloc( int32 size, int32 tag, int32 zone, void *user ); // Malloc You can pass a NULL user if the tag is < PU_PURGELEVEL
void  Z_FreeTags( int32 zonenum, int32 lowtag, int32 hightag );  // Free a series of memory tags
void  Z_Realloc( void **ptr, int32 newsize );    // reallocate memory block
void  Z_ChangeTag( void *ptr, int32 tag );       // Change the tag of a memory item
int32 Z_ZoneSize( int32 zonenum );               // Return the total heap size
int32 Z_ZoneUsed( int32 zonenum );               // Return used portion of heap size
int32 Z_ZoneMaxFree( int32 zonenum );            // Returns largest available contiguous block in a zone.
int32 Z_AvailHeap( void );                       // Returns largest available contiguous block
void  Z_DumpHeap( int32 lowtag, int32 hightag ); // Dump the heap (for debugging purposes)
void  Z_CheckHeap( void );                       // Check the heap for corruption
void  Z_Touch( void * ptr );                     // move the ptr to the end of the LRU list
void  Z_ShutDownZone( int32 zonenum );           // Free up the memory occupied by a zone

#ifdef __cplusplus
};
#endif
#endif
