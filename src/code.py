import board
from digitalio import DigitalInOut, Direction, Pull
import time
import mymodule
led = DigitalInOut(board.D13)

led.direction = Direction.OUTPUT
interrupt_pin = DigitalInOut(board.D12)
interrupt_pin.pull = Pull.UP

led.value = True
# Give time to delete this build if we get stuck...once in deep sleep and can't get out, we're stuck.
time.sleep(20)
led.value = False
mymodule.deep_sleep()
led.value = True
time.sleep(4)
led.value = False
