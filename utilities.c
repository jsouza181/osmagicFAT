
//Find the value of two bytes swapped for littleEndian and "concatenated".
unsigned int swapTwoBytes(unsigned int byteA, unsigned int byteB) {
  unsigned int ret;
  ret = byteB << 8;
  ret = ret | byteA;

  return ret;
}

//Same as swapTwoBytes but applied to four bytes.
unsigned int swapFourBytes(unsigned int byteA, unsigned int byteB,
                            unsigned int byteC, unsigned int byteD) {
  unsigned int ret;
  ret = 0;

  ret = byteD;
  ret = ret << 8;
  ret = ret | byteC;
  ret = ret << 8;
  ret = ret | byteB;
  ret = ret << 8;
  ret = ret | byteA;

  return ret;
}
