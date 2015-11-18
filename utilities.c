
//Find the value of two bytes swapped for littleEndian and "concatenated".
int swapBytes(int leftByte, int rightByte) {
  rightByte = rightByte << 8;
  rightByte = rightByte | leftByte;

  return rightByte;
}
