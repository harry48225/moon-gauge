import busio as io
import adafruit_ds3231
import time

import board

SCL = board.GP17
SDA = board.GP16
i2c = io.I2C(SCL, SDA)

rtc = adafruit_ds3231.DS3231(i2c)

# for setting the time
if False:
    t = time.struct_time((2021, 3, 28, 16, 6, 0, 6,-1, -1))

    rtc.datetime = t

while True:
    print(rtc.datetime)
    time.sleep(10)