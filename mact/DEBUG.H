#ifndef _debug_public
#define _debug_public
#ifdef __cplusplus
extern "C" {
#endif

// DEBUG
//
// DEBUG has two different types of debug messages
// the first is just a normal debug message which can be globally turned on
// or off.
// the second utilizes a debug level which must be individually masked on or
// off by the programmer.  The level is an arbitrary number chosen by the
// programmer.
//
// Each mode uses its own function: the first mode uses Dbg and the second
// uses Debug
//
// In the first mode DBG messages can either be enabled or disabled through:
//
// DBG_EnableGeneralDebug( void )
// DBG_DisableGeneralDebug( void )
//
// In the second mode DEBUG messages are filtered according to a debug mask.
// this debug mask is initialized when DEBUG is started to filter out all
// DEBUG messages.  To enable a certain DEBUG level call
// DBG_EnableDebugLevel( int32 level ) and to disable a certain level call
// DBG_DisableDebugLevel( int32 level ). The entire filter can be cleared with
// DBG_DisableAllDebugLevels( void ).
//
// The output of DEBUG messages are controlled by the function
// DBG_SetDebugOutputMode, the current output mode can be obtained by
// DBG_GetDebugOutputMode.
//
// The output functions supported by DEBUG natively is through printf and
// to a file.  Two other output modes have been left unimplemented,
// dbgout_user1 and dbgout_user2. Output functions can be registered for
// these modes using the function DBG_RegisterOutputFunction which takes
// the mode you want to customize as well as three function pointers:
//
// startup - a void function which starts up the output method
// print - a function which takes a char * which actually does the printing
// shutdown - a void function which shutsdown the output method
//


#define DEBUG 1

#define MAXDEBUGLEVEL 50

typedef enum
   {
   dbgout_screen,
   dbgout_file,
   dbgout_user1,
   dbgout_user2,
   dbgout_endtype
   } debugoutput_t;

void DBG_DebugString( int32 level, char *msg, ... );
void DBG_DbgString( char *msg, ... );

#if ( DEBUG == 1 )
   #define Debug  DBG_DebugString
   #define Dbg    DBG_DbgString
#else
   #define Debug  if (1) {} else DBG_DebugString
   #define Dbg  if (1) {} else DBG_DbgString
#endif

void DBG_EnableGeneralDebug( void );
void DBG_DisableGeneralDebug( void );

void DBG_EnableDebugLevel( int32 level );
void DBG_DisableDebugLevel( int32 level );
void DBG_DisableAllDebugLevels( void );
void DBG_SetDebugOutputMode( int32 mode );
int32 DBG_GetDebugOutputMode( void );
void DBG_Shutdown( void );
void DBG_Startup( int32 mode, int32 level, int32 output );
void DBG_RegisterOutputFunction
   (
   int32 outmode,
   void (*startup) (void),
   void (*print) (char *),
   void (*shutdown) (void)
   );

#ifdef __cplusplus
};
#endif
#endif
