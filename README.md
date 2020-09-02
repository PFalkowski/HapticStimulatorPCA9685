# HapticStimulatorPCA9685
Arduino code for 10-channel haptic stimularor over PCA9685

To interface serial port from Python, install pySerial
```
pip install PySerial
```
than run:

```
import serial

ser = serial.Serial("COM3") #where COM3 = your COM port
ser.baudrate = 115200

if not ser.is_open:
	ser.open()

#try messing with opening it half way
ser.write(b'2000,2000,2000,2000,2000,2000,2000,2000,2000,2000\r\n')

#fully
ser.write(b'4096,4096,4096,4096,4096,4096,4096,4096,4096,4096\r\n')

#turning off
ser.write(b'0,0,0,0,0,0,0,0,0,0\r\n')

#setting output frequency (40-1600Hz allowed, see https://learn.adafruit.com/16-channel-pwm-servo-driver/library-reference#setpwmfreq-freq-335626-1)
ser.write(b'SetFreq:1600\r\n') #set output frequency to 1600Hz
```


guide on interfacing serial port from Python https://pyserial.readthedocs.io/en/latest/shortintro.html
