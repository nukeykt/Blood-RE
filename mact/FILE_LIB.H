#ifndef _file_lib_public
#define _file_lib_public
#ifdef __cplusplus
extern "C" {
#endif

enum
   {
   filetype_binary,
   filetype_text
   };

enum
   {
   access_read,
   access_write,
   access_append
   };

//==========================================================================
//
// SafeOpenWrite - Opens a file for writing, returns handle
//
//==========================================================================
int32 SafeOpenWrite ( const char * filename, int32 filetype );

//==========================================================================
//
// SafeOpenRead - Opens a file for reading, returns handle
//
//==========================================================================
int32 SafeOpenRead ( const char * filename, int32 filetype );

//==========================================================================
//
// SafeOpenAppend - Opens a file for appending, returns handle
//
//==========================================================================
int32 SafeOpenAppend ( const char * filename, int32 filetype );

//==========================================================================
//
// SafeClose - Close a file denoted by the file handle
//
//==========================================================================
void SafeClose ( int32 handle );

//==========================================================================
//
// SafeFileExists - Checks for existence of file
//
//==========================================================================
boolean SafeFileExists ( const char * filename );

//==========================================================================
//
// SafeFileLength - Get length of a file pointed to by handle
//
//==========================================================================
int32 SafeFileLength ( int32 handle );

//==========================================================================
//
// SafeTell - Get file position of handle
//
//==========================================================================
int32 SafeTell( int32 handle );

//==========================================================================
//
// SafeSeek - Set file position of handle
//
//==========================================================================
void SafeSeek( int32 handle, int32 position );

//==========================================================================
//
// SafeRead - reads from a handle
//
//            handle - handle of file to read from
//
//            buffer - pointer of buffer to read into
//
//            count  - number of bytes to read
//
//==========================================================================
void SafeRead (int32 handle, void *buffer, int32 count);

//==========================================================================
//
// SafeWrite - writes to a handle
//
//             handle - handle of file to write to
//
//             buffer - pointer of buffer to write from
//
//             count  - number of bytes to write
//
//==========================================================================
void SafeWrite (int32 handle, void *buffer, int32 count);

//==========================================================================
//
// LoadFile - Load a file
//
//            filename - name of file
//
//            bufferptr - pointer to pointer of buffer to read into
//
//            returns number of bytes read
//
//==========================================================================
int32 LoadFile ( const char * filename, void ** bufferptr );

//==========================================================================
//
// SaveFile - Save a file
//
//            filename - name of file
//
//            bufferptr - pointer to buffer to write from
//
//            count - number of bytes to write
//
//==========================================================================
void SaveFile ( const char * filename, void * bufferptr, int32 count );

//==========================================================================
//
// GetPathFromEnvironment - Add a pathname described in an environment
//                          variable to a standard filename.
//
//                          fullname - final string containing entire path
//
//                          envname - string naming enivronment variable
//
//                          filename - standard filename
//
//==========================================================================
void GetPathFromEnvironment( char *fullname, const char *envname, const char *filename );

//==========================================================================
//
// DefaultExtension - Add a default extension to a path
//
//                    path - a path
//
//                    extension - default extension should include '.'
//
//==========================================================================
void DefaultExtension (char *path, const char *extension);

//==========================================================================
//
// DefaultPath - Add the default path to a filename if it doesn't have one
//
//               path - filename
//
//               extension - default path
//
//==========================================================================
void DefaultPath (char *path, const char *basepath);

//==========================================================================
//
// ExtractFileBase - Extract the base filename from a path
//
//                   path - the path
//
//                   dest - where the file base name will be placed
//
//==========================================================================
void ExtractFileBase (char *path, char *dest);

//==========================================================================
//
// GetExtension - Extract the extension from a name
//                returns true if an extension is found
//                returns false otherwise
//
//==========================================================================
boolean GetExtension( char *filename, char *extension );

//==========================================================================
//
// SetExtension - Sets the extension from a name.  Assumes that enough
// 					space is left at the end of the string to hold an extension.
//
//==========================================================================
void SetExtension( char *filename, const char *extension );

#ifdef __MSDOS__
//******************************************************************************
//
// GetPath
//
// Purpose
//    To parse the directory entered by the user to make the directory.
//
// Parms
//    Path - the path to be parsed.
//
// Returns
//    Pointer to next path
//
//******************************************************************************
char * GetPath (char * path, char *dir);

//******************************************************************************
//
// ChangeDirectory ()
//
// Purpose
//    To change to a directory.  Checks for drive changes.
//
// Parms
//    path - The path to change to.
//
// Returns
//    TRUE  - If successful.
//    FALSE - If unsuccessful.
//
//******************************************************************************
boolean ChangeDirectory (char * path);

//******************************************************************************
//
// ChangeDrive ()
//
// Purpose
//    To change drives.
//
// Parms
//    drive - The drive to change to.
//
// Returns
//    TRUE  - If drive change successful.
//    FALSE - If drive change unsuccessful.
//
//******************************************************************************
boolean ChangeDrive (char *drive);

#endif

#ifdef __cplusplus
};
#endif
#endif
