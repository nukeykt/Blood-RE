#ifndef __timelib_public__
#define __timelib_public__
#ifdef __cplusplus
extern "C" {
#endif

/*
================
TIMELIB_CreateTimer
Create a new timer
================
*/
int32 TIMELIB_CreateTimer (void);

/*
================
TIMELIB_DestroyTimer
Destroy a timer
================
*/
void TIMELIB_DestroyTimer ( int32 handle );

/*
================
TIMELIB_StartTimer
Start a timer
================
*/
void TIMELIB_StartTimer ( int32 handle );

/*
================
TIMELIB_StopTimer
Stop a timer
================
*/
void TIMELIB_StopTimer ( int32 handle );

/*
================
TIMELIB_GetTimer
get the current time of a timer
================
*/
float32 TIMELIB_GetTimer ( int32 handle );

/*
================
TIMELIB_StartTiming
simple stop watch start
================
*/
void TIMELIB_StartTiming ( void );

/*
================
TIMELIB_StopTiming
simple stop watch stop
================
*/
float32 TIMELIB_StopTiming ( void );

#ifdef __cplusplus
};
#endif
#endif
