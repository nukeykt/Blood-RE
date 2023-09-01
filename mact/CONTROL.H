//***************************************************************************
//
// Public header for CONTROL.C.
//
//***************************************************************************

#ifndef _control_public
#define _control_public
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "keyboard.h"


//***************************************************************************
//
// DEFINES
//
//***************************************************************************

// maximum number of joysticks which can be hooked up to your machine
#define MaxJoys             2
// maximum number of "gamefunc_" type buttons
#define MAXGAMEBUTTONS      64

// the current state of a specific button
#define BUTTON(x) \
    ( \
    ((x)>31) ? \
    ((CONTROL_ButtonState2>>( (x) - 32) ) & 1) :\
    ((CONTROL_ButtonState1>> (x) ) & 1)          \
    )
// the last state of a specific button, meaning the state of the button,
// the last time CONTROL_GetInput was called
#define LASTBUTTON(x) \
    ( \
    ((x)>31) ? \
    ((CONTROL_ButtonHeldState2>>((x)-32)) & 1) :\
    ((CONTROL_ButtonHeldState1>>(x)) & 1)\
    )

// compatibility define
#define BUTTONHELD(x) LASTBUTTON(x)
// debounced button press
#define BUTTONJUSTPRESSED(x) \
    ( BUTTON( x ) && !LASTBUTTON( x ) )
// debounced button releas
#define BUTTONRELEASED(x) \
    ( !BUTTON( x ) && LASTBUTTON( x ) )
// the button state has changed
#define BUTTONSTATECHANGED(x) \
    ( BUTTON( x ) != LASTBUTTON( x ) )

//***************************************************************************
//
// TYPEDEFS
//
//***************************************************************************
// axis definitions for digital axis mapping
typedef enum
   {
   axis_up,
   axis_down,
   axis_left,
   axis_right
   } axisdirection;

// analog axes definition
typedef enum
   {
   // rotation around y
   analog_turning=0,
   // translation along x
   analog_strafing=1,
   // rotation around x
   analog_lookingupanddown=2,
   // tranlation along y
   analog_elevation=3,
   // rotation around z
   analog_rolling=4,
   // tranlation along z
   analog_moving=5,
   analog_maxtype
   } analogcontrol;

// direction defines for UserInput
typedef enum
   {
   dir_North,
   dir_NorthEast,
   dir_East,
   dir_SouthEast,
   dir_South,
   dir_SouthWest,
   dir_West,
   dir_NorthWest,
   dir_None
   } direction;

// UserInput structure, this structure is passed into calls to CONTROL_GetUserInput
// button0 is enter or space, button 1 is Escape, dir is the direction of a
// controller or the arrow keys
typedef struct
   {
   boolean   button0;
   boolean   button1;
   direction dir;
   } UserInput;

// the interface structure to CONTROL_GetInput.
// the button state is explicitly defined in CONTROL_ButtonState?
// the button state is accessed through the BUTTON macros
typedef struct
   {
   fixed     dx;
   fixed     dy;
   fixed     dz;
   fixed     dyaw;
   fixed     dpitch;
   fixed     droll;
   } ControlInfo;

// the different control paradigms for the CONTROL library
typedef enum
   {
   controltype_keyboard,
   controltype_keyboardandmouse,
   controltype_keyboardandjoystick,
   controltype_keyboardandexternal,
   controltype_keyboardandgamepad,
   controltype_keyboardandflightstick,
   controltype_keyboardandthrustmaster
   } controltype;


//***************************************************************************
//
// GLOBALS
//
//***************************************************************************

// whether the rudder is enabled
extern boolean  CONTROL_RudderEnabled;
// whether a mouse was found (only applicable under keyboardandmouse)
extern boolean  CONTROL_MousePresent;
// whether the mouse is enabled
extern boolean  CONTROL_MouseEnabled;
// whether the joystick is enabled
extern boolean  CONTROL_JoystickEnabled;
// what port the joystick is on
extern byte     CONTROL_JoystickPort;
// these variables contain the current button state and the previous
// button state from the last call to CONTROL_GetInput
extern uint32   CONTROL_ButtonState1;
extern uint32   CONTROL_ButtonHeldState1;
extern uint32   CONTROL_ButtonState2;
extern uint32   CONTROL_ButtonHeldState2;


//***************************************************************************
//
// PROTOTYPES
//
//***************************************************************************

// define the keyboard scancodes for a gamefunc function denoted by which
void CONTROL_MapKey( int32 which, kb_scancode key1, kb_scancode key2 );
// get the current key definitions
void CONTROL_GetKeyMap( int32 which, int32 * key1, int32 * key2 );
// define a button for a gamefunc function.  whichbutton denotes which button
// to map on the device currently being used (joystick, mouse etc.)
// doubleclicked is whether this is for when the button is singleclicked or
// doubleclicked
void CONTROL_MapButton
        (
        int32 whichfunction,
        int32 whichbutton,
        boolean doubleclicked
        );
// get the current button mapping for the gamefunc
void CONTROL_GetButtonMap
   (
   int32   whichfunction,
   int32 * singleclicked,
   int32 * doubleclicked
   );
// setup a specific gamefunc button.  which is the gamefunc button.
// toggle is whether the button is to be momentary(false) or a toggle button
// (true)
void CONTROL_DefineFlag( int32 which, boolean toggle );
// tells whether or not a specific gamefunc button has been setup with
// CONTROL_DefineFlag
boolean CONTROL_FlagActive( int32 which );
// clear all gamefunc setup stuff.  clears all effects from calling CONTROL_DefineFlag
void CONTROL_ClearAssignments( void );
// get input which can be used when moving around in the menu
// this changes the button state, so it should only be called once
// you have entered "menu mode", i.e. input has been changed from the
// game to the menu
void CONTROL_GetUserInput( UserInput *info );
// get the current control information.  this updates the button state
void CONTROL_GetInput( ControlInfo *info );
// clear a specific button, that button will not function as a button again,
// until the device button or keyboard button is released, even if the keyboard
// is in auto-repeat mode for that key.
void CONTROL_ClearButton( int32 whichbutton );
// clear the state of the userinput.  if either button is set to true, that
// button will not become true again until it is released and then pressed
// again.  Likewise, if a dir is set, that dir will not register, until the
// device is let go or the keyboard button is released
void CONTROL_ClearUserInput( UserInput *info );
// wait for all UserInput "buttons" to be released.  Should generally not
// be called, because it sits in a while loop
void CONTROL_WaitRelease( void );
// waits for all UserInput to be released. CAUTION: sits in a while loop
void CONTROL_Ack( void );
// Centers the joystick, each callback is called to display an appropriate
// message and then once a UserInput button is pressed, the current state
// of the joystick is read.  This should be called after all music and sound
// has been turned on, so that the values will be accurate for the game.
#ifdef __cplusplus
void CONTROL_CenterJoystick
   (
   void CenterCenter( void ),
   void UpperLeft( void ),
   void LowerRight( void ),
   void CenterThrottle( void ),
   void CenterRudder( void )
   );
#else
void CONTROL_CenterJoystick
   (
   void ( *CenterCenter )( void ),
   void ( *UpperLeft )( void ),
   void ( *LowerRight )( void ),
   void ( *CenterThrottle )( void ),
   void ( *CenterRudder )( void )
   );
#endif
// Set the DC_bias for the hat button on those joysticks which use the throttle
// on the second joystick to encode the hat switch.
void CONTROL_SetJoystickHatBias
   (
   int32 newbias
   );
// Get Hat bias
int32 CONTROL_GetJoystickHatBias
   (
   void
   );
// get the current mouse sensitivity
int32 CONTROL_GetMouseSensitivity( void );
// set the current mouse sensitivity
void CONTROL_SetMouseSensitivity( int32 newsensitivity );
// start the control system.  which is the type of control paradigm to be used
// TimeFunction is a call back function to get the current "time" in the game
// ticspersecond is the timebase for the above "time"
void CONTROL_Startup
   (
   controltype which,
   int32 ( *TimeFunction )( void ),
   int32 ticspersecond
   );
// shutdown the control system
void CONTROL_Shutdown( void );

// map an analog axis, which axis is which device axis to map
// which analog is which analog axis funtion to map to (analog_moving)
void CONTROL_MapAnalogAxis
   (
   int32 whichaxis,
   int32 whichanalog
   );
// get the current analog axis mapping
int32 CONTROL_GetAnalogAxisMap
   (
   int32 whichaxis
   );
// map gamefunc buttons to a specific axis.  whichaxis is which device axis.
// whichfunction is which gamefunc button to map.
// direction is which direction of the axis you want to map, for verticle axes
// this would be axis_up and axis_down.  for horizontal axes it would be axis_left
// and axis_right.
void CONTROL_MapDigitalAxis
   (
   int32 whichaxis,
   int32 whichfunction,
   int32 direction
   );
// get digital axis mapping
void CONTROL_GetDigitalAxisMap
   (
   int32 whichaxis,
   int32 * min,
   int32 * max
   );
// set the scale value for a device axis.  which axis is which device axis.
// axisscale is a 16.16 fixed point scale value.  NOTE: the scaled axis is
// only used with analog functions, digital mappings are resolved before
// scaling
void CONTROL_SetAnalogAxisScale
   (
   int32 whichaxis,
   int32 axisscale
   );
// get the scale value for a device axis
int32 CONTROL_GetAnalogAxisScale
   (
   int32 whichaxis
   );
// debug routine to see the current state of your axes
void CONTROL_PrintAxes( void );
// debug routine to print out key mappings
void CONTROL_PrintKeyMap
   (
   void
   );
// debug routine to print out the current state of a gamefunc function
void CONTROL_PrintControlFlag
   (
   int32 i
   );

#ifdef __cplusplus
};
#endif
#endif
