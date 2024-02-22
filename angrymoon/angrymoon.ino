#include "GxEPD2_functions.h"


void setup()
{
  display_init();
  
  // first update should be full refresh
  helloWorld();
  delay(1000);
  printText("CUSTom Text!!");
  // partial refresh mode can be used to full screen,
  // effective if display panel hasFastPartialUpdate
  delay(1000);
  clear_screen();
  delay(1000);
  helloFullScreenPartialMode();
  delay(1000);
  //stripeTest(); return; // GDEH029Z13 issue
  helloArduino();
  delay(1000);
  helloEpaper();
  delay(1000);
  //helloValue(123.9, 1);
  //delay(1000);
  showFont("FreeMonoBold9pt7b", "FreeMonoBold9pt7b");
  delay(1000);
  /*
  if (display.epd2.WIDTH < 104)
  {
    showFont("glcdfont", 0);
    delay(1000);
  }
  */
  //drawGrid(); return;
  drawBitmaps();
  drawGraphics();
  //return;

  if (does_support_partial_update())
  {
    showPartialUpdate();
    delay(1000);
  } // else // on GDEW0154Z04 only full update available, doesn't look nice
  //drawCornerTest();
  //showBox(16, 16, 48, 32, false);
  //showBox(16, 56, 48, 32, true);
  poweroff_display();
  deepSleepTest();
#if defined(ESP32) && defined(_GxBitmaps1304x984_H_)
  drawBitmaps1304x984();
  poweroff_display();
#endif
  Serial.println("setup done");
  end_display();
}

void loop()
{
}

// note for partial update window and setPartialWindow() method:
// partial update window size and position is on byte boundary in physical x direction
// the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
// see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()

