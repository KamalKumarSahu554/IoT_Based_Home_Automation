/*************************************************************
Title         :   Home automation using blynk
Description   :   To control light's brigntness with brightness,monitor temperature , monitor water level in the tank through blynk app
Pheripherals  :   Arduino UNO , Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
*************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL3AYJ4azbw"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "9TbmculbD9en9j39wT4c5qYm80spEiyV"

// Comment this out to disable prints
// #define BLYNK_PRINT Serial

// Include Library for BLYNK
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
// Include Library for CLCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

char auth[] = BLYNK_AUTH_TOKEN;
bool heater_sw, cooler_sw, inlet_sw, outlet_sw;
unsigned int tank_volume;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// This function is called every time the Virtual Pin 0 state changes
/*To turn ON and OFF cooler based virtual PIN value and print the notification on CLCD */
BLYNK_WRITE(COOLER_V_PIN)
{
  // To read the value on virtual pin connected to cooler
  cooler_sw = param.asInt();
  if (cooler_sw)
  {
    cooler_control(ON);
    lcd.setCursor(7, 0);
    lcd.print("CO_LR ON ");
  }
  else
  {
    cooler_control(OFF);
    lcd.setCursor(7, 0);
    lcd.print("CO_LR OFF");
  }
}

/*To turn ON and OFF heater based virtual PIN value and print the notification on CLCD*/
BLYNK_WRITE(HEATER_V_PIN)
{
  // To read the value on virtual pin connected to heater
  heater_sw = param.asInt();
  if (heater_sw)
  {
    heater_control(ON);
    lcd.setCursor(7, 0);
    lcd.print("HT_R ON  ");
  }
  else
  {
    heater_control(OFF);
    lcd.setCursor(7, 0);
    lcd.print("HT_R OFF ");
  }
}

/*To turn ON and OFF inlet vale based virtual PIN value*/
BLYNK_WRITE(INLET_V_PIN)
{
  // To read the valuse from the inlet pin
  inlet_sw = param.asInt();
  if (inlet_sw)
  {
    enable_inlet();
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_ON ");
  }
  else
  {
    disable_inlet();
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_OFF");
  }
}

/*To turn ON and OFF outlet value based virtual switch value*/
BLYNK_WRITE(OUTLET_V_PIN)
{
  // To read the valuse from the outlet pin
  outlet_sw = param.asInt();
  if (outlet_sw)
  {
    enable_outlet();
    lcd.setCursor(7, 1);
    lcd.print("OT_FL_ON ");
  }
  else
  {
    disable_outlet();
    lcd.setCursor(7, 1);
    lcd.print("OT_FL_OFF");
  }
}
/* To display temperature and water volume as gauge on the Blynk App*/
void update_temperature_reading()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(TEMPERATURE_GAUGE, read_temperature());
  Blynk.virtualWrite(WATER_VOL_GAUGE, volume());
}

/*To turn off the heater if the temperature raises above 35 deg C*/
void handle_temp(void)
{
  // Read temperature and compare with 35 and also check if heater is ON
  if ((read_temperature() > float(35)) && heater_sw)
  {
    heater_sw = 0;
    // Turn OFF the heater
    heater_control(OFF);
    // To print heater status on the dashboard
    lcd.setCursor(7, 0);
    lcd.print("HT_R OFF");

    // To print notification on the blynk app
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "\nTemperature is avobe 35 degree celsius\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning OFF the heater\n");

    // To reflect on the status on the button widgeton heater pin
    Blynk.virtualWrite(HEATER_V_PIN, 0);
  }
}

/*To control water volume above 2000ltrs*/
void handle_tank(void)
{
  // To check volume of the water is less then 2000 then enable the inlet valve
  if ((tank_volume < 2000) && (inlet_sw == 0))
  {
    enable_inlet();
    inlet_sw = 1;
    // To print status on dash board
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_ON ");

    // To print the notification on the blynk mobile app
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "\nVolume of water in the tank is less than 2000\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning ON the inlet valve\n");

    // reflecting on the button widget
    Blynk.virtualWrite(INLET_V_PIN, 1);
  }
}

void setup(void)
{
  // To Config Garden light as output
  init_ldr();
  // Initialize the LCD
  lcd.init();
  // Turn on the back light
  lcd.backlight();
  // Clear the CLCD
  lcd.clear();
  // To set cursor to the first position
  lcd.home();
  // To display the string
  lcd.setCursor(0, 0);
  lcd.print("T=");
  // To set curser at second line first position
  lcd.setCursor(0, 1);
  lcd.print("V=");

  // To connect arduino to the blynk cloud
  Blynk.begin(auth);

  // To Initialise temperature system
  init_temperature_system();

  // To initialize the serial tank
  init_serial_tank();

  // To update temperature to blynk app for every 0.5 second
  timer.setInterval(500L, update_temperature_reading);
}

void loop(void)
{
  Blynk.run();

  // Keep timer runing
  timer.run();

  // To control brightness of the LED
  brightness_control();

  // To read the temperature and display it on CLCD
  String temperature;
  temperature = String(read_temperature(), 2);
  lcd.setCursor(2, 0);
  lcd.print(temperature);

  // To read the volume of water and dispaly it in CLCD
  tank_volume = volume();
  lcd.setCursor(2, 1);
  lcd.print(tank_volume);

  // To maintain volume of water for 2000
  handle_tank();

  // To maitanin threshold temperature
  handle_temp();
}
