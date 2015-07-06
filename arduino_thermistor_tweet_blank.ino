/*
	This code will take input from four thermistors connected to an arduino Uno,
	use a wifi connection to connect to a router, and send a tweet to a predetermined 
	twitter account with the readings from the thermistors.  I have not written all of this
	code from scratch, but borrowed examples for the various components of the functionality
	and filled in the gaps and woven everything together where appropriate.
	Comments and attributes from the original code have been left in place when available.
	
	This code was used to monitor temperature in various points in a back yard pig roasting pit
	with some probes inserted in the meat, some reading temperatures in various points in the 
	pit.  During initial testing, console and LCD outputs were used, but the output source 
	during real usage for the pig roast was wifi tweets.
	

        I used replacement temperature probes from Amazon for less than $10 :  http://www.amazon.com/gp/product/B003LRSH4A?psc=1&redirect=true&ref_=od_aui_detailpages00
        with a 200k resistor - use a good thermometer, the LCD output and cups of hot and cold water to check to 
        make sure your resistance is correct.




/*  
    Arduino thermistor example software
    Tutorial:  http://www.hacktronics.com/Tutorials/arduino-thermistor-tutorial
    Copyright (c) 2010 Mark McComb, hacktronics LLC
    License: http://www.opensource.org/licenses/mit-license.php (Go crazy)
*/

#include <LiquidCrystal.h>
#include <math.h>
#include <Twitter_WiFi.h>
#include <SPI.h>
#include <WiFi.h>

/*Network and Twitter Information*/
char ssid[] = "########"; /*put your ssid here */
char pass[] = "########"; /*put your wifi password here */

// Your Token to Tweet (get it from http://arduino-tweet.appspot.com/)

Twitter twitter("###################################"); //put your twitter token here


/*
See tutorial for Thermistor connections

LCD Connections:
rs (LCD pin 4) to Arduino pin 12
rw (LCD pin 5) to Arduino pin 11
enable (LCD pin 6) to Arduino pin 10
LCD pin 15 to Arduino pin 13
LCD pins d4, d5, d6, d7 to Arduino pins 5, 4, 3, 2
*/

LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);
int backLight = 13;    // pin 13 will control the backlight

 String infoString = "" ;

void setup(void) {
  pinMode(backLight, OUTPUT);
  digitalWrite(backLight, HIGH); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
  lcd.begin(20, 4);              // rows, columns.  use 16,2 for a 16x2 LCD, etc.
  lcd.clear();                   // start with a blank screen
  lcd.setCursor(0,0);            // set cursor to column 0, row 0

  delay(1000);
  WiFi.begin(ssid, pass);
  delay(10000);
  Serial.begin(9600);
  
  
}

double Thermister(int RawADC) {
  double Temp;
  // See http://en.wikipedia.org/wiki/Thermistor for explanation of formula
  Temp = log(((10240000/RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;           // Convert Kelvin to Celcius
  return Temp;
}

void printTemp(void) {
  int fTemp;
  double temp = Thermister(analogRead(0));  // Read sensor
  
  int fTemp1;
  double temp1 = Thermister(analogRead(1));
  
  int fTemp2;
  double temp2 = Thermister(analogRead(2));
  
  int fTemp3;
  double temp3 = Thermister(analogRead(4));
  
  //only need the lcd for testing....once it's on the pig and spinning, we will read temps from the twitter feed
  //lcd.clear();
  //lcd.setCursor(0,0);
  //lcd.print("Temperature is:");
  //lcd.setCursor(0,1);
  //lcd.print(temp);
  //lcd.print(" C / ");
  fTemp = (temp * 1.8) + 32.0;    // Convert to USA
  fTemp = fTemp - 5; //seems to be about 5 degrees warmer inside the project box, so we're going to compensate
  fTemp1 = (temp1 * 1.8) + 32.0;
  fTemp2 = (temp2 * 1.8) + 32.0;
  fTemp3 = (temp3 * 1.8) + 32.0;
  
  //lcd.print(fTemp);
  //lcd.print(" F");
  Serial.print("Ambient temp: ");
  Serial.println(fTemp);
  Serial.print("Probe 1 temp: ");
  Serial.println(fTemp1);
  Serial.print("Probe 2 temp: ");
  Serial.println(fTemp2);
  Serial.print("Probe 3 temp: ");
  Serial.println(fTemp3);
  
  if (fTemp > 68 && fTemp < 78) {
    //lcd.setCursor(0,3);
   // lcd.print("Very comfortable");
  }
  
  infoString = "Approx Ambient Temp: " ;
  infoString.concat(fTemp);
  infoString.concat(" Ham Temp: ");
  infoString.concat(fTemp1);
  infoString.concat(" Loin Temp: ");
  infoString.concat(fTemp2);
  infoString.concat(" Firebox Temp: ");
  infoString.concat(fTemp3);
  char msg[200];
  infoString.toCharArray(msg,200);
  
    Serial.println("connecting ...");
  if (twitter.post(msg)) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
  
 
}

void loop(void) {
  printTemp();
  delay(900000);  //how often do you want the temperature reading to happen??
}


