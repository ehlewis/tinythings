https://github.com/todd-herbert/heltec-eink-modules/blob/main/docs/XBitmapTutorial/3color.md
https://forum.arduino.cc/t/gxepd-gxepd2-h-bitmapformat-need-info-in-order-to-make-my-own-images/1193845/3



Export as XBitmap

At this stage it is time to export one of our XBitmaps: the one with with the black data.

Open the Export dialog by selecting File -> Export As... To save as an XBitmap image, enter a filename with the .xbm extension.

Be aware that the name you choose here will determine the name of the image in the code later on. A good name for our example image might be  parrot_black.xbm. Remember that you are also going to make an accompanying parrot_red.xbm file shortly.

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