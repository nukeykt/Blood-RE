/*
	TENUtils.h
        
	Copyright 1995-1996 T. E. Network, Inc. All rights reserved.
        
	Contains common utilities.
        
*/

#ifndef TENUTILS
#define TENUTILS


/* Constants */

#define eStringTooSmall 3701
#define eStringBadFormat 3702
#define eStringNameNotFound 3703
#define eNotEnoughMemory 3704


/* Prototypes */

#ifdef __cplusplus
extern "C"
{
#endif

void tenUtInit(void);
void tenUtExit(void);
int tenUtAddStringParam(char *buf, size_t bufSize, const char *name,
        const char *value);
int tenUtGetStringParam(const char *buf, const char *name, char *dst,
        size_t dstSize);
int tenUtDeleteStringParam(char *buf, const char *name);
int tenUtNextStringParam(const char **loc);
int tenUtReplaceStringParam(char *buf, size_t bufSize, const char *newParams);
void tenUtTimeString(time_t time, char *buf, size_t size);
int tenUtInet_aton(unsigned long *addrNet, char *addrStr);
int tenUtStrcasecmp(const char *s1, const char *s2);
int tenUtStrncasecmp(const char *s1, const char *s2, size_t n);
char *tenUtStrlwr(char *s);

/*
**      Converts an Internet IP address from the dotted string format to the network binary value
**              i.e. 127.0.0.1 -> 0x7f000001
**
**      Returns 0                       upon successful completion
**                      otherwise,      the error code encountered
**
**      addrNet Value to hold the 32-bit Internet address
**      addrStr Character string in dotted-decimal notation
*/

int tenUtInet_ntoa(char *addrStr, size_t addrStrLen, unsigned long addrNet);
/*
**      Converts a 32-bit Internet address, network binary format, to dotted-decimal notation
**              i.e. 0x7f000001 -> 127.0.0.1
**
**      Returns 0                       upon successful completion
**                      otherwise,      the error code encountered
**
**      addrStr         Buffer to hold the string in dotted-decimal notation
**      addrStrLen      The length of the addrStr buffer
**      addrNet         The 32-bit Internet address
*/

void tenUtSleep(unsigned long msecs);
unsigned long tenUtCurMsecs(void);           // returns time in milliseconds

char *pwd(char *pathbuff,int bufflen);

int envIsWin();

char *getWinPath(char *buff,int bufflen);

#if ((!defined(DOS))&&(!defined(UNIX))&&(!defined(WIN32))&&(!defined(__WATCOMC__))&&(!defined(MSDOS)))
int spawnvp(int, char *, char **);
#endif

#ifdef __cplusplus
}
#endif

// some macros to smooth the name changes
#define initTENUtils	tenUtInit
#define exitTENUtils	tenUtExit
#define addStringParam  tenUtAddStringParam
#define getStringParam  tenUtGetStringParam
#define ten_inet_aton 	tenUtInet_aton
#define ten_strncasecmp tenUtStrncasecmp
#define ten_strcasecmp 	tenUtStrcasecmp
#define ten_strlwr	tenUtStrlwr
#define teninet_ntoa	tenUtInet_ntoa
#define tenSleep	tenUtSleep
#define curMsecs	tenUtCurMsecs

#endif /* TENUTILS */
