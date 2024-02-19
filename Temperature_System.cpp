#include "temperature_system.h"
#include "Arduino.h"
#include "main.h"

void init_temperature_system(void)
{
  // To configure Heater and cooler pin output
  pinMode(HEATER, OUTPUT);
  pinMode(COOLER, OUTPUT);

  // To turn OFF the heater and cooler
  digitalWrite(HEATER, LOW);
  digitalWrite(COOLER, LOW);
}

float read_temperature(void)
{
  float temperature;
  // Reading the analog valule and contain to voltage and then to temperature in degree gree celcius
  temperature = (((analogRead(TEMPERATURE_SENSOR) * (float)5 / 1024)) / (float)0.01);
  return temperature;
}

// Function to control the cooler
void cooler_control(bool control)
{
  if (control)
  {
    digitalWrite(COOLER, HIGH);
  }
  else
  {
    digitalWrite(COOLER, LOW);
  }
}

void heater_control(bool control)
{
  if (control)
  {
    digitalWrite(HEATER, HIGH);
  }
  else
  {
    digitalWrite(HEATER, LOW);
  }
}
