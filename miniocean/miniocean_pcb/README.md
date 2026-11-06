#  MINIOCEAN

Small implementation of OLED display

## Hardware Configuration


Cable Connections
|ESP32     | MAX7219 LED Matrix | MPU6050 Accelerometer |
|----------|--------------------|-----------------------|
|3v3       | VCC                | VCC                   |
|GND       | GND                | GND                   |
|GPIO 18   | CLK                |                       |
|GPIO 5    | CS                 |                       |
|GPIO 23   | DIN                |                       |
|GPIO 22   |                    | SCL                   |
|GPIO 21   |                    | SDA                   |



## Installed Libraries

MD_MAX72xx
Adafruit_MPU6050
Adafruit BusIO


## Useful Links 


74hc595 shift register
https://www.youtube.com/watch?v=18qzPIYFGKk
https://www.electrodragon.com/product/16x16-led-display-module/
https://forum.arduino.cc/t/16x16-display/111552
https://forum.arduino.cc/t/16x16-led-matrix/315867

