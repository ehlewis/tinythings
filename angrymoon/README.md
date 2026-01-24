
## About
My e-ink board is a WaveShare 2.13 inch. On the flex ribbon cable we can see a marking of "FPC-A002 20.04.08"

The RTC module is a DS3232

I am compiling in Arduino with the preset: ESP32-WROOM-DA Module

## Hardware setup


Cable Connections
|ESP32     | Harness Color  | e-ink | RTC |
|----------|----------------|-------|-----|
|3v3       | Grey           | VCC   | VCC |
|GND       | Brown          | GND   | GND |
|23        | Blue           | DIN   |     |
|18        | Yellow         | CLK   |     |
|5         | Orange         | CS    |     |
|TX2/GPIO17| Green          | DC    |     |
|RX2/GPIO16| White          | RST   |     |
|D4        | Purple         | BUSY  |     |
|D33       |                |       | SQW |
|D26       |                |       | SCL |
|D25       |                |       | SDA |




## Libraries:
Adafruit_BusIO
Adafruit_GFX_Library
GxEPD2
ArduinoJson


## Image generation
>>>>>>> Stashed changes

https://github.com/ZinggJM/GxEPD2/tree/master

GxEPD2_Example.ino

line 200 of GxEPD2_display_selection.h
GxEPD2_BW<GxEPD2_213_GDEY0213B74, GxEPD2_213_GDEY0213B74::HEIGHT> display(GxEPD2_213_GDEY0213B74(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEY0213B74 122x250, SSD1680, (FPC-A002 20.04.08)


Original display: 250 × 122
Original XBM: 250 x 122


### Export as XBitmap

Cropped image: 94 × 94
Cropped left offset: 76
Cropped top offset: 15

At this stage it is time to export one of our XBitmaps: the one with with the black data.

Open the Export dialog by selecting File -> Export As... To save as an XBitmap image, enter a filename with the .xbm extension.

Be aware that the name you choose here will determine the name of the image in the code later on. A good name for our example image might be  parrot_black.xbm.

After pressing Export, another "Export Image as XBM" dialog will appear. No changes are needed here, and you can click Export to continue.

The XBitmap format only needs slight modification to be used with Adafruit_GFX.

The .xbm extension needs to be changed to .h (Note: this step is not required if you are using an alternative Arduino IDE such as vs-code). Place the XBitmap file (now with .h ending) in the same directory as your .ino file and it should appear in the Arduino IDE.

Inside this .h file, the data type of the "image bits array" needs to be changed. Add 
`PROGMEM const` 
before the static unsigned char xxxx_bits[] = { part. This step specifies that the XBitmap data must be stored in the flash memory space, not in the valuable SRAM space. This is a requirement of the drawXBitmap() method.

You can now include your xbitmap files as a headers

#include "parrot_black.h"
Whenever your are ready, you can draw them with the drawXBitmap() method.

Here, we are drawing from the top left corner (0,0), with the width and height that are automatically set in our XBM .h file

panel.fillScreen(panel.WHITE);
panel.drawXBitmap(0, 0, parrot_black_bits, parrot_black_width, panel.BLACK);


Since we have an offset for these images we need to Embed these offsets in a struct

Define a structure for each cropped bitmap:
`
struct MoonImage {
    const uint8_t* data;     // PROGMEM bitmap
    uint16_t width;
    uint16_t height;
    int16_t offsetX;
    int16_t offsetY;
};
`

Then define one per phase:

extern const uint8_t waxing_crescent_bits[] PROGMEM;
`
MoonImage waxing_crescent = {
    waxing_crescent_bits,
    96, 96,    // cropped width & height
    77, 13     // offset to center
};
`
and then draw it
`
void drawMoon(const MoonImage& img)
{
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);

        int16_t x = (EPAPER_W - img.width) / 2 + img.offsetX;
        int16_t y = (EPAPER_H - img.height) / 2 + img.offsetY;

        display.drawXBitmap(x, y, img.data, img.width, img.height, GxEPD_BLACK);
    }
    while (display.nextPage());
}
`


## Useful links

https://github.com/todd-herbert/heltec-eink-modules/blob/main/docs/XBitmapTutorial/3color.md
https://forum.arduino.cc/t/gxepd-gxepd2-h-bitmapformat-need-info-in-order-to-make-my-own-images/1193845/3




## Troubleshooting

Do NOT call http.getString() before http.getStream() or getStream() will be empty and fail with DeserializationError::EmptyInput

If you build unsucessfully and get odd errors such as 

\Wificonfiguration.c:2:
\Wificonfiguration.h:7:1: error: unknown type name 'class'
\Wificonfiguration.h:7:25: error: expected '=', ',', ';', 'asm' or '__attribute__' before '{' token
\Wificonfiguration.c:3:18: error: expected '=', ',', ';', 'asm' or '__attribute__' before ':' token
\Wificonfiguration.c:8:18: error: expected '=', ',', ';', 'asm' or '__attribute__' before ':' token

It may be looking at a .c file which no longer exists but is cached. remove the temp cache directory which will be somewhere around /private/var/folders/qn/1115z7gx3qs12msnxk8310fh0000gn/T
You can see where is storing this by finding an #include in the arduino app and hovering over it. It will show where the cache is 

