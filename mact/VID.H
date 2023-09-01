// vid.h -- video driver defs
#ifndef __vid_h
#define __vid_h
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {disp_bitmap, disp_framebuffer, disp_vesa} display_t;
typedef enum {blit_move, blit_and, blit_or} blit_t;

typedef void pixel_t;
// a pixel can be one, two, or four bytes

typedef byte pixel;

typedef struct
   {
	int x;
	int y;
	int width;
	int height;
	} vrect_t;

typedef struct
   {
   display_t type;
   boolean   installed;
   byte     *framebuffer;
   pixel_t  *buffer;
   pixel_t  *colormap;   // 256*pixbytes*grades size
   int32    *ylookup;
   unsigned  pixbytes;   // 1, 2, or 4
   unsigned  rowbytes;   // may be > width*pixbytes if displayed in a window
   unsigned  screenbytes;
   unsigned  width;
   unsigned  height;
   float     aspect;     // width / height -- < 0 is taller than wide
   unsigned  cbits;
   unsigned  grades;     // 1<<cbits for convenience
   } viddef_t;

extern   viddef_t vid;   // global video state

void VID_NormalizePalette (byte *palette);
// Changes a 0-255 rgb palette to a 0-63 rgb palette compatible with VGA

void	VID_SetPalette (byte *palette);
// called at startup and after any gamma correction

void	VID_GetPalette (byte *palette);
// called at startup and after any gamma correction

void  VID_Init (int32 width, int32 height, byte *palette, display_t displaytype, int scale );
// Called at startup to set up translation tables, takes 256 8 bit RGB values
// the palette data will go away after the call, so it must be copied off if
// the video driver will need it again

void	VID_Shutdown (void);
// Called at shutdown

void VID_GetBuffer (void);
// Called each frame before any drawing is to take place
// changes vid for next frame

void VID_ClearRegion
   (
   byte *destbuffer,
   int32 destclipx,
   int32 destclipy,
   int32 destclipwidth,
   int32 destclipheight,
   int32 destbytesperline,

   int32 drawx,
   int32 drawy,
   int32 drawwidth,
   int32 drawheight,
   int32 color
   );
// Sets the specified region of the view buffer to a single color.

void VID_BlitRegion
   (
   byte *sourcebuffer,
   int32 sourcewidth,
   int32 sourceheight,
   int32 sourcebytesperline,

   byte *destbuffer,
   int32 destclipx,
   int32 destclipy,
   int32 destclipwidth,
   int32 destclipheight,
   int32 destbytesperline,

   int32 imagex,
   int32 imagey,
   int32 imagewidth,
   int32 imageheight,

   int32 drawx,
   int32 drawy,
   blit_t blittype
   );

// Copies bitmap to the specified region of the view buffer.

void VID_UpdateRegion( int32 x, int32 y, int32 width, int32 height );
// flushes the given rectangle from the view buffer to the screen

void  VID_Update (int rectcount, vrect_t *rects);
// flushes the given rectangles from the view buffer to the screen

void  VID_UpdateScreen( void );
// flushes the entire view buffer to the screen

void  VID_ClearViewBuffer( int color );
// Sets the view buffer to a single color.

void ServiceEvents( void );
// Services any pending events (mouse, keyboard, window, etc.)
// Required for any events to take place.

void KEYBOARD_Startup
   (
   void
   );

void KEYBOARD_Shutdown
   (
   void
   );

#ifdef __cplusplus
};
#endif
#endif /* __vid_h */
