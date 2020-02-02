/*
 * Board: ETM8809K2-02
 * Manufacturer: http://www.canton-electronics.com
 * Chipset: HT1622 or equivalent
 *        LCD = 9 digit 16 seg + 3 dp + continuous backlight
 * Wiring:
 * VDD = 5VDC
 * K = GND
 * A = 3.3V or 5V
 * DATA, RW, CS =  5V logic, as below
 * LCD and RD not used
 *      Arduino is little-endian
 *      H162x commands and addresses are MSB-first (3 bit mode + 9 bit command code)
 * Note that the very last bit is X (don't care)
 * Data is LSB-first, in address-sequential 4-bit nibbles as desired.
 * Addressing (each address hold 4 bits):
 * Addr 0x00-0x03  = Digit 0
 * Addr 0x04-0x07  = Digit 2
 * Addr 0x08-0x0B  = Digit 4
 * Addr 0x0C-0x0F  = Digit 6
 * Addr 0x10-0x13  = Digit 8
 * Addr 0x14-0x17  = Digit 1
 * Addr 0x18-0x1B  = Digit 3
 * Addr 0x1C-0x1F  = Digit 5
 * Addr 0x21 bit 0 = Decimal 3
 * Addr 0x21 bit 1 = Decimal 2
 * Addr 0x22-0x25  = Digit 7
 * Addr 0x27 bit 0 = Decimal 1
 * Addr 0x27 bit 1 = Icon 'Hz'
 * Typical names for segments:
 *  /-----------\   /-----------\
 * ||    'a'    || ||    'b'    ||
 *  \-----------/   \-----------/
 *  /-\ /--\      /-\      /--\ /-\
 * |   |\   \    |   |    /   /|   |
 * |   | \   \   |   |   /   / |   |
 * |'h'|  \'k'\  |'m'|  /'n'/  |'c'|
 * |   |   \   \ |   | /   /   |   |
 * |   |    \   \|   |/   /    |   |
 *  \-/      \--/ \-/ \--/      \-/
 *  /-----------\   /-----------\
 *  ||    'u'    || ||    'p'    ||
 *  \-----------/   \-----------/
 *  /-\      /--\ /-\ /--\      /-\
 * |   |    /   /|   |\   \    |   |
 * |   |   /   / |   | \   \   |   |
 * |'g'|  /'t'/  |'s'|  \'r'\  |'d'|
 * |   | /   /   |   |   \   \ |   |
 * |   |/   /    |   |    \   \|   |
 *  \-/ \--/      \-/      \--/ \-/
 * /-----------\   /-----------\
 *||    'f'    || ||    'e'    ||
 * \-----------/   \-----------/
 */

#ifndef HT1622_LIBRARY_H
#define HT1622_LIBRARY_H

#include "Print.h"
#include "SPI.h"
#define ARRAY_SIZE(x) ((sizeof x) / (sizeof *x))

//const uint8_t DIGIT_ADDR[] = {0x00, 0x04, 0x08, 0x0C, 0x10, /**/0x14, 0x18, 0x1C, 0x20, 0x24};

// Based on a font presented at http://windways.org/personal_page/stockticker/
const uint16_t HT1622_SEG_CHARSET[] = {
        0x0000, 0x0023, 0x00A0, 0xAA88, 0xB99B, 0xBCCB, 0xD385, 0x0080, //  !"#$%&'
        0x0044, 0x4400, 0xCCCC, 0x8888, 0x0040, 0x8008, 0x0001, 0x0440, // ()*+,-./
        0x3773, 0x0062, 0x9339, 0x113b, 0xA02A, 0xB11B, 0xB30B, 0x1032, // 01234567
        0xb33b, 0xb03b, 0x0880, 0x0480, 0x8044, 0x8109, 0x4408, 0x1838, // 89:;<=>?
        0x3379, 0xB23A, 0x19BB, 0x3311, 0x19B3, 0xB311, 0xB210, 0x331B, // @ABCDEFG
        0xA22A, 0x1991, 0x0323, 0xA244, 0x2301, 0x6262, 0x6226, 0x3333, // HIJKLMNO
        0xB238, 0x3337, 0xB23C, 0xB11B, 0x1890, 0x2323, 0x2640, 0x2626, // PQRSTUVW
        0x4444, 0xA828, 0x1551, 0x0891, 0x4004, 0x1980, 0x0404, 0x0101, // XYZ[\]^_
        0x4000, 0xCB00, 0xAB00, 0x8300, 0x8B80, 0x8701, 0x8898, 0xB980, // `abcdefg
        0xAA00, 0x0800, 0x0B80, 0x08C4, 0x0880, 0x8A0A, 0x8A00, 0x8B00, // hijklmno
        0xB280, 0xB880, 0x8200, 0xB900, 0x8888, 0x0B00, 0x0600, 0x0606, // pqrstuvw
        0x4444, 0x4840, 0x8500, 0x8891, 0x2200, 0x1988, 0x4040, 0xFFFF, // xyz{|}~
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// HT162x commands (Start with 0b100, ends with one "don't care" byte)
#define  CMD_SYS_DIS  0x00  // SYS DIS    (0000-0000-X) Turn off system oscillator, LCD bias gen [Default]
#define  CMD_SYS_EN   0x01  // SYS EN     (0000-0001-X) Turn on  system oscillator
#define  CMD_LCD_OFF  0x02  // LCD OFF    (0000-0010-X) Turn off LCD display [Default]
#define  CMD_LCD_ON   0x03  // LCD ON     (0000-0011-X) Turn on  LCD display
#define  CMD_RC_INT   0x10  // RC INT     (0001-10XX-X) System clock source, on-chip RC oscillator
#define  CMD_BIAS_COM 0x29  // BIAS & COM (0010-10X1-X) 1/3 bias, 4 commons // HT1621 only

#define HT1622_MSB_FORMAT  false
#define HT1622_LSB_FORMAT  true
#define HT1622_WRITE_DELAY_USECS 2  // Tclk min. on data sheet - overhead is more than this at low clock speeds

class HT1622: public Print {
    private:
        int _cs_p, _wr_p, _data_p, _backlight_p;
        SPIClass *_spi;

        bool _backlight_en;
        const uint8_t *_digitAddr;
        uint8_t _digitAddrCount;
        const uint16_t *_charset = (const uint16_t*)&HT1622_SEG_CHARSET;
        uint8_t _charsetLen = ARRAY_SIZE(HT1622_SEG_CHARSET);
        uint8_t _outBits = 0;
        uint8_t _outBuff = 0;

        uint16_t (*_charAdapter)(uint16_t character) = NULL;

        uint8_t charPos = 0;

        void beginTransfer();
        void endTransfer();

        void sendBits(uint16_t data, uint8_t bits, boolean LSB_FIRST = HT1622_MSB_FORMAT);
        void sendBitsSpi(uint16_t data, uint8_t bits, boolean LSB_FIRST = HT1622_MSB_FORMAT);
    public:
        HT1622();
        HT1622(int cs, int wr, int data, int backlight = -1);

        void setDigitAddr(const uint8_t *digitAddr, const uint8_t digitAddrCount);
        void setCharset(const uint8_t *digitAddr, const uint8_t digitAddrCount);
        void setCharAdapter(uint16_t (*charAdapter)(uint16_t character));

        void seekLeft();
        void seekRight();

        static void changeBits(uint16_t src, uint16_t &dst, uint8_t srcBit, uint8_t dstBit);
        uint16_t adaptChar(uint16_t src);

        void begin(int cs, int wr, int data, int backlight = -1);
        void begin(SPIClass &spi, int cs, int backlight = -1);
        void backlight();
        void noBacklight();
        void wrCommand(unsigned char CMD);
        void wrData(uint8_t addr, uint16_t sdata, uint8_t bits = 4);
        void config();

        void clear();

        void writeSegment(const uint8_t nr, const char value);
        void allSegments(uint8_t state);

        virtual size_t write(uint8_t);
};

#endif