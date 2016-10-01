#include "zxcard.h"

int main( int argc, char *argv[] ){
  printf( "\nZX Card File Dump Reader\n");
  if ( argc < 2 ) {
    printf( "No input file path specified.\n\nUsage: %c <file>\nThe parameter <file> is your zx card dump file\nEither 1KB version with all sectors or 192B version with first three blocks is acceptable.", argv[0]); return 1;
  }

  FILE* dump;
  dump = fopen( argv[1], "rb" ); //b for binary
  if ( dump == NULL ) {
    printf( "\nFile %c not found.", *argv[1]); return 2;
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

    printf("\n\n------------Sector 0------------\n");
    printf("\nCard UID: "); printHex( 5, &sec0.UID, 0 );
    printf("\nCard default sign: "); printHex( 8, &sec0.defaultSignature, 1 );
    printf("\nSec0 keyA: "); printHex( 6, &sec0.keyA, 0 );
    printf("\nSec0 keyB: "); printHex( 6, &sec0.keyB, 0 );
    printf("\nSec0 ACs: "); printHex( 4, &sec0.ACs, 0 );



    printf("\n\n------------Sector 1------------\n");

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
    printf("\nLast balance: RMB "); printf("%.2f", getBalance( &sec1.decBalanceFloatA + offset - 6 )); //prevent from dword addr calculation
    printf("\nLast payment at month "); printHex( 1, &sec1.payPosIdA + offset, 0 );
    //printf("\nContinuous payment "); printHex( 1, &sec1.seqPayCountA + offset, 0 ); printf(" times");
    printf("\nLast total payment %d times", ntohs(*( &sec1.countA + offset / 2 )));
    float balance1 = getBalance( &sec1.decBalanceFloatA + offset - 6 );
    //vice versa for another zone
    if ( hasE == 0 ) {
      if ( offset == 0x00 ) printf("\n\n--- Zone B ---"); else printf("\n\n--- Zone A ---");
      printf("\nCurrent balance: RMB "); printf("%.2f", getBalance( &sec1.decBalanceFloatB - offset - 6 ));
      printf("\nCurrent payment at month "); printHex( 1, &sec1.payPosIdB - offset, 0 );
      //printf("\nContinuous payment "); printHex( 1, &sec1.seqPayCountB - offset, 0 ); printf(" times");
      printf("\nTotal payment %d times", ntohs(*( &sec1.countB - offset / 2 )));
      float balance2 = getBalance( &sec1.decBalanceFloatB - offset - 6 );
      printf("\n\nLast time you spent RMB %.2f", balance1 - balance2 );
    }

    printf("\n\n--- Zone C ---");
    printf("\nCard No. %d", ntohs(sec1.cardNum) + 0x013328c6 );
    printf("\nSec1 keyA: "); printHex( 6, &sec1.keyA, 0 );
    printf("\nSec1 keyB: "); printHex( 6, &sec1.keyB, 0 );
    printf("\nSec1 ACs: "); printHex( 4, &sec1.ACs, 0 );

    printf("\n\n------------Sector 2------------\n");
    printf("\nCard No. %d", ntohs(sec2.cardNum) + 0x013328c6 );
    if ( sec1.cardNum != sec2.cardNum ) { printf( ", different from sector 1 !"); }
    printf("\nUnknown characteristic:\n");
    printHex( 6, &sec2.unknown, 2 );
    printf("\nSec2 keyA: "); printHex( 6, &sec2.keyA, 0 );
    printf("\nSec2 keyB: "); printHex( 6, &sec2.keyB, 0 );
    printf("\nSec2 ACs: "); printHex( 4, &sec2.ACs, 0 );
    printf("\n\n-------------E N D--------------\n");
  } else {
    printf( "\nWrong file size. Must be 192B (0~2 sectors) or 1KB (0~15 sectors)." ); return 3;
  }

  fclose(dump);
  return 0;
}

/* old fault method ?
unsigned short checksum = 0; unsigned char t;
for ( int i = 0; i < 0x10; i++ ) {
  memcpy( &t, &sec1.placeholder_1A-7 + offset + i, 1);
  printf("\n@%x: %x,%d",&sec1.placeholder_1A-7 + offset + i, t, sizeof(*(&sec1.placeholder_1A-7 + offset + i)));
  //XXX: sizeof(*(&sec1.placeholder_1A-7 + offset + i) = 1, but sizeof(*(&sec1.countA + offset + i) = 2 ????????
  checksum += t; //*( &sec1.countA + offset + i );
}*/
