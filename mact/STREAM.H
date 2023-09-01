// stream.h

#ifndef _stream_public
#define _stream_public
#ifdef __cplusplus
extern "C" {
#endif

//====================================================================
//
// streamlib
//
//====================================================================
// streamlib is a primitive stream parser, it separates a text stream
// into separate tokens which are denoted by non-printable characters,
// spaces and the semi-colon.
//
// comments can be added using the standard c "//"
// Several of the functions take crossline as a parameter this tells
// the library whether or not to cross over the next EOL or semi-colon.
// this behavior changes slightly with these three functions:
// STREAM_Get - gets the next token in the stream, crossline denotes
//              whether or not to cross over semi-colons or EOLS.
//
// NOTE: multiple tokens can be grabbed as a string if the '"' is used
// around the sequence of tokens.  The semi-colon is not active when
// it is between two quotes.
//
//

void STREAM_Free( int32 streamhandle );
void STREAM_Reset ( int32 streamhandle );
int32 STREAM_Parse ( char *data, int32 length, char * name );
int32 STREAM_GetLineNumber( int32 streamhandle );
boolean STREAM_More (int32 streamhandle, boolean crossline);
char * STREAM_Get
   (
   int32 streamhandle,
   boolean crossline
   );

#ifdef __cplusplus
};
#endif
#endif
