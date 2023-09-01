/*
	DebugLib.h

	Copyright (c) 1995 TENetwork, Inc. All rights reserved.
        
	Contains shared prototypes and definitions used by ANSIUtil.c.

	Cross-platform portable.
*/

#ifndef DEBUGLIB
#define DEBUGLIB

/* Constants */

#define kDebugInfo 1
#define kDebugError 2
#define kDebugFatal 3

/* Flags */
#define kDBThrowException 0x01
#define kDBSetNewHandler 0x02

#define kDoVerifications    /* when you want extra debugging, define this */

#ifdef __cplusplus

class tenDbCVerifyException
{
public:
	long line;
	char file[64];
	char expr[128];
	int err;			// if nonzero, error code. Otherwise, assert failure.
	char text[256];

	tenDbCVerifyException();
};

#endif


/* Prototypes */

#ifdef __cplusplus
extern "C"
{
#endif

void tenDbInit(unsigned long flags);
void tenDbExit(void);
typedef void (*tenDbExitFn)(void);
typedef void (*tenDbMsgFn)(int msgLevel, char *msg);
void tenDbSetExitRoutine(tenDbExitFn);
void tenDbSetDebugMsgRoutine(tenDbMsgFn);
#ifdef __cplusplus
void tenDbSetExceptionPrehandleRoutine(int (*exFn) (tenDbCVerifyException e));
#endif
void tenDbPrintf(const char *format, ...);
void *tenDbOpenLogFile(char *name, int level);
void tenDbCloseLogFile(void *logRef);
void tenDbLimitLogFile(void *logRef, long numLines);
void tenDbFlushLog(void *logRef);
void tenDbUseRelativeTime(void *logRef);
void tenDbLprintf(void *logRef, int level, const char *format, ...);
int tenDbVerifyFailed(char *expr, char *file, long line);
int tenDbVerifyNoErrCore(int err, char *expr, char *file, long line);
char *tenDbErrstr(int err);
#ifdef __cplusplus
}
#endif

// fix some renamed stuff for reverse compatibility.

#define CVerifyException tenDbCVerifyException
//#define dprintf tenDbPrintf
#define initDebug tenDbInit
#define exitDebug tenDbExit
#define exitFn	tenDbExitFn
#define msgFn tenDbMsgFn
#define setExitRoutine	tenDbSetExitRoutine
#define setDebugMsgRoutine tenDbSetDebugMsgRoutine
#define setExceptionPrehandleRoutine tenDbSetExceptionPrehandleRoutine
#define verifyFailed tenDbVerifyFailed
#define verifyNoErrCore tenDbVerifyNoErrCore
#define verify tenDbVerify
#define verifyNoErr tenDbVerifyNoErr

/* Other Macros */

#ifdef kDoVerifications
#define tenDbVerify(x) ((x) ? 1 : tenDbVerifyFailed(#x, __FILE__, __LINE__))
#define tenDbVerifyNoErr(x) (tenDbVerifyNoErrCore(x, #x, __FILE__, __LINE__))
#else
#define tenDbVerify(x)
#define tenDbVerifyNoErr(x) (x)
#endif /* kDoVerifications */


#endif  /* DEBUGLIB */

