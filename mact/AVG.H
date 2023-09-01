#ifndef __avg_public__
#define __avg_public__
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
   {
   int32 * values;
   int32 index;
   int32 sum;
   int32 average;
   int32 shift;
   int32 base;
   } avg_t;

// numvalues = number of items to average
// each element is set to initvalue
avg_t * AVG_Init( int32 numvalues, int32 initvalue );
void        AVG_Free( avg_t * avg );
// value is the new value entered into the table
// returns the current average after inserting the new value
int32       AVG_Value( avg_t * avg, int32 value );
// returns the current average
int32       AVG_Average( avg_t * avg );
// returns the current average in floating point (much more precise)
float32     AVG_FloatAverage( avg_t * avg );

#ifdef __cplusplus
};
#endif
#endif
