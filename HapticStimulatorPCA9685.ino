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

class HapticCommand
{
  private:
    unsigned int PitchInHz;
    
    void set_fully_on(uint8_t pin)
    {
      pwm.setPWM(pin, 4096, 0);
    }

    void set_fully_off(uint8_t pin)
    {
      pwm.setPWM(pin, 0, 4096);
    }

    void setToPercent(uint8_t pin, uint8_t percentage)
    {
      int strength = map(percentage, 0, 100, -4096, 0);
      if (strength < -4095)
      {
        set_fully_off(pin);
      }
      else if (strength > -1)
      {
        set_fully_on(pin);
        
      }
    }

    void Parse(char* string)
    {
      if (strlen(string) >= MinMessageLength)
      {
        char *field;
        field = strtok(string, "," );
        PitchInHz = atoi(field);
        //Serial.print("Parsed PitchInHz: ");
        //Serial.println(PitchInHz, DEC);
        //Serial.flush();

        field = strtok(nullptr, "," );
        DurationInMs = atoi(field);
        //Serial.print("Parsed DurationInMs: ");
        //Serial.println(DurationInMs, DEC);
        //Serial.flush();

        field = strtok(nullptr, "," );
        Finger = atoi(field);
        //Serial.print("Parsed Finger: ");
        //Serial.println(Finger, DEC);
        //Serial.flush();
      }
    }

  public:
    int DurationInMs = 1000;
    int Finger = 0;

    void ReadFromSerial()
    {
      char res[MaxMessageLength];

      int availableBytes = Serial.available();
      for (int i = 0; i < availableBytes; ++i)
      {
        res[i] = Serial.read();
      }
      //Serial.flush();
      Parse(res);
    }

    void Play()
    {
      pwm.setPWM(Finger, 4096, 0);
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


void setup()
{
  Serial.begin(115200);
}

uint8_t pin = 0;

void loop()
{
  HapticCommand command;

  Serial.flush();
  if (Serial && Serial.available())
  {
    command.ReadFromSerial();
    delay(32);
    command.Play();
  }
  //Serial.print("Waiting for data, serial state is = ");
  //Serial.println(Serial.available());
  delay(32);
}
