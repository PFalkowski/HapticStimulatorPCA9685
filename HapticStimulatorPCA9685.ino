#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/*
  Haptic stimulation
  Licence MIT
  (C) Piotr Falkowski 2020
*/
const bool diagnosticInfo = false;
const int channelsNo = 10;

int FingerToPinMap[channelsNo] = {0, 1, 2, 3, 4, 8, 9, 10, 11, 12};

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
String inputString;
bool stringComplete = false;

class HapticCommand
{
  private:
    unsigned int PitchInHz;
    int FingerStimulationStrength[channelsNo];
    
    void set_fully_on(uint8_t pin)
    {
      pwm.setPWM(pin, 4096, 0);
    }

    void set_fully_off(uint8_t pin)
    {
      pwm.setPWM(pin, 0, 4096);
    }
    
    void ChangeStrength()
    {
      for(int i=0; i < 10; ++i)
      {
        Serial.print("Setting motor ");
        Serial.print(i);
        Serial.print(" to ");
        Serial.println(FingerStimulationStrength[i]);
        if (FingerStimulationStrength[i] >= 4096)
        {
          set_fully_on(FingerToPinMap[i]);
        }
        else if (FingerStimulationStrength[i] <= 0)
        {
          set_fully_off(FingerToPinMap[i]);
        }
        else
        {
          pwm.setPWM(FingerToPinMap[i], 4096 - FingerStimulationStrength[i], 0);
        }
      }
    }

    void SetFrequency(unsigned int newFrequency)
    {
      pwm.setPWMFreq(newFrequency);
    }
  public:

    void Execute(String command)
    {
      if (command.startsWith("SetFreq:"))
      {
        String rawValue = command.substring(8);
        rawValue.trim();
        int parsed = rawValue.toInt();
        int fitted = min(max(40, parsed), 1600);
        Serial.print("Setting oscilation frequency to: ");
        Serial.println(fitted);
        SetFrequency(fitted);
      }
      else
      {          
        int commaIndex = 0;
        bool hasNextComma = true;
        
        int i = 0;
        while(hasNextComma)
        {
          if (diagnosticInfo)
          {
            Serial.print("commaIndex: ");
            Serial.println(commaIndex);
          }
          int nextCommaIndex = command.indexOf(',', commaIndex + 1);
          hasNextComma = commaIndex >= 0;
          String parsed = command.substring(commaIndex, nextCommaIndex);
          if (parsed[0] == ',')
          {
             parsed = parsed.substring(1, -1);
          }
          
          if (diagnosticInfo)
          {
            Serial.print("parsed: ");
            Serial.println(parsed);
            Serial.print("nextCommaIndex: ");
            Serial.println(nextCommaIndex);
            Serial.print("hasNextComma: ");
            Serial.println(hasNextComma);
          }
          FingerStimulationStrength[i++] = parsed.toInt();
          commaIndex = nextCommaIndex;
        }
        if (diagnosticInfo)
        {
          Serial.print("hasNextComma: ");
          Serial.println(hasNextComma);
        }
        ChangeStrength();
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
    command.Execute(inputString);
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
