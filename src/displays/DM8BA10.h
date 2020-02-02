//
// Created by Radovan Kadlic on 30/01/2020.
//

#ifndef HT1622_DM8BA10_H
#define HT1622_DM8BA10_H

#include "HT1622.h"
/*
Bit numbering via a little-endian uint16_t mapped to 4 addresses:
    /-----------\   /-----------\
   ||     F     || ||     7     ||
    \-----------/   \-----------/
   /-\ /--\      /-\      /--\ /-\
  |   |\   \    |   |    /   /|   |
  |   | \   \   |   |   /   / |   |
  | D |  \ E \  | 6 |  / 4 /  | 5 |
  |   |   \   \ |   | /   /   |   |
  |   |    \   \|   |/   /    |   |
   \-/      \--/ \-/ \--/      \-/
    /-----------\   /-----------\
   ||     C    || ||     3     ||
    \-----------/   \-----------/
   /-\      /--\ /-\ /--\      /-\
  |   |    /   /|   |\   \    |   |
  |   |   /   / |   | \   \   |   |
  | 9 |  / B /  | A |  \ 2 \  | 1 |
  |   | /   /   |   |   \   \ |   |
  |   |/   /    |   |    \   \|   |
   \-/ \--/      \-/      \--/ \-/
    /-----------\   /-----------\
   ||     8     || ||     0     ||
    \-----------/   \-----------/
*/

/*const uint16_t HT1622_SEG_CHARSET[] = {
        0x0000, 0x0440, 0x0060, 0xAA88, 0xB99B, 0xBCCB, 0xD385, 0x0080, //  !"#$%&'
        0x0044, 0x4400, 0xCCCC, 0x8888, 0x0040, 0x8008, 0x0001, 0x0440, // ()*+,-./
        0xABB3, 0x0032, 0x93A9, 0x81AB, 0x302A, 0xB18B, 0xB30B, 0x80A2, // 01234567
        0xB3AB, 0xB0AB, 0x0880, 0x0480, 0x8044, 0x8109, 0x4408, 0x1838, // 89:;<=>?
        0xA3B9, 0xB2AA, 0x85EB, 0x3311, 0x19B3, 0xB311, 0xB210, 0x331B, // @ABCDEFG
        0xA22A, 0x1991, 0x0323, 0xA244, 0x2301, 0x6262, 0x6226, 0x3333, // HIJKLMNO
        0xB238, 0x3337, 0xB23C, 0xB11B, 0x1890, 0x2323, 0x2640, 0x2626, // PQRSTUVW
        0x4444, 0xA828, 0x1551, 0x0891, 0x4004, 0x1980, 0x0404, 0x0101, // XYZ[\]^_
        0x4000, 0xCB00, 0xAB00, 0x8300, 0x8B80, 0x8701, 0x8898, 0xB980, // `abcdefg
        0xAA00, 0x0800, 0x0B80, 0x08C4, 0x0880, 0x8A0A, 0x8A00, 0x8B00, // hijklmno
        0xB280, 0xB880, 0x8200, 0xB900, 0x8888, 0x0B00, 0x0600, 0x0606, // pqrstuvw
        0x4444, 0x4840, 0x8500, 0x8891, 0x0440, 0x1988, 0x4040, 0xFFFF, // xyz{|}~
};*/

//Digit addresses starting from right
//const uint8_t DM8BA10_DIGIT_ADDR[] = {0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24};
const uint8_t DM8BA10_DIGIT_ADDR[] = {0x24, 0x20, 0x1C, 0x18, 0x14, 0x10, 0x0C, 0x08, 0x04, 0x00};
uint16_t DM8BA10_DIGIT_BUFF[ARRAY_SIZE(DM8BA10_DIGIT_ADDR)];

uint16_t DM8BA10_adaptChar(uint16_t src) {
    uint16_t dst = src;

    HT1622::changeBits(src, dst, 6, 4);
    HT1622::changeBits(src, dst, 7, 6);
    HT1622::changeBits(src, dst, 4, 7);

    HT1622::changeBits(src, dst, 10, 11);
    HT1622::changeBits(src, dst, 11, 10);
    HT1622::changeBits(src, dst, 15, 12);
    HT1622::changeBits(src, dst, 12, 15);

    return dst;
};

void DM8BA10_setup(HT1622 &ht1622) {
    ht1622.setDigitAddr(DM8BA10_DIGIT_ADDR, ARRAY_SIZE(DM8BA10_DIGIT_ADDR));
    ht1622.setDigitBuff(DM8BA10_DIGIT_BUFF, ARRAY_SIZE(DM8BA10_DIGIT_BUFF));
    ht1622.setCharAdapter(&DM8BA10_adaptChar);
}

#endif //HT1622_DM8BA10_H
