import busio as io
import adafruit_ds3231
import time

import board
import pwmio

class Voltmeter_Display(object):

    def __init__(self, pin):
        ''' pin should be a pwmio.PWMOut '''
        self.pin = pwmio.PWMOut(pin, frequency = 1024, duty_cycle = 0)
        self.PWM_MAX = 65535


    def display_percentage(self, percentage):
        ''' percentage should be 0-1 '''
        self.pin.duty_cycle = int(percentage * self.PWM_MAX)
        print(self.pin.duty_cycle)


SCL = board.GP17
SDA = board.GP16
i2c = io.I2C(SCL, SDA)

rtc = adafruit_ds3231.DS3231(i2c)

# for setting the time
if False:
    t = time.struct_time((2021, 3, 28, 16, 6, 0, 6,-1, -1))

    rtc.datetime = t


# main loop

display = Voltmeter_Display(board.GP1)

print("started")
while True:
    print(rtc.datetime)

    display.display_percentage(rtc.datetime.tm_sec/60)


    time.sleep(0.1)