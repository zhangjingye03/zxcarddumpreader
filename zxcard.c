#include <stdio.h>
#include <stdlib.h>

struct zxcard_sec0 {
  unsigned char UID[5];
  unsigned char unknown_3[2]; //fixed 0x08 0x04 (?)
  unsigned char placeholder_1; //0x00
  unsigned char defaultSignature[8]; //bcdefghi
  unsigned char placeholder_2[32]; //0x00
  unsigned char keyA[6];
  unsigned char ACs[6]; //0xFF, 0x07, 0x80, 0x69
  unsigned char keyB[6];
};

struct zxcard_sec1 {
  // line 0, 0h~Fh
  unsigned short countA;
  unsigned char unknown_1A[2]; //not checksum!
  unsigned char decBalanceIntA[2];
  unsigned char decBalanceFloatA;
  unsigned char placeholder_1A; //0x00
  unsigned char seqPayCountA;
  unsigned char payPosIdA;
  unsigned char placeholder_2A; //0x00
  unsigned char unknown_2A;
  unsigned char placeholder_3A; //0x00
  unsigned char unknown_3A;
  unsigned char unknown_4A;
  unsigned char checksumA;
  // line 1, 10h~1Fh
  unsigned short countB;
  unsigned char unknown_1B[2]; //not checksum!
  unsigned char decBalanceIntB[2];
  unsigned char decBalanceFloatB;
  unsigned char placeholder_1B; //0x00
  unsigned char seqPayCountB;
  unsigned char payPosIdB;
  unsigned char placeholder_2B; //0x00
  unsigned char unknown_2B;
  unsigned char placeholder_3B; //0x00
  unsigned char unknown_3B;
  unsigned char unknown_4B;
  unsigned char checksumB;
  // line 2, 20h~2Fh
  unsigned short cardNum;
  unsigned char placeholder[14]; //0x12, 0x34, 0x01, 0x02, 0x01, 0xFF, 0x00, 0x00, 0x00, 0x17, 0x08, 0x01, 0x00, 0x00
  // line 2, 30h~3Fh
  unsigned char keyA[6];
  unsigned char ACs[6]; //0xFF, 0x07, 0x80, 0x69
  unsigned char keyB[6];
};

struct zxcard_sec2 {
  unsigned char placeholder_1[2];
  unsigned short cardNum;
  unsigned char unknown[6];
  unsigned char placeholder_2[38];
  unsigned char keyA[6];
  unsigned char ACs[6]; //0xFF, 0x07, 0x80, 0x69
  unsigned char keyB[6];
};

int main( int argc, char* argv[] ){
  //printf( "argc: %d, argv[0]: %c\n", argc, argv[1]);
  /*char a[2]; a[0] = 0x32; a[1] = 0x25;
  printf("%x%x\n", *a, *(a+1));*/
  if ( argc < 2 ) {
    printf( "No input file path specified.\n\nUsage: %c <file>\nThe parameter <file> is your zx card dump file\nEither 1KB version with all sectors or 192B version with first three blocks is acceptable.", argv[0]); return 1;
  }

  FILE* dump;
  dump = fopen( argv[1], "rb" ); //b for binary
  if ( dump == NULL ) {
    printf( "\nFile %d not found.", argv[1]); return 2;
  }
  fseek( dump, 0, SEEK_END ); //seek to the file end
  long dumpsize = ftell( dump ); //get bytes count
  printf( "\nFile size: %d", dumpsize );

  if ( dumpsize == 192 ) { //mini dump
    unsigned char buffer[192];
    fread( &buffer, 192, 1, dump ); //size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
    //TODO
    printf("\nFile Hex: %c", buffer);
  } else if ( dumpsize == 1024 ) { //full dump

  } else {
    printf( "\nWrong file size. Must be 192B (0~2 sectors) or 1KB (0~15 sectors)." ); return 3;
  }
}
