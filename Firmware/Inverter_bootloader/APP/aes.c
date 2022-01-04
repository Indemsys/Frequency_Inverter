#include "app_cfg.h"

extern const unsigned char AESKey[32];

#define BPOLY     0x1b //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE 16   //!< Block size in number of bytes.



#if KEY_COUNT == 1
  #define KEYBITS 128 //!< Use AES128.
#elif KEY_COUNT == 2
  #define KEYBITS 192 //!< Use AES196.
#elif KEY_COUNT == 3
  #define KEYBITS 256 //!< Use AES256.
#else
  #error Use 1, 2 or 3 keys!
#endif

#if KEYBITS == 128
  #define ROUNDS 10 //!< Number of rounds.
  #define KEYLENGTH 16 //!< Key length in number of bytes.
#elif KEYBITS == 192
  #define ROUNDS 12 //!< Number of rounds.
  #define KEYLENGTH 24 //!< // Key length in number of bytes.
#elif KEYBITS == 256
  #define ROUNDS 14 //!< Number of rounds.
  #define KEYLENGTH 32 //!< Key length in number of bytes.
#else
  #error Key must be 128, 192 or 256 bits!
#endif

#define EXPANDED_KEY_SIZE (BLOCKSIZE * (ROUNDS+1)) //!< 176, 208 or 240 bytes.



INT8U block1[ 256 ]; //!< Workspace 1.
INT8U block2[ 256 ]; //!< Worksapce 2.
INT8U sBox[256];


INT8U * powTbl; //!< Final location of exponentiation lookup table.
INT8U * logTbl; //!< Final location of logarithm lookup table.
INT8U * sBoxInv; //!< Final location of inverse s-box.
INT8U * expandedKey; //!< Final location of expanded key.



void CalcPowLog( INT8U * powTbl, INT8U * logTbl )
{
  INT8U i = 0;
  INT8U t = 1;

  do
  {
    // Use 0x03 as root for exponentiation and logarithms.
    powTbl[i] = t;
    logTbl[t] = i;
    i++;

    // Muliply t by 3 in GF(2^8).
    t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);
  } while ( t != 1 ); // Cyclic properties ensure that i < 255.

  powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc.
}



void CalcSBox( INT8U * sBox )
{
  INT8U i, rot;
  INT8U temp;
  INT8U result;

  // Fill all entries of sBox[].
  i = 0;
  do
  {
    // Inverse in GF(2^8).
    if ( i > 0 )
    {
      temp = powTbl[ 255 - logTbl[i] ];
    }
    else
    {
      temp = 0;
    }

    // Affine transformation in GF(2).
    result = temp ^ 0x63; // Start with adding a vector in GF(2).
    for ( rot = 0; rot < 4; rot++ )
    {
      // Rotate left.
      temp = (temp<<1) | (temp>>7);

      // Add rotated byte in GF(2).
      result ^= temp;
    }

    // Put result in table.
    sBox[i] = result;
  } while ( ++i != 0 );
}



void CalcSBoxInv( INT8U * sBox, INT8U * sBoxInv )
{
  INT8U i = 0;
  INT8U j = 0;

  // Iterate through all elements in sBoxInv using  i.
  do
  {
    // Search through sBox using j.
    do
    {
      // Check if current j is the inverse of current i.
      if ( sBox[ j ] == i )
      {
        // If so, set sBoxInc and indicate search finished.
        sBoxInv[ i ] = j;
        j = 255;
      }
    } while ( ++j != 0 );
  } while ( ++i != 0 );
}



void CycleLeft( INT8U * row )
{
  // Cycle 4 bytes in an array left once.
  INT8U temp = row[0];
  row[0] = row[1];
  row[1] = row[2];
  row[2] = row[3];
  row[3] = temp;
}



void InvMixColumn( INT8U * column )
{
  INT8U r0, r1, r2, r3;

  r0 = column[1] ^ column[2] ^ column[3];
  r1 = column[0] ^ column[2] ^ column[3];
  r2 = column[0] ^ column[1] ^ column[3];
  r3 = column[0] ^ column[1] ^ column[2];

  column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
  column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
  column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
  column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

  r0 ^= column[0] ^ column[1];
  r1 ^= column[1] ^ column[2];
  r2 ^= column[2] ^ column[3];
  r3 ^= column[0] ^ column[3];

  column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
  column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
  column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
  column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

  r0 ^= column[0] ^ column[2];
  r1 ^= column[1] ^ column[3];
  r2 ^= column[0] ^ column[2];
  r3 ^= column[1] ^ column[3];

  column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
  column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
  column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
  column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

  column[0] ^= column[1] ^ column[2] ^ column[3];
  r0 ^= column[0];
  r1 ^= column[0];
  r2 ^= column[0];
  r3 ^= column[0];

  column[0] = r0;
  column[1] = r1;
  column[2] = r2;
  column[3] = r3;
}



void SubBytes( INT8U * bytes, INT8U count )
{
  do
  {
    *bytes = sBox[ *bytes ]; // Substitute every byte in state.
    bytes++;
  } while ( --count );
}



void InvSubBytesAndXOR( INT8U * bytes, INT8U * key, INT8U count )
{
  do
  {
//    *bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add key.
    *bytes = block2[ *bytes ] ^ *key; // Use block2 directly. Increases speed.
    bytes++;
    key++;
  } while ( --count );
}



void InvShiftRows( INT8U * state )
{
  INT8U temp;

  // Note: State is arranged column by column.

  // Cycle second row right one time.
  temp = state[ 1 + 3*4 ];
  state[ 1 + 3*4 ] = state[ 1 + 2*4 ];
  state[ 1 + 2*4 ] = state[ 1 + 1*4 ];
  state[ 1 + 1*4 ] = state[ 1 + 0*4 ];
  state[ 1 + 0*4 ] = temp;

  // Cycle third row right two times.
  temp = state[ 2 + 0*4 ];
  state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
  state[ 2 + 2*4 ] = temp;
  temp = state[ 2 + 1*4 ];
  state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
  state[ 2 + 3*4 ] = temp;

  // Cycle fourth row right three times, ie. left once.
  temp = state[ 3 + 0*4 ];
  state[ 3 + 0*4 ] = state[ 3 + 1*4 ];
  state[ 3 + 1*4 ] = state[ 3 + 2*4 ];
  state[ 3 + 2*4 ] = state[ 3 + 3*4 ];
  state[ 3 + 3*4 ] = temp;
}



void InvMixColumns( INT8U * state )
{
  InvMixColumn( state + 0*4 );
  InvMixColumn( state + 1*4 );
  InvMixColumn( state + 2*4 );
  InvMixColumn( state + 3*4 );
}



void XORBytes( INT8U * bytes1, INT8U * bytes2, INT8U count )
{
  do
  {
    *bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.
    bytes1++;
    bytes2++;
  } while ( --count );
}



void CopyBytes( INT8U * to, INT8U * from, INT8U count )
{
  do
  {
    *to = *from;
    to++;
    from++;
  } while ( --count );
}



void KeyExpansion( INT8U * expandedKey )
{
  INT8U temp[4];
  INT8U i;
  INT8U Rcon[4] = { 0x01, 0x00, 0x00, 0x00}; // Round constant.

  unsigned char const * key = AESKey;

  // Copy key to start of expanded key.
  i = KEYLENGTH;
  do
  {
    *expandedKey = *key;
    expandedKey++;
    key++;
  } while ( --i );

  // Prepare last 4 bytes of key in temp.
  expandedKey -= 4;
  temp[0] = *(expandedKey++);
  temp[1] = *(expandedKey++);
  temp[2] = *(expandedKey++);
  temp[3] = *(expandedKey++);

  // Expand key.
  i = KEYLENGTH;
  while ( i < BLOCKSIZE*(ROUNDS+1) )
  {
    // Are we at the start of a multiple of the key size?
    if ( (i % KEYLENGTH) == 0 )
    {
      CycleLeft( temp ); // Cycle left once.
      SubBytes( temp, 4 ); // Substitute each byte.
      XORBytes( temp, Rcon, 4 ); // Add constant in GF(2).
      *Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);
    }

    // Keysize larger than 24 bytes, ie. larger that 192 bits?
#if KEYLENGTH > 24
    // Are we right past a block size?
    else if ( (i % KEYLENGTH) == BLOCKSIZE )
    {
      SubBytes( temp, 4 ); // Substitute each byte.
    }
#endif

    // Add bytes in GF(2) one KEYLENGTH away.
    XORBytes( temp, expandedKey - KEYLENGTH, 4 );

    // Copy result to current 4 bytes.
    *(expandedKey++) = temp[ 0 ];
    *(expandedKey++) = temp[ 1 ];
    *(expandedKey++) = temp[ 2 ];
    *(expandedKey++) = temp[ 3 ];

    i += 4; // Next 4 bytes.
  }
}



void InvCipher( INT8U * block, INT8U * expandedKey )
{
  INT8U round = ROUNDS-1;
  expandedKey += BLOCKSIZE * ROUNDS;

  XORBytes( block, expandedKey, 16 );
  expandedKey -= BLOCKSIZE;

  do
  {
    InvShiftRows( block );
    InvSubBytesAndXOR( block, expandedKey, 16 );
    expandedKey -= BLOCKSIZE;
    InvMixColumns( block );
  } while ( --round );

  InvShiftRows( block );
  InvSubBytesAndXOR( block, expandedKey, 16 );
}



void aesInit(void)
{
  powTbl = block1;
  logTbl = block2;
  CalcPowLog( powTbl, logTbl );

//  sBox = tempbuf;
  CalcSBox( sBox );

  expandedKey = block1;
  KeyExpansion( expandedKey );

  sBoxInv = block2; // Must be block2.
  CalcSBoxInv( sBox, sBoxInv );
}



void aesDecrypt( unsigned char * buffer, unsigned char * chainBlock )
{
  INT8U temp[ BLOCKSIZE ];

  CopyBytes( temp, buffer, BLOCKSIZE );
  InvCipher( buffer, expandedKey );
  XORBytes( buffer, chainBlock, BLOCKSIZE );
  CopyBytes( chainBlock, temp, BLOCKSIZE );
}

