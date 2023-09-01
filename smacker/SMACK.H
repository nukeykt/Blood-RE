#ifndef SMACKH
#define SMACKH

#include "rad.h"

RCSTART

#define SMACKVERSION "3.0c"

typedef struct SmackTag {
  u32 Version;           // SMK2 only right now
  u32 Width;             // Width (1 based, 640 for example)
  u32 Height;            // Height (1 based, 480 for example)
  u32 Frames;            // Number of frames (1 based, 100 = 100 frames)
  u32 MSPerFrame;        // Frame Rate
  u32 SmackerType;       // bit 0 set=ring frame
  u32 LargestInTrack[7]; // Largest single size for each track
  u32 tablesize;         // Size of the init tables
  u32 codesize;          // Compression info   
  u32 absize;            // ditto
  u32 detailsize;        // ditto
  u32 typesize;          // ditto
  u32 TrackType[7];      // high byte=0x80-Comp,0x40-PCM data,0x20-16 bit,0x10-stereo
  u32 extra;             // extra value (should be zero)
  u32 NewPalette;        // set to one if the palette changed
  u8  Palette[772];      // palette data
  u32 PalType;           // type of palette
  u32 FrameNum;          // Frame Number to be displayed
  u32 FrameSize;         // The current frame's size in bytes
  u32 SndSize;           // The current frame sound tracks' size in bytes
  u32 LastRectx;         // Rect set in from SmackToBufferRect (X coord)
  u32 LastRecty;         // Rect set in from SmackToBufferRect (Y coord)
  u32 LastRectw;         // Rect set in from SmackToBufferRect (Width)
  u32 LastRecth;         // Rect set in from SmackToBufferRect (Height)
  u32 OpenFlags;         // flags used on open
  u32 LeftOfs;           // Left Offset used in SmackTo
  u32 TopOfs;            // Top Offset used in SmackTo
  u32 ReadError;         // Set to non-zero if a read error has ocurred
  u32 addr32;            // translated address for 16 bit interface
} Smack;              

#define SmackHeaderSize(smk) ((((u8*)&((smk)->extra))-((u8*)(smk)))+4)

typedef struct SmackSumTag {
  u32 TotalTime;         // total time
  u32 MS100PerFrame;     // MS*100 per frame (100000/MS100PerFrame=Frames/Sec)
  u32 TotalOpenTime;     // Time to open and prepare for decompression
  u32 TotalFrames;       // Total Frames displayed
  u32 SkippedFrames;     // Total number of skipped frames
  u32 SoundSkips;        // Total number of sound skips
  u32 TotalBlitTime;     // Total time spent blitting
  u32 TotalReadTime;     // Total time spent reading 
  u32 TotalDecompTime;   // Total time spent decompressing
  u32 TotalBackReadTime; // Total time spent reading in background
  u32 TotalReadSpeed;    // Total io speed (bytes/second)
  u32 SlowestFrameTime;  // Slowest single frame time
  u32 Slowest2FrameTime; // Second slowest single frame time
  u32 SlowestFrameNum;   // Slowest single frame number
  u32 Slowest2FrameNum;  // Second slowest single frame number
  u32 AverageFrameSize;  // Average size of the frame
  u32 Highest1SecRate;   // Highest 1 sec data rate
  u32 Highest1SecFrame;  // Highest 1 sec data rate starting frame
  u32 HighestMemAmount;  // Highest amount of memory allocated
  u32 TotalExtraMemory;  // Total extra memory allocated
  u32 HighestExtraUsed;  // Highest extra memory actually used
} SmackSum;


//=======================================================================
#define SMACKNEEDPAN    0x00020L // Will be setting the pan
#define SMACKNEEDVOLUME 0x00040L // Will be setting the volume
#define SMACKFRAMERATE  0x00080L // Override fr (call SmackFrameRate first)
#define SMACKLOADEXTRA  0x00100L // Load the extra buffer during SmackOpen
#define SMACKPRELOADALL 0x00200L // Preload the entire animation
#define SMACKNOSKIP     0x00400L // Don't skip frames if falling behind
#define SMACKSIMULATE   0x00800L // Simulate the speed (call SmackSim first)
#define SMACKFILEHANDLE 0x01000L // Use when passing in a file handle
#define SMACKTRACK1     0x02000L // Play audio track 1
#define SMACKTRACK2     0x04000L // Play audio track 2
#define SMACKTRACK3     0x08000L // Play audio track 3
#define SMACKTRACK4     0x10000L // Play audio track 4
#define SMACKTRACK5     0x20000L // Play audio track 5
#define SMACKTRACK6     0x40000L // Play audio track 6
#define SMACKTRACK7     0x80000L // Play audio track 7
#define SMACKTRACKS (SMACKTRACK1|SMACKTRACK2|SMACKTRACK3|SMACKTRACK4|SMACKTRACK5|SMACKTRACK6|SMACKTRACK7)

#define SMACKYINTERLACE 0x100000L // Force interleaving Y scaling
#define SMACKYDOUBLE    0x200000L // Force doubling Y scaling
#define SMACKYNONE  (SMACKYINTERLACE|SMACKYDOUBLE) // Force normal Y scaling
#define SMACKFILEISSMK 0x2000000L // Internal flag for 16 to 32 bit thunking

#define SMACKAUTOEXTRA 0xffffffffL // NOT A FLAG! - Use as extrabuf param 
//=======================================================================
  
#define SMACKSURFACEFAST             0
#define SMACKSURFACESLOW             1
#define SMACKSURFACEDIRECT           2
#define SMACKSURFACEFASTWITHZERORECT 3


RADDLLIMP Smack PTR4* RADEXPLINK SmackOpen(char PTR4* name,u32 flags,u32 extrabuf);

#ifdef __RADMAC__
  #include <files.h>

  Smack PTR4* RADEXPLINK SmackMacOpen(FSSpec* fsp,u32 flags,u32 extrabuf);
#endif

RADDLLIMP u32  RADEXPLINK SmackDoFrame(Smack PTR4* smk);
RADDLLIMP void RADEXPLINK SmackNextFrame(Smack PTR4* smk);
RADDLLIMP u32  RADEXPLINK SmackWait(Smack PTR4* smk);
RADDLLIMP void RADEXPLINK SmackClose(Smack PTR4* smk);

RADDLLIMP void RADEXPLINK SmackVolumePan(Smack PTR4* smk, u32 trackflag,u32 volume,u32 pan);    

RADDLLIMP void RADEXPLINK SmackSummary(Smack PTR4* smk,SmackSum PTR4* sum);

RADDLLIMP u32  RADEXPLINK SmackSoundInTrack(Smack PTR4* smk,u32 trackflags);
RADDLLIMP u32  RADEXPLINK SmackSoundOnOff(Smack PTR4* smk,u32 on);

#ifndef __RADMAC__
RADDLLIMP void RADEXPLINK SmackToScreen(Smack PTR4* smk,u32 left,u32 top,u32 BytesPS,u16 PTR4* WinTbl,u32 SetBank);
#endif

RADDLLIMP void RADEXPLINK SmackToBuffer(Smack PTR4* smk,u32 left,u32 top,u32 Pitch,u32 destheight,void PTR4* buf,u32 Reversed);
RADDLLIMP u32  RADEXPLINK SmackToBufferRect(Smack PTR4* smk, u32 SmackSurface);

RADDLLIMP void RADEXPLINK SmackGoto(Smack PTR4* smk,u32 frame);
RADDLLIMP void RADEXPLINK SmackColorRemap(Smack PTR4* smk,void PTR4* remappal,u32 numcolors,u32 paltype);
RADDLLIMP void RADEXPLINK SmackColorTrans(Smack PTR4* smk,void PTR4* trans);
RADDLLIMP void RADEXPLINK SmackFrameRate(u32 forcerate);
RADDLLIMP void RADEXPLINK SmackSimulate(u32 sim);

RADDLLIMP u32  RADEXPLINK SmackGetTrackData(Smack PTR4* smk,void PTR4* dest,u32 trackflag);

RADDLLIMP void RADEXPLINK SmackSoundCheck(void);

//======================================================================
#ifdef __RADDOS__

  #define SMACKSOUNDNONE -1

  extern void* SmackTimerSetupAddr;
  extern void* SmackTimerReadAddr;
  extern void* SmackTimerDoneAddr;

  typedef void RADLINK (*SmackTimerSetupType)(void);
  typedef u32 RADLINK (*SmackTimerReadType)(void);
  typedef void RADLINK (*SmackTimerDoneType)(void);

  #define SmackTimerSetup() ((SmackTimerSetupType)(SmackTimerSetupAddr))()
  #define SmackTimerRead() ((SmackTimerReadType)(SmackTimerReadAddr))()
  #define SmackTimerDone() ((SmackTimerDoneType)(SmackTimerDoneAddr))()

  u8 RADEXPLINK SmackSoundUseMSS(void* DigDriver);

  #ifndef AIL_startup
    #ifdef __SW_3R
      extern void cdecl AIL_startup_reg(void);
      #define AIL_startup AIL_startup_reg
    #else
      extern void cdecl AIL_startup_stack(void);
      #define AIL_startup AIL_startup_stack
    #endif
  #endif
  #define SmackSoundMSSLiteInit() SmackSoundMSSLiteInitWithStart(&AIL_startup);
  void RADEXPLINK SmackSoundMSSLiteInitWithStart(void* start);
  void RADEXPLINK SmackSoundMSSLiteDone(void);

  u8 RADEXPLINK SmackSoundUseSOS3r(u32 SOSDriver,u32 MaxTimerSpeed);
  u8 RADEXPLINK SmackSoundUseSOS3s(u32 SOSDriver,u32 MaxTimerSpeed);
  u8 RADEXPLINK SmackSoundUseSOS4r(u32 SOSDriver,u32 MaxTimerSpeed);
  u8 RADEXPLINK SmackSoundUseSOS4s(u32 SOSDriver,u32 MaxTimerSpeed);

  #ifdef __SW_3R
    #define SmackSoundUseSOS3 SmackSoundUseSOS3r
    #define SmackSoundUseSOS4 SmackSoundUseSOS4r
  #else
    #define SmackSoundUseSOS3 SmackSoundUseSOS3s
    #define SmackSoundUseSOS4 SmackSoundUseSOS4s
  #endif

#else

  #define SMACKRESRESET    0
  #define SMACKRES640X400  1
  #define SMACKRES640X480  2
  #define SMACKRES800X600  3
  #define SMACKRES1024X768 4

  RADDLLIMP u32 RADEXPLINK SmackSetSystemRes(u32 mode);  // use SMACKRES* values

  #ifdef __RADMAC__

    #include <windows.h>
    #include <palettes.h>
    #include <qdoffscreen.h>

    #define SmackTimerSetup()
    #define SmackTimerDone()
    u32 RADEXPLINK SmackTimerRead(void);

    u16 RADEXPLINK SmackBufferUseGDevice( GDHandle gd );

    #define SMACKAUTOBLIT          0
    #define SMACKDIRECTBLIT        1
    #define SMACKGWORLDBLIT        2
    #define SMACKGWORLDCUSTOMBLIT  3

    typedef struct SmackBufTag {
      u32 Reversed;
      u32 SurfaceType;    // SMACKSURFACExxxxxx
      u32 BlitType;       // SMACKxxxxxBLIT
      u32 Width;
      u32 Height;
      u32 Pitch;
      u32 Zoomed;
      u32 ZWidth;
      u32 ZHeight;
      u32 DispColors;     // colors on screen
      u32 MaxPalColors;
      u32 PalColorsInUse;
      u32 StartPalColor;
      u32 EndPalColor;
      void* Buffer;
      void* Palette;
      u32 PalType;
      u32 cursortype;

      WindowPtr wp;
      GWorldPtr gwp;
      CTabHandle cth;
      PaletteHandle palh;
    } SmackBuf;

  #else

    #ifdef __RADWIN__

      #define INCLUDE_MMSYSTEM_H
      #include "windows.h"
      #include "windowsx.h"

      #ifdef __RADNT__          // to combat WIN32_LEAN_AND_MEAN
        #include "mmsystem.h"
      #endif

      #define SMACKAUTOBLIT        0
      #define SMACKFULL320X240BLIT 1
      #define SMACKFULL320X200BLIT 2
      #define SMACKSTANDARDBLIT    3
      #define SMACKWINGBLIT        4

      #define WM_SMACKACTIVATE WM_USER+0x5678

      typedef struct SmackBufTag {
        u32 Reversed;             // 1 if the buffer is upside down
        u32 SurfaceType;          // SMACKSURFACExxxx defines
        u32 BlitType;             // SMACKxxxxBLIT defines
        u32 FullScreen;           // 1 if full-screen
        u32 Width;
        u32 Height;
        u32 Pitch;
        u32 Zoomed;
        u32 ZWidth;
        u32 ZHeight;
        u32 DispColors;           // colors on the screen
        u32 MaxPalColors;         // total possible colors in palette (usually 256)
        u32 PalColorsInUse;       // Used colors in palette (usually 236)
        u32 StartPalColor;        // first usable color index (usually 10)
        u32 EndPalColor;          // last usable color index (usually 246)
        RGBQUAD Palette[256];
        u32 PalType;
        u32 forceredraw;  // force a complete redraw on next blit (for >8bit)
        u32 didapalette;  // force an invalidate on the next palette change

        void PTR4* Buffer;
        void PTR4* DIBRestore;
        u32 OurBitmap;
        u32 OrigBitmap;
        u32 OurPalette;
        u32 WinGDC;
        u32 FullFocused;
        u32 ParentHwnd;
        u32 OldParWndProc;
        u32 OldDispWndProc;
        u32 DispHwnd;
        u32 WinGBufHandle;
      } SmackBuf;

      RADDLLIMP void RADEXPLINK SmackGet(Smack PTR4* smk,void PTR4* dest);
      RADDLLIMP void RADEXPLINK SmackBufferGet( SmackBuf PTR4* sbuf, void PTR4* dest);
      
      RADDLLIMP u8 RADEXPLINK SmackSoundUseMSS(void PTR4* dd);
      RADDLLIMP u8 RADEXPLINK SmackSoundUseDirectSound(void PTR4* dd); // NULL=Create

      #define SmackTimerSetup()
      #define SmackTimerDone()
      #define SmackTimerRead timeGetTime

    #endif

  #endif

  #ifdef __RADMAC__
    SmackBuf PTR4* RADEXPLINK SmackBufferOpen( WindowPtr wp, u16 BlitType, u16 width, u16 height, u16 ZoomW, u16 ZoomH );
    u16  RADEXPLINK SmackBufferBlit( SmackBuf PTR4* sbuf, u16 hwndx, u16 hwndy, u16 subx, u16 suby, u16 subw, u16 subh );
    void RADEXPLINK SmackBufferFromScreen( SmackBuf PTR4* destbuf, u16 x, u16 y);
  #else
    RADDLLIMP SmackBuf PTR4* RADEXPLINK SmackBufferOpen( HWND wnd, u16 BlitType, u16 width, u16 height, u16 ZoomW, u16 ZoomH );
    RADDLLIMP u16  RADEXPLINK SmackBufferBlit( SmackBuf PTR4* sbuf, HDC dc, u16 hwndx, u16 hwndy, u16 subx, u16 suby, u16 subw, u16 subh );
    RADDLLIMP void RADEXPLINK SmackBufferFromScreen( SmackBuf PTR4* destbuf, HWND hw, u16 x, u16 y);
  #endif

  RADDLLIMP char PTR4* RADEXPLINK SmackBufferString(char PTR4* dest,u16 BlitType);

  RADDLLIMP void RADEXPLINK SmackBufferNewPalette( SmackBuf PTR4* sbuf, void PTR4* pal, u16 paltype );
  RADDLLIMP u16  RADEXPLINK SmackBufferSetPalette( SmackBuf PTR4* sbuf );
  RADDLLIMP void RADEXPLINK SmackBufferClose( SmackBuf PTR4* sbuf );
 
  RADDLLIMP void RADEXPLINK SmackBufferClear( SmackBuf PTR4* destbuf, u16 color);
  RADDLLIMP void RADEXPLINK SmackBufferToBuffer( SmackBuf PTR4* destbuf, u16 destx, u16 desty, SmackBuf PTR4* sourcebuf,u16 sourcex,u16 sourcey,u16 sourcew,u16 sourceh);
  RADDLLIMP void RADEXPLINK SmackBufferToBufferTrans( SmackBuf PTR4* destbuf, u16 destx, u16 desty, SmackBuf PTR4* sourcebuf,u16 sourcex,u16 sourcey,u16 sourcew,u16 sourceh,u16 TransColor);
  RADDLLIMP void RADEXPLINK SmackBufferCopyPalette( SmackBuf PTR4* destbuf, SmackBuf PTR4* sourcebuf, u16 remap);

  RADDLLIMP u16  RADEXPLINK SmackBufferFocused( SmackBuf PTR4* sbuf);

#endif

RCEND

#endif
