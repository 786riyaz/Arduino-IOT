from machine import Pin, I2C, Timer
from ssd1306 import SSD1306_I2C
import utime

# Initialize LED
led = Pin(25, Pin.OUT)
LED_State = True

# Initialize I2C for OLED
i2c = I2C(0, sda=Pin(8), scl=Pin(9), freq=400000)

# Initialize OLED (128x64 for SSD1306; adjust to 64x128 for SH1107 if needed)
oled = SSD1306_I2C(128, 64, i2c, addr=0x3C)  # Change addr to 0x3D if needed

# Initialize Timer
tim = Timer()

def tick(timer):
    global led, LED_State
    # Toggle LED state
    LED_State = not LED_State
    led.value(LED_State)
    
    # Get current time using utime
    current_time = utime.localtime()
    hours = current_time[3]
    minutes = current_time[4]
    seconds = current_time[5]
    
    # Format time as HH:MM:SS
    time_string = "{:02d}:{:02d}:{:02d}".format(hours, minutes, seconds)
    
    # Clear OLED display
    oled.fill(0)
    
    # Display time centered (approximate for 128x64 display)
    oled.text("Current Time", 10, 10)
    oled.text(time_string, 30, 30)
    
    # Update display
    oled.show()

# Start timer to update every 1 second (freq=1)
tim.init(freq=1, mode=Timer.PERIODIC, callback=tick)
