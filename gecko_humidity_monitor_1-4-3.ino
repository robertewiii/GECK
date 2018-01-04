//Robertewiii 1/4/17, this is an update to the gecko humidity sensor that uses functions and includes more features. Version 1.4.3
//added support for relay thermostat since 1.3.2, will cycle a relay off in the event temperature reaches above 80F, normally open relay, break circuit for active heating source
//fixed issue with humidity comparison operations, issue caused status message to display the humidity to always be too high or too low and never indicated safe humidity
//fixed issue with LCD display failing to clear characters leading to spelling errors with display messages
#include <Wire.h>
#include <rgb_lcd.h>
#include "DHT.h"

rgb_lcd lcd;
//these are the initial color values for the RGB display
const int colorR = 0;
const int colorG = 0;
const int colorB = 0;

#define DHTPIN A1 //the pin to connect the DHT sensor to
#define DHTTYPE DHT22 //DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

//pin definintions
const int CEL_PIN = 3; //defines the pin that Celsius switch mode button is on
const int ALERT_PIN = 2; //defines the pin that the alert switch is on
const int BUZZER_PIN = 4; //defines the pin the buzzer is on
const int BACK_LIGHT_PIN = 5; //defines the pin that the switch that cycles the back light on the display will be placed on
const int RELAY_PIN1 = 7; //pin that will hold the relay to switch the heater on and off


//temperature mitigation 
const double MAX_TEMP = 26.7; //26.66C = 80F, this is the temperature that will cycle the relay open if the temperature reads this value, relay should be safely in place to interupt the heating circuit
//change these settings to configure incubator to be species specific
const double DANGER = 45.00; //the dangerously low humidity level that alarm triggers at, %Rh, any time the sensor reads humidity and it is less than this value it triggers the buzzer alarm if enabled, set to 68.00 for incubating crested gecko eggs
//upper and lower humidity ranges, these trigger status messages only and will not trip an alarm for values outside this range
const double UPPER = 80.00; //upper limit of safe humidity range, set to 84.00 for incubating crested gecko eggs, set at 80% for adult geckos and keep tank above this for a few hours every day
const double LOWER = 50.00; //lower limit of safe humidity range, set to 78.00 for incubating crested gecko eggs, set at 50% for adults and do not go below this value

//temperature ranges, these values trigger status messages 
const double HOT = 25.6; //above this is too hot, set at 27.6 (82F) for crested gecko eggs, 28C is the absolute max temperature that either a crested gecko or their eggs can endure, set to 25.6C (78F) for ideal max temp for adults, 28C for eggs
const double COLD = 21.1; //below this is too cold, adult crested geckos can tolerate night time temperature drops as low as 15.5C (60F), try to keep temperatures above 21C (70F)
//temperature driven sex biasing
//certain reptiles determine sex based on average temperature they were incubated at, ie temperature can strongly bias male/female hatch rates 
//crested geckos are believed to be influenced by this, the default settings change the LCD color and display a status message to aid in sex biasing for crested geckos

const bool SEX_BIAS = false; //set this to false in order to disable the sex biasing feature
const double MALE_MIN = 25.5; //this is a minimum for male biasing
const double MALE_MAX = 27; //this is the maximum for male biasing
const double FEMALE_MIN = 21.1; //this is the minimum temp for female biasing 
const double FEMALE_MAX = 23.8; //this is a max for female biasing



void setup() {
  // put your setup code here, to run once:
pinMode(CEL_PIN, INPUT); //make the celsius switch pin an input
pinMode(ALERT_PIN, INPUT); //make the alert pin switch an input
pinMode(BUZZER_PIN, OUTPUT); //make the pin the buzzer sits on an output
pinMode(BACK_LIGHT_PIN, INPUT);
pinMode(RELAY_PIN1, OUTPUT);

lcd.begin(16, 2); // set up the LCD's number of columns and rows:
lcd.setRGB(colorR, colorG, colorB); //sets color of display initially
Serial.begin(9600);    
dht.begin();
delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:

//variables
float humidity = dht.readHumidity();
float temperature = dht.readTemperature(); //humidity as %RH, temperature in C
bool celsiusDisplay; //initialize reading to be displayed in Celsius
bool alertBool; //the alarm bool, if true then alarm is on, off if false,
bool backLightBool; //turning this on or off will disable the backlight, on for on

// check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(humidity) || isnan(temperature)) 
    {
        Serial.println("Failed to read from DHT");
    } 
    else 
    {
        Serial.print("Humidity: "); 
        Serial.print(humidity);
        Serial.print(" %\t");
        Serial.print("Temperature: "); 
        Serial.print(temperature);
        Serial.println(" *C");
    }
    
  //main functions that drive the program called here  
  readSensors(temperature, humidity, celsiusDisplay, alertBool,  backLightBool);
  sensorDisplay (humidity, temperature, celsiusDisplay, alertBool);
  tempStatus (temperature, backLightBool);
  humidStatus(humidity);
  alarm(humidity, alertBool);
  
}

void humidStatus (double humidity_par)
{
  //checks the humidty and displays status messages to tell if it is acceptable or not
 
  
  lcd.setCursor(0, 0); //sets cursor to column 0 row 0

  if (humidity_par < LOWER) 
	lcd.print("hLow  ");
  
  else if (humidity_par > UPPER)
    lcd.print("hHigh ");

  else
    lcd.print("hGood ");  
  
}

void tempStatus (double temp_par, bool backLight_par)
{
  //changes display color based on temp and gives status info
  int red = 0;
  int blue = 0;
  int green = 0;
  //keep these constants == 0 in order to maintain the ability to cycle the backlight off
  const int RED0 = 0;
  const int GREEN0 = 0;
  const int BLUE0 = 0;
  lcd.setCursor(6, 0); 

  if (backLight_par == LOW)
    lcd.setRGB(RED0, GREEN0, BLUE0); //check if the switch to disable the backlight is set to low, if it is, set the LCD color to (0,0,0) to turn the backlight off
  //the backlight only changes during an if/else operation if the switch is set to HIGH, otherwise only a status message gets displayed

  if (SEX_BIAS == false)
  {
	  //these settings are for when sex biasing is disabled and only status messages relating to the incubator being too hot, cold, or just right in temperature are desired
	  if (temp_par >= HOT)
	  {
		  //for temps that are too hot
		  red = 255;
		  blue = 0;
		  green = 0;
		  //red for hot
		  lcd.print("tHot  "); //tHot for temp hot
		  if (backLight_par == HIGH)
			  lcd.setRGB(red, green, blue);
	}
	  else if (temp_par <= COLD)
	  {
		  //for temps that are too cold
		  red = 0;
		  blue = 255;
		  green = 0;
		  //blue for too cold
		  lcd.print("tCold "); //tCold for temp cold
		  if (backLight_par == HIGH)
			  lcd.setRGB(red, green, blue);
	  }
	  else
	  {
		  //temp is just right
		  red = 0;
		  blue = 0;
		  green = 255;
		  //green for good
		  lcd.print("tGood "); //tGood for temp good
		  if (backLight_par == HIGH)
			  lcd.setRGB(red, green, blue);
	  }

  }
  else
  {
	  //start of the sequence for sex biasing being true
	  if (temp_par >= HOT)
	  {
		  //warn the user that the incubator is too hot
		  red = 255;
		  blue = 0;
		  green = 0;
		  //red for hot
		  lcd.print("tHot  ");
		  if (backLight_par == HIGH)
			  lcd.setRGB(red, green, blue);
	  }
	  else if (temp_par <= COLD)
	  {
		  //warn the user that the incubator is too cold
		  red = 0;
		  blue = 255;
		  green = 0;
		  //blue for too cold
		  lcd.print("tCold ");
		  if (backLight_par == HIGH)
			  lcd.setRGB(red, green, blue);
	  }
	  else if (temp_par >= MALE_MIN && temp_par < MALE_MAX)
	  {
		  //biasing for male
		  red = 0;
		  blue = 128;
		  green = 128;
		  //cyan for boy
		  lcd.print("tMale ");
		  if (backLight_par == HIGH)
			  lcd.setRGB(red, green, blue);
	  }
	  else if (temp_par > FEMALE_MIN && temp_par <= FEMALE_MAX)
	  {
		  //biasing for female
		  red = 255;
		  blue = 255;
		  green = 0;
		  //magenta for female
		  lcd.print("tFem  ");
		  if (backLight_par == HIGH)
			  lcd.setRGB(red, green, blue);

	  }
	  else
	  {
		  //temp is outside of the sex biasing ranges, but inside the max and min temp ranges
		  red = 0;
		  blue = 0;
		  green = 255;
		  //green for good
		  lcd.print("tGood "); //TG for temp good
		  if (backLight_par == HIGH)
			  lcd.setRGB(red, green, blue);
	  }
  }
}

void alarm (double humidity_par, bool alertBool_par)
{
  //will sound an alarm if the humidity is dangerously low and the alarm is enabled
  const int LOW_TONE = 10000; //low note in alarm
  const int HIGH_TONE = 15000; //high note in alarm


  if ((humidity_par <= DANGER) && (alertBool_par == true))
  {
    tone(BUZZER_PIN, LOW_TONE, 100); //plays the low note on buzzer pin for 100 ms
    delay(100); //wait 100 ms
    tone(BUZZER_PIN, HIGH_TONE, 100); //plays the high note on buzzer pin for 100 ms
  }
}

void readSensors (float & temp, float & humidity, bool & celciusDisplay, bool & alertBool, bool & backLightBool_Par)
{

  temp = dht.readTemperature(); //reads the temperature and returns it back as pass by reference
  humidity = dht.readHumidity(); //reads the humidity and returns it back as pass by reference
  alertBool = digitalRead(ALERT_PIN);
  celciusDisplay = digitalRead(CEL_PIN); 
  backLightBool_Par = digitalRead(BACK_LIGHT_PIN);
}

void sensorDisplay (double humidity, double temperature, bool celsiusDisplay, bool alertBool)
{
  double tempF; //the temperature in F
  tempF = (temperature * 1.8)+32;

  if (temperature <= MAX_TEMP)
    digitalWrite(RELAY_PIN1, HIGH); //turn the heater on
  else
    digitalWrite(RELAY_PIN1, LOW); //turn the heater off
  
  if (alertBool == true)
  {
    lcd.setCursor (12, 0); // column 15, row 0
    lcd.print("AOn ");
  }
  else
  {
    lcd.setCursor (12, 0); // column 15, row 0
    lcd.print("AOff");
  }
  
  lcd.setCursor(0, 1); //column 0, row 1

  if (celsiusDisplay == true)
  {
    lcd.setCursor(0, 1); //column 0, row 1
    lcd.print(temperature); //temp in C
    lcd.setCursor(5, 1); //column 5 row 1
    lcd.print("C");
    lcd.setCursor(8, 1); //column 8 row 1
    lcd.print(humidity); //humidity in %RH
    lcd.setCursor(13, 1); //column 13, row 1
    lcd.print("%Rh"); //marks the units for humidity
    delay(100); //delay 100 ms
  }
  else 
  {
    lcd.setCursor(0, 1); //column 0, row 1
    lcd.print(tempF); //temp in F
    lcd.setCursor(5, 1); //column 5 row 1
    lcd.print("F");
    lcd.setCursor(8, 1); //column 8 row 1
    lcd.print(humidity); //humidity in %RH
    lcd.setCursor(13, 1); //column 13, row 1
    lcd.print("%Rh"); //marks the units for humidity
    delay(100); //delay 100 ms
  }

}




