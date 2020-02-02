/*******************************************************************************
Based on segment-lcd-with-ht1622 from anxzhu (2016-2018)
Follow of MartyMacGyver / LCD_HT1622_16SegLcd work (https://github.com/MartyMacGyver/LCD_HT1622_16SegLcd)

This file is part of the HT1622 arduino library, and thus under the MIT license.
More info on the project and the license conditions on :
https://github.com/radenko/ht1622-arduino

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include <Arduino.h>
#include "HT1622.h"

//#define SERIAL_DEBUG Serial

void HT1622::changeBits(uint16_t src, uint16_t &dst, uint8_t srcBit, uint8_t dstBit) {
    int srcMask = 1 << srcBit;
    int dstMask = 1 << dstBit;

    if (src & srcMask) {
        dst |= dstMask;
    } else {
        dst &= (~dstMask);
    }
}

HT1622::HT1622():_cs_p(-1), _wr_p(-1), _data_p(-1), _backlight_p(-1) {
}

HT1622::HT1622(int cs, int wr, int data, int backlight):_cs_p(cs), _wr_p(wr), _data_p(data), _backlight_p(backlight) {
}

void HT1622::begin(SPIClass &spi, int cs, int backlight) {
    this->_spi = &spi;
    this->_cs_p = cs;
    this->_backlight_p = backlight;

    if (this->_cs_p != -1) {
        pinMode(this->_cs_p, OUTPUT);
    }
    if (this->_backlight_p != -1) {
        pinMode(this->_backlight_p, OUTPUT);
    }

    delay(50);

    _backlight_en=true;
    config();
}

void HT1622::begin(int cs, int wr, int data, int backlight) {
    this->_cs_p = cs;
    this->_wr_p = wr;
    this->_data_p = data;
    this->_backlight_p = backlight;

//    pinMode(this->_cs_p, OUTPUT);
    pinMode(this->_wr_p, OUTPUT);
    pinMode(this->_data_p, OUTPUT);
    if (this->_cs_p != -1) {
        pinMode(this->_cs_p, OUTPUT);
    }
    if (this->_backlight_p != -1) {
        pinMode(this->_backlight_p, OUTPUT);
    }

//    pinMode(this->_backlight_p, OUTPUT);
    delay(50);

    _backlight_en=true;
    config();
}

void HT1622::backlight()
{
    if (_backlight_en)
        digitalWrite(_backlight_p, HIGH);
    delay(1);
}

void HT1622::noBacklight()
{
    if(_backlight_en)
        digitalWrite(_backlight_p, LOW);
    delay(1);
}

void HT1622::beginTransfer() {
    digitalWrite(_cs_p, LOW);
    uint8_t _outBits = 0;
    uint8_t _outBuff = 0;
}

void HT1622::endTransfer() {
    if (_outBits!=0) {
        this->sendBitsSpi(0, 8 - this->_outBits);
        uint8_t _outBits = 0;
        uint8_t _outBuff = 0;
    }

    digitalWrite(_cs_p, HIGH);
    delay(1);
}

void HT1622::clear() {
    this->seekLeft();
    this->allSegments(0);
    if (this->digitBuffer != NULL) {
        memset(this->digitBuffer, 0, this->digitBufferSize * sizeof(this->digitBuffer[0]));
    }
}

void HT1622::sendBitsSpi(uint16_t data, uint8_t bits, boolean LSB_FIRST)
{
    while (bits) {
        int res = 8 - _outBits;
        if (res > bits) {
            res = bits;
        }

        _outBuff = _outBuff << res;

        _outBuff |= data >> (bits - res);
        _outBits += res;
        bits -= res;
        data = data & (0xFFFF >> (16 - bits));

        if (_outBits == 8) {
            //SERIAL_DEBUG.print("--Writing bits: ");
            //SERIAL_DEBUG.println(this->_outBuff, BIN);
            this->_spi->transfer(this->_outBuff);
            _outBuff = 0;
            _outBits = 0;
        }
    }
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Send up to 16 bits, MSB (default) or LSB
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void HT1622::sendBits(uint16_t data, uint8_t bits, boolean LSB_FIRST)
{
    if (this->_spi != NULL) {
        this->sendBitsSpi(data, bits, LSB_FIRST);
    } else {
        uint16_t mask;
        mask = (LSB_FIRST ? 1 : 1 << bits - 1);
        //printf("Writing %d bits of 0x%04x with mask 0x%04x in %s\n", bits, data, mask, LSB_FIRST?"LSB":"MSB");

        //SERIAL_DEBUG.print("++Writing bits: ");
        for (uint8_t i = bits; i > 0; i--) {
            //SERIAL_DEBUG.print(data & mask ? 1 : 0);

            digitalWrite(this->_wr_p, LOW);
            delayMicroseconds(HT1622_WRITE_DELAY_USECS);
            data & mask ? digitalWrite(this->_data_p, HIGH) : digitalWrite(this->_data_p, LOW);
            delayMicroseconds(HT1622_WRITE_DELAY_USECS);
            digitalWrite(this->_wr_p, HIGH);
            delayMicroseconds(HT1622_WRITE_DELAY_USECS);
            LSB_FIRST ? data >>= 1 : data <<= 1;
        }
        //SERIAL_DEBUG.println();
        delayMicroseconds(HT1622_WRITE_DELAY_USECS);
    }
}

void HT1622::wrCommand(unsigned char cmd) {  //100
    this->beginTransfer();
    this->sendBits(0b100, 3);
    this->sendBits(cmd, 8);
    this->sendBits(1, 1);
    //this->sendBits(0, 4);
    this->endTransfer();
}

void HT1622::config()
{
    wrCommand(CMD_SYS_EN);
    wrCommand(CMD_RC_INT);
    this->clear();
    wrCommand(CMD_LCD_ON); // Should turn it on
    delay(50);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void HT1622::allSegments(uint8_t state)
{
    const int SegDelay = 10;

    for (uint8_t addr = 0x00; addr < 0x3F; addr++)
    {
        wrData(addr, (state ? 0xff : 0x00));
    }
    delay(SegDelay);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void HT1622::wrData(uint8_t addr, uint16_t sdata, uint8_t bits)
{
//    SERIAL_DEBUG.print("Writing data ");
//    SERIAL_DEBUG.print(sdata);
//    SERIAL_DEBUG.print(" to ");
//    SERIAL_DEBUG.println(addr);

    this->beginTransfer();
    this->sendBits(0b101, 3);
    this->sendBits(addr, 6);
    this->sendBits(sdata, bits, HT1622_MSB_FORMAT);
    //this->sendBits(0,  8 - ((3+6+bits) % 8));
    this->endTransfer();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void HT1622::wrBuffer()
{
//    SERIAL_DEBUG.print("Writing  buffer");

    this->beginTransfer();
    this->sendBits(0b101, 3);
    this->sendBits(0, 6);
    for (int i=this->digitBufferSize - 1; i >= 0; i--) {
        this->sendBits(this->digitBuffer[i], 16);
    }
    this->endTransfer();
}

void HT1622::writeSegment(const uint8_t nr, const char value) {
    uint16_t data;
    if (value < 0x20) {
        data = this->_charset[0];
    } else if (value >= (this->_charsetLen + 0x20)) {
        data = this->_charset[this->_charsetLen - 1];
    } else {
        data = this->_charset[value - 0x20];
    }

    if (this->digitBuffer != NULL) {
        this->digitBuffer[nr] = this->_charAdapter == NULL ? data : this->_charAdapter(data);
        if (!this->noRedraw) {
            this->wrBuffer();
        }
    } else {
        wrData(_digitAddr[nr], (this->_charAdapter == NULL ? data : this->_charAdapter(data)), 16);
    }
}

void HT1622::setDigitAddr(const uint8_t *digitAddr, const uint8_t digitAddrCount) {
    this->_digitAddr = digitAddr;
    this->_digitAddrCount = digitAddrCount;
    this->seekLeft();
}

void HT1622::setDigitBuff(uint16_t *buffer, const uint8_t bufferSize) {
    this->digitBuffer = buffer;
    this->digitBufferSize = bufferSize;
}

void HT1622::setDigitBuffClear(uint16_t *buffer, const uint8_t bufferSize) {
    this->setDigitBuff(buffer, bufferSize);
    memset(this->digitBuffer, 0, this->digitBufferSize * sizeof(buffer[0]));
}

void HT1622::seekLeft() {
    this->charPos = 0;
}

void HT1622::seekRight() {
    this->charPos = this->_digitAddrCount - 1;
}

void HT1622::setCharAdapter(uint16_t (*charAdapter)(uint16_t character)) {
    this->_charAdapter = charAdapter;
}

void HT1622::setCharset(const uint8_t *digitAddr, const uint8_t digitAddrCount) {
    this->_digitAddr = digitAddr;
    this->_digitAddrCount = digitAddrCount;
}

size_t HT1622::write(unsigned char ch) {
    this->writeSegment(this->charPos, ch);
    this->charPos++;
    if (this->charPos >= this->_digitAddrCount) {
        this->charPos = 0;
    }
}