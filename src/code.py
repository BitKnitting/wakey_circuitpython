import board
from digitalio import DigitalInOut, Direction, Pull
import time
import mymodule
print("start - will wait for 30 seconds.")
led = DigitalInOut(board.D13)
led.direction = Direction.OUTPUT
wakey_pin = DigitalInOut(board.D12)
wakey_pin.direction = Direction.INPUT
wakey_pin.pull = Pull.DOWN
# Just a start....
led.value = True
# Give time to delete this build if we get stuck...once in deep sleep and can't get out, we're stuck.
time.sleep(30)
while True:
    if wakey_pin.value:
        led.value = True
    else:
        led.value = False
        # Here I set the SCR register to deep sleep and call __WFI().  m0 goes to sleep
        # I expect if I put 3V on the wakey_pin, it will wake up - since an interrupt
        # has occured?  But I guess an interrupt hasn't occured.  So what if I try __WFE?
        mymodule.deep_sleep()
    time.sleep(.01)    