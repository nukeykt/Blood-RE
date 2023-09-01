#ifndef _random_public
#define _random_public
#ifdef __cplusplus
extern "C" {
#endif

void  InitializeRandomNumberGenerator ( void );
int32 GetRandomNumberSeed ( void );


#if RANDOMNUMBERTEST
byte    GameRandom ( char * string );
#define GameRandomNumber( string )   GameRandom(string)
#else
byte    GameRandom ( void );
#define GameRandomNumber(string)   GameRandom()
#endif

byte    Random ( void );
#define RandomNumber   Random

void    SetRandomNumberIndex ( int32 i );
int32   GetRandomNumberIndex ( void );

#ifdef __cplusplus
};
#endif
#endif
