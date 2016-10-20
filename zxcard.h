#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

struct zxcard_sec0 {
  unsigned char UID[4];
  unsigned char bitCheckCount;
  unsigned char SAK; //0x08
  unsigned short ATQA; //0x04, 0x00
  unsigned char defaultSignature[8]; //bcdefghi(2014), it varies after 2015
  unsigned char placeholder_2[32]; //0x00
  unsigned char keyA[6];
  unsigned char ACs[4]; //0xFF, 0x07, 0x80, 0x69
  unsigned char keyB[6];
};

struct zxcard_sec1 {
  // block 0, 0h~Fh
  unsigned short countA;
  unsigned char unknown_1A[2]; //not checksum!
  unsigned char decBalanceIntA[2];
  unsigned char decBalanceFloatA;
  unsigned char placeholder_1A; //0x00
  unsigned char seqPayCountA;
  unsigned char payMonthA;
  unsigned char placeholder_2A; //0x00
  unsigned char unknown_2A;
  unsigned char placeholder_3A; //0x00
  unsigned char unknown_3A;
  unsigned char unknown_4A;
  unsigned char checksumA;
  // block 1, 10h~1Fh
  unsigned short countB;
  unsigned char unknown_1B[2]; //not checksum!
  unsigned char decBalanceIntB[2];
  unsigned char decBalanceFloatB;
  unsigned char placeholder_1B; //0x00
  unsigned char seqPayCountB;
  unsigned char payMonthB;
  unsigned char placeholder_2B; //0x00
  unsigned char unknown_2B;
  unsigned char placeholder_3B; //0x00
  unsigned char unknown_3B;
  unsigned char unknown_4B;
  unsigned char checksumB;
  // block 2, 20h~2Fh
  unsigned short cardNum;
  unsigned char placeholder[9]; //0x12, 0x34, 0x01, 0x02, 0x01, 0xFF, 0x00, 0x00, 0x00. 0x17, 0x08, 0x01, 0x00, 0x00
  unsigned char distributionTime;
  unsigned char placeholder2[4]; //0x08, 0x01, 0x00, 0x00
  // block 3, 30h~3Fh
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

unsigned char getBitCheckCount ( unsigned char UID[4] ) {
  short i = 0; unsigned char bcc = *UID;
  for ( i = 1; i < 4; i++ ) {
    bcc = bcc ^ *(UID + i);
  }
  return bcc;
}

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
  unsigned short all = 0, i = 1; // Only 0x0 ~ 0xe add up to checksum
  for ( ; i < 0xf; i++ ) {
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
      else printf( "0x%x ", *(contentP + i) );
    } else {
      if ( *(contentP + i) < 0x10 ) printf( "0%x", *(contentP + i) );
      else printf( "%x", *(contentP + i) );
    }
  }
}

float getBalance( unsigned char zonedata[16] ) {
  /*char test[2];
  sprintf(test, "%x", **(&sec1.decBalanceIntA+offset/2));
  unsigned short bint = 0;
  sscanf( test, "%d", &bint );
  printf("\n\nx%x %x %d\n\n",**(&sec1.decBalanceIntA+offset/2),*(&sec1.decBalanceFloatA+offset-1),bint*100);*/
  char x100[2], x1[2], xp01[2];
  sprintf( x100, "%x", *(zonedata + 0x04) );
  //printf("\nx100:%c", *x100 );
  sprintf( x1, "%x", *(zonedata + 0x05) );
  //printf("\nx1:%c", *x1 );
  sprintf( xp01, "%x", *(zonedata + 0x06) );
  //printf("\nxp01:%c", *xp01 );
  unsigned int i100, i1, ip01; //XXX: Why cannot use short???
  sscanf( x100, "%d", &i100 );
  //printf("\ni100:%d", i100 );
  sscanf( x1, "%d", &i1 );
  //printf("\ni1:%d", i1 );
  sscanf( xp01, "%d", &ip01 );
  //printf("\nip01:%d", ip01 );
  return i100 * 100 + i1 + ip01 * 0.01;
}
