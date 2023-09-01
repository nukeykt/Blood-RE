//****************************************************************************
//
// Public header for EXTERNAL controller support
//
//****************************************************************************

#ifndef _external_public_
#define _external_public_
#ifdef __cplusplus
extern "C" {
#endif

// External Controller ID

#define CONTROLID 0xdead
#define EXTERNALPARM "control"
#define EXTERNAL_GetInput  1
#define EXTERNALAXISUNDEFINED   0x7f
#define EXTERNALBUTTONUNDEFINED 0x7f
#define MAXEXTERNALAXES 6
#define MAXEXTERNALBUTTONS 32

typedef struct
   {
   word      id;
   word      intnum;
   int32     axes[MAXEXTERNALAXES];
   uint32    buttonstate;
   byte      buttonmap[MAXEXTERNALBUTTONS][2];
   byte      analogaxesmap[MAXEXTERNALAXES];
   word      command;
   byte      digitalaxesmap[MAXEXTERNALAXES][2];
   } ExternalControlInfo;

#ifdef __cplusplus
};
#endif
#endif
