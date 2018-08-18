import board
from digitalio import DigitalInOut, Direction
import lowpower
import time
# put the m0 to sleep on pin D12
lowpower.sleep(board.D12)
# This part of the script runs after the D12 pin goes from LOW to HIGH
led = DigitalInOut(board.D13)
led.direction = Direction.OUTPUT
on = True
# Blink on for 200 ms 5 times.
for i in range(10):
    led.value = on
    time.sleep(.2)
    on = not on
