#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/*
  Haptic stimulation
  Licence MIT
  (C) Piotr Falkowski 2020
*/

const int MinMessageLength = 6;
const int MaxMessageLength = 64;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
String inputString;
bool stringComplete = false;

class HapticCommand
{
  private:
    unsigned int PitchInHz;
    int FingerStimulationStrength[10];
    
    void set_fully_on(uint8_t pin)
    {
      pwm.setPWM(pin, 4096, 0);
    }

    void set_fully_off(uint8_t pin)
    {
      pwm.setPWM(pin, 0, 4096);
    }
  public:

    void ParseCommand(char* string)
    {
    }
    
    void Play()
    {
      for(int i=0; i<sizeof(FingerStimulationStrength); ++i)
      {
        if (FingerStimulationStrength[i] >= 4096)
        {
          set_fully_on(i);
        }
        else if (FingerStimulationStrength[i] <= 0)
        {
          set_fully_off(i);
        }
        else
        {
          pwm.setPWM(i, FingerStimulationStrength[i], 0);
        }
      }
    }

    void Setup(unsigned int PitchInHz, unsigned long oscillatorFrequency = 27000000, unsigned long I2CwireClock = 400000)
    {
      pwm.begin();

      // In theory the internal oscillator is 25MHz but it really isn't
      // that precise. You can 'calibrate' by tweaking this number till
      // you get the frequency you're expecting!
      pwm.setOscillatorFrequency(oscillatorFrequency);  // The int.osc. is closer to 27MHz

      pwm.setPWMFreq(PitchInHz);

      // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
      // some i2c devices dont like this so much so if you're sharing the bus, watch
      // out for this!
      Wire.setClock(I2CwireClock);

    }
};

HapticCommand command;
void setup()
{
  // initialize serial:
  Serial.begin(115200);
  // reserve bytes for the inputString:
  inputString.reserve(256);
  command.Setup(140);
}

void loop()
{
  if (stringComplete) 
  {
    Serial.println("Received command: " + inputString + "Executing...");
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}


/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() 
{
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
