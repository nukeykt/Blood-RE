#ifndef __mouse_h
#define __mouse_h
#ifdef __cplusplus
extern "C" {
#endif

#define LEFT_MOUSE   1
#define RIGHT_MOUSE  2
#define MIDDLE_MOUSE 4
#define LEFT_MOUSE_PRESSED( button ) ( ( ( button ) & LEFT_MOUSE ) != 0 )
#define RIGHT_MOUSE_PRESSED( button ) ( ( ( button ) & RIGHT_MOUSE ) != 0 )
#define MIDDLE_MOUSE_PRESSED( button ) ( ( ( button ) & MIDDLE_MOUSE ) != 0 )

boolean MOUSE_Init( void );
void    MOUSE_Shutdown( void );
void    MOUSE_ShowCursor( void );
void    MOUSE_HideCursor( void );
int32   MOUSE_GetButtons( void );
void    MOUSE_GetPosition( int32*x, int32*y  );
void    MOUSE_GetDelta( int32*x, int32*y  );
void    MOUSE_SetCapture( void  );
void    MOUSE_ReleaseCapture( void  );

#ifdef __cplusplus
};
#endif
#endif /* __mouse_h */
