#ifndef _location_public
#define _location_public
#ifdef __cplusplus
extern "C" {
#endif

#if ( LOCATIONINFO == 1 )

#define funcstart() \
   { \
   SoftError( "funcstart : module '%s' at line %d.\n", __FILE__, __LINE__ );
   } \

#define funcend() \
   { \
   SoftError( "  funcend : module '%s' at line %d.\n", __FILE__, __LINE__ );
   } \


#endif

#ifdef __cplusplus
};
#endif
#endif
