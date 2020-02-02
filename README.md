# HT1621 7 segment LCD Arduino library
Arduino library for lcds based on the HT1622 driver.

<img src="extras/photo.jpg" alt="photo" width="500">

## APIs reference

* `void begin(int cs_p, int wr_p, int data_p, [int backlight_p])`  
Starts the lcd with the pin assignement declared. The backlight pin is optional

* `void begin(SPIClass &SPI, [int cs_p], [int backlight_p])`  
Starts the lcd with the SPI. The backlight pin is optional

* `void clear()`  
Clears the display

* `void backlight()`  
Turns on the backlight 

* `void noBacklight()`  
Turns off the backlight

* `void write(unsigned char ch)`
Implements write method for Print interface. 

* `void noDisplay()`  
Turns off the display (doesn't turn off the backlight) 

* `void display()`  
Turns the display back on after it has been disabled by `noDisplay()`


## Internal functioning

Can drive any segmented display up to 16 segments.

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