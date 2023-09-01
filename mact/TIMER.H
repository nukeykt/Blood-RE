//////////////////////////////////////////////////////////////////////////////
//
// timer.h  -- Public header for time.c
//
// Simple timer functions.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __timer_h
#define __timer_h
#ifdef __cplusplus
extern "C" {
#endif

#define NO_MORE_TIMERS -1

int  TIME_AddTimer( int TicksPerSecond, volatile int32 *Counter );
void TIME_RemoveTimer( int which );

#ifdef __cplusplus
};
#endif
#endif /* __timer_h */
