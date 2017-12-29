# GECK
Everything a gecko breeder needs for their temperature/humidity monitor

libraries used:
https://github.com/Seeed-Studio/Grove_Temperature_And_Humidity_Sensor 
https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight

This project uses the Seeed grove system and an Arduino Uno to accurately monitor temperature and humidity in an incubator. OOB it is configured specifically for monitoring adult crested geckos as simple thermostat, hygrometer and thermometer. It can be configured as an incubator monitor using values included in the comments on the code. It uses a DHT sensor made by Seeed to measure the temperature and humidity with far greater accuracy than any common off the shelf reptile specific hygrometer or thermometer. It displays these values on an RGB backlit LCD display with status messages to warn if the values are unacceptable. It is user configurable to change temperature units to display in either Fahrenheit or Celsius on the fly by flipping the switch on digital pin 2. It also includes an audible alarm in the form of a piezo buzzer to sound out when the humidity drops to dangerously low levels in order to warn the user. The alarm can be disabled with the switch located on digital pin 3. If the alarm is enabled then the display will mark an “AOn” character in the upper right hand corner. The design is completely modular and easily repairable. The DHT sensor is easily replaceable which is essential for any precision hygrometer given that these instruments are not user calibratable. Additional features include temperature monitoring with a color changing display to quickly gage readings at a glance. Included is an optional “sex biasing mode” which will display additional information for temperature biasing. Certain reptiles determine sex during incubation from the average temperature of their nest. The specific temperatures needed to achieve this can be set by the user in the sketch and the monitor will change the LCD display color and display status codes for the purpose of sex biasing. By default this feature is turned off and is configured for crested geckos.
Parts list:
As previously stated all parts are based around the grove connector ecosystem for simplicity and ease of use.
1x Arduino Uno or equivalent
1x base shield V2 http://wiki.seeed.cc/Base_Shield_V2/
1x Grove - Temperature&Humidity Sensor Pro (AM2302）https://www.seeedstudio.com/Grove-Temperature%26Humidity-Sensor-Pro%EF%BC%88AM2302%EF%BC%89-p-838.html
1x grove – buzzer https://www.seeedstudio.com/Grove-Buzzer-p-768.html
3x grove – switch(P) https://www.seeedstudio.com/Grove-Switch%28P%29-p-1252.html
1x Grove – LCD RGB Backlight https://www.seeedstudio.com/Grove-LCD-RGB-Backlight-p-1643.html
Optional: 1x grove relay switch https://www.seeedstudio.com/Grove-Relay-p-769.html 

Pinout list:
Plug the listed device into the socket named on the base shield. A# pins are mapped to Uno analog pins while D# pins map directly to digital pins on the Uno.
12C- display
A1 – DHT sensor
D2 – Alarm enable/disable switch
D3- Temperature display mode switch (°F/°C)
D4- Buzzer
D5 – back light toggle switch
D7 – relay to switch enclosure heater on and off
In order to configure this program for a species other than crested geckos change the following global constants:
DANGER – The minimum trigger level for the humidity alarm
UPPER – The upper Rh% target
LOWER- the lower Rh% target
HOT – The maximum safe temperature
COLD – The minimum safe temperature
Instructions for sex biasing:
As previously stated in the introduction some reptiles determine sex during incubation time based on the average temperature of the nest. In order to aid breeders in biasing the male/female ratio of their egg clutches this monitor includes an optional “sex biasing mode”. This feature is off by default and is toggled in the sketch by setting the global constant bool “SEX_BIAS”. These values are located directly prefacing the setup function. By default, this feature is turned off and configured for crested geckos. Make sure that your temperature ranges for a chosen species do not overlap. The following variables and constants need to be changed in order to bias other species or disable this feature:
Const bool SEX_BIAS, true for on, false for off
Const double MALE_MIN, the minimum temperature for the male sexing range
Const double MALE_MAX
Const double FEMALE_MIN
Const double FEMALE_MAX


Display messages – The bottom row of the display shows the temperature then the humidity as % relative humidity.
The upper left hand corner will display if the humidity is low, high, or safe/acceptable
The upper right hand corner will display if the humidity alarm is enabled or disabled
The top row center will show two letters that indicate the temperature status denoted by the form T*, see the following to decode
TC – temperature is cold, LCD backlight if enabled will be dark blue
TF – “temperature female”, more female geckos will hatch than male, LCD will be magenta
TG – “temperature good”, approximately equal numbers of male and female geckos will hatch, LCD will be green
TM – “temperature male” more male geckos will hatch than female, lcd will be cyan
TH – “temperature hot” temperature readings are reading too high, LCD will be a deep red
