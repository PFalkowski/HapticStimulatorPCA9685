# HapticStimulatorPCA9685
Arduino code for 10-channel haptic stimularor over PCA9685

To interface serial port from Python, install pySerial
```
pip install PySerial
```
than run:

```
import serial
import io

ser = serial.Serial("COM3") #where COM3 = your COM port
sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))
ser.baudrate = 115200

if not ser.is_open:
	ser.open()

#try messing with opening it half way
ser.write(b'2000,2000,2000,2000,2000,2000,2000,2000,2000,2000\r\n')

#fully
ser.write(b'4096,4096,4096,4096,4096,4096,4096,4096,4096,4096\r\n')

#turning off
ser.write(b'0,0,0,0,0,0,0,0,0,0\r\n')

```


guide on interfacing serial port from Python https://pyserial.readthedocs.io/en/latest/shortintro.html
