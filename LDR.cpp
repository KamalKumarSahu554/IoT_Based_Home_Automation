#include "ldr.h"
#include "Arduino.h"
#include "main.h"

void init_ldr(void)
{
  pinMode(GARDEN_LIGHT, OUTPUT);
}

void brightness_control(void)
{
  unsigned int ldr_val;
  // To read vale from ldr
  ldr_val = analogRead(LDR_SENSOR);

  // Mapping 0 - 1023 to 255 - 0
  // Scaling down 0 - 1023 to 255 - 0
  ldr_val = (1023 - ldr_val) / 4;

  // To assign duty cycle to led based in LDR value
  analogWrite(GARDEN_LIGHT, ldr_val);
  delay(100);
}
