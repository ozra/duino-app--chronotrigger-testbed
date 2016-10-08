#ifndef UTILS_CONVERSION
#define UTILS_CONVERSION

char HexLut[17] = "0123456789ABCDEF";

void say_hex(U8 value){
  say(HexLut[(value >> 4) & 0x0F]);
  say(HexLut[value & 0x0F]);
}

#endif