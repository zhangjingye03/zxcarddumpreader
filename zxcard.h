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
      else printf( "0x%x ", *(contentP + i) );
    } else {
      if ( *(contentP + i) < 0x10 ) printf( "0%x", *(contentP + i) );
      else printf( "%x", *(contentP + i) );
    }
  }
}
