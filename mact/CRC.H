#ifndef _crc_public
#define _crc_public
#ifdef __cplusplus
extern "C" {
#endif

int32 UpdateCRC(int32 crc, int32 c);
uint16 CalculateCRC (uint8 * src, uint32 size);

#ifdef __cplusplus
};
#endif
#endif
