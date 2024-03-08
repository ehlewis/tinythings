# tinythings


My board is a WaveShare 2.13 inch. On the flex ribbon cable we can see a marking of "FPC-A002 20.04.08"

I am compiling in Arduino with the preset: ESP32-WROOM-DA Module


https://github.com/ZinggJM/GxEPD2/tree/master

GxEPD2_Example.ino

line 200 of GxEPD2_display_selection.h
GxEPD2_BW<GxEPD2_213_GDEY0213B74, GxEPD2_213_GDEY0213B74::HEIGHT> display(GxEPD2_213_GDEY0213B74(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEY0213B74 122x250, SSD1680, (FPC-A002 20.04.08)

Cable Connections
ESP32       |   Color     | e-ink display
3v3             Grey        VCC
GND             Brown       GND
23              Blue        DIN
18              Yellow      CLK
5               Orange      CS
TX2/GPIO17      Green       DC
RX2/GPIO16      White       RST 
4               Purple      BUSY


Installed Ardiuno Libraries:
Adafruit_BusIO
Adafruit_GFX_Library
GxEPD2


If you build unsucessfully and get odd errors such as 

\Wificonfiguration.c:2:
\Wificonfiguration.h:7:1: error: unknown type name 'class'
\Wificonfiguration.h:7:25: error: expected '=', ',', ';', 'asm' or '__attribute__' before '{' token
\Wificonfiguration.c:3:18: error: expected '=', ',', ';', 'asm' or '__attribute__' before ':' token
\Wificonfiguration.c:8:18: error: expected '=', ',', ';', 'asm' or '__attribute__' before ':' token

It may be looking at a .c file which no longer exists but is cached. remove the temp cache directory which will be somewhere around /private/var/folders/qn/1115z7gx3qs12msnxk8310fh0000gn/T
You can see where is storing this by finding an #include in the arduino app and hovering over it. It will show where the cache is 


