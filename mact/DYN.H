#ifndef __dyn_public__
#define __dyn_public__
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
   {
   int32 maxnumber;
   int32 numberperblock;
   int32 arrayblockshift;
   int32 datasize;
   int32 datashift;
   int32 numberblocks;
   void ** arrays;
   } dyn_t;

dyn_t * DYN_Create
   (
   int32 maxnumber,
   int32 numberperblock,
   int32 datasize
   );

void DYN_Destroy
   (
   dyn_t * dyn
   );

void * DYN_Entry
   (
   dyn_t * dyn,
   int32 itemnumber
   );

#ifdef __cplusplus
};
#endif
#endif
