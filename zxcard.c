#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#define MAXLEN 1000

struct zxcard_sec0 {
  unsigned char UID[5];
  unsigned char unknown_3[2]; //fixed 0x08 0x04 (?)
  unsigned char placeholder_1; //0x00
  unsigned char defaultSignature[8]; //bcdefghi
  unsigned char placeholder_2[32]; //0x00
  unsigned char keyA[6];
  unsigned char ACs[4]; //0xFF, 0x07, 0x80, 0x69
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
  unsigned char ACs[4]; //0xFF, 0x07, 0x80, 0x69
  unsigned char keyB[6];
};

struct zxcard_sec2 {
  unsigned char placeholder_1[2];
  unsigned short cardNum;
  unsigned char unknown[6];
  unsigned char placeholder_2[38];
  unsigned char keyA[6];
  unsigned char ACs[4]; //0xFF, 0x07, 0x80, 0x69
  unsigned char keyB[6];
};

unsigned short checkSum( unsigned char sec1[], unsigned short offset ) { //char[] is a pointer.
  short i = 0; unsigned short sum = 0;
  for ( ; i < 0x10; i++ ) {
    //printf("\n@%x: 0x%x", &sec1+i+offset, *(sec1+i+offset) );
    sum += *(sec1+i+offset);
  }
  //printf("\nChecksum = 0x%x", sum );
  return sum;
}

unsigned char verifySum( unsigned short sum ) {
  unsigned short all = 0, i = 0;
  for ( ; i < 0x100; i++ ) {
    all = i * 0x100 + 0xff;
    if ( sum == all ) return 1;
  }
  return 0;
}

void printHex( unsigned short size, unsigned char contentP[], unsigned char showhat ) {
  unsigned short i = 0;
  for ( ; i < size; i++ ) {
    if ( showhat == 1 ) printf( "%c", *(contentP + i));
    else if ( showhat == 2 ) {
      if ( *(contentP + i) < 0x10 ) printf( "0x0%x ", *(contentP + i) );
      else printf( "0x%x", *(contentP + i) );
    } else {
      if ( *(contentP + i) < 0x10 ) printf( "0%x", *(contentP + i) );
      else printf( "%x", *(contentP + i) );
    }
  }
}

int main( int argc, char* argv[] ){
  printf( "\nZX Card File Dump Reader\n");
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

  fseek( dump, 0, SEEK_SET ); //seek to the file head
  if ( dumpsize == 192 || dumpsize == 1024 ) {
    //read file into structs
    struct zxcard_sec0 sec0;
    fread( &sec0, 64, 1, dump ); //size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
    struct zxcard_sec1 sec1;
    fread( &sec1, 64, 1, dump );
    struct zxcard_sec2 sec2;
    fread( &sec2, 64, 1, dump );

    printf("\n\n------------Sector 0------------");
    printf("\nCard UID: "); printHex( 5, &sec0.UID, 0 );
    printf("\nCard default sign: "); printHex( 8, &sec0.defaultSignature, 1 );
    printf("\nSec0 keyA: "); printHex( 6, &sec0.keyA, 0 );
    printf("\nSec0 keyB: "); printHex( 6, &sec0.keyB, 0 );
    printf("\nSec0 ACs: "); printHex( 4, &sec0.ACs, 0 );



    printf("\n\n------------Sector 1------------");

    unsigned short offset = 0x00; unsigned char hasE = 0;
    unsigned short ac = checkSum( &sec1, 0 ), bc = checkSum( &sec1, 0x10 );
    //Zone A checksum
    printf("\nZone A checksum = 0x%x, ", ac);
    if ( !verifySum(ac) ) {
      printf("failed. Skipping."); offset = 0x10; hasE = 1;
    } else {
      printf("OK.");
    }
    printf("\nZone B checksum = 0x%x, ", bc);
    //Zone B checksum
    if ( !verifySum(bc) ) {
      printf("failed. Skipping."); offset = 0x00; hasE = 1;
    } else {
      printf("OK.");
    }
    //Check corruption
    if ( !verifySum(ac) && !verifySum(bc) ) {
      printf("\n\nEither your zx card or the file dump was corrupt.\nTerminating."); return -1;
    }
    //Check total payment count and select A or B
    if ( hasE == 0 && sec1.countA > sec1.countB ) offset = 0x10;

    if ( offset == 0x00 ) printf("\n\n--- Zone A ---"); else printf("\n\n--- Zone B ---");
    printf("\nLast balance: RMB ");
    printHex( 2, &sec1.decBalanceIntA + offset / 2, 0 ); printf("."); // XXX: /2 for dword (?)
    printHex( 1, &sec1.decBalanceFloatA + offset, 0 );
    printf("\nLast payment at POS "); printHex( 1, &sec1.payPosIdA + offset, 0 );
    printf("\nContinuous payment "); printHex( 1, &sec1.seqPayCountA + offset, 0 ); printf(" times");
    printf("\nLast total payment %d times", ntohs(*( &sec1.countA + offset / 2 )));
    //vice versa for another zone
    if ( hasE == 0 ) {
      if ( offset == 0x00 ) printf("\n\n--- Zone B ---"); else printf("\n\n--- Zone A ---");
      printf("\nCurrent balance: RMB ");
      printHex( 2, &sec1.decBalanceIntB - offset / 2, 0 ); printf(".");
      printHex( 1, &sec1.decBalanceFloatB - offset, 0 );
      printf("\nCurrent payment at POS "); printHex( 1, &sec1.payPosIdB - offset, 0 );
      printf("\nContinuous payment "); printHex( 1, &sec1.seqPayCountB - offset, 0 ); printf(" times");
      printf("\nTotal payment %d times", ntohs(*( &sec1.countB - offset / 2 )));
    }

    printf("\n\n--- Zone C ---");
    printf("\nCard No.%d", ntohs(sec1.cardNum) + 0x013328c6 );
    printf("\nSec0 keyA: "); printHex( 6, &sec0.keyA, 0 );
    printf("\nSec0 keyB: "); printHex( 6, &sec0.keyB, 0 );
    printf("\nSec0 ACs: "); printHex( 4, &sec0.ACs, 0 );
    //printf("\n\n----------Sector 1----------");
  } else {
    printf( "\nWrong file size. Must be 192B (0~2 sectors) or 1KB (0~15 sectors)." ); return 3;
  }
}

/* old fault method ?
unsigned short checksum = 0; unsigned char t;
for ( int i = 0; i < 0x10; i++ ) {
  memcpy( &t, &sec1.placeholder_1A-7 + offset + i, 1);
  printf("\n@%x: %x,%d",&sec1.placeholder_1A-7 + offset + i, t, sizeof(*(&sec1.placeholder_1A-7 + offset + i)));
  //XXX: sizeof(*(&sec1.placeholder_1A-7 + offset + i) = 1, but sizeof(*(&sec1.countA + offset + i) = 2 ????????
  checksum += t; //*( &sec1.countA + offset + i );
}*/
