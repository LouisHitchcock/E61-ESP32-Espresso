#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <XMLWriter.h>
#include <FastLED.h>
#include <ESP32Servo.h>
#include <PID_v2.h>
#include "index.h"

bool isEstopTriggered = false;  // ESTOP Active

WebServer server(80); // Server on port 80

const char* ssid = "";
const char* password = "";

const int numReadings = 10;
const int numSensors = 3;
const int sensorPins[numSensors] = { 6, 9, 10 };  // LinearPot, BrewBoilerTemp, SteamBoilerTemp
int sensorReadings[numSensors][numReadings];
int sensorValues[numSensors] = { 0 };
int sensorIndex = 0;
int sensorValue1 = 0;
float BrewBoilerCalc = 0.0;  //Brew Boiler Temperautre
float SteamBoilerCalc = 0.0;  // Steam Boiler Temperature

const int outputPins[] = { 4, 5, 15, 17 };   // BrewBoiler, SteamBoiler, Pump, Solenoid
bool outputStates[] = { false, false, false };
const int numOutputs = 3;

bool logicStates[] = { false, false, false, false };  // BrewBoiler, SteamBoiler, ESTOP, EnableAll
const int numbLogicStates =  sizeof(logicStates) / sizeof(logicStates[0]);

const int physicalSwitchesPins[] = { 42, 41, 39 };  // BrewBoiler, SteamBoiler, ESTOP
bool physicalSwitchStates[] = { false, false, false };
const int numPhysicalSwitches =  sizeof(physicalSwitchesPins) / sizeof(physicalSwitchesPins[0]);

bool softwareSwitches[] = { false, false, false, false, false, false, false, false };  // BrewBoiler, SteamBoiler, ESTOP, EnableAll, Pump, Solenoid, firmwareRestart, Manual/Auto
const int numbSoftwareSwitches =  sizeof(softwareSwitches) / sizeof(softwareSwitches[0]);

#define beta 3950          // Beta coefficient of the thermistor    
#define R0 100000          // Resistance of the thermistor at 25°C (10kΩ)
double Kp = 2, Ki = 5, Kd = 1;      // PID Values
PID_v2 brewBoilerPID(Kp, Ki, Kd, PID::Direct);    // Define Brew Boiler PID
PID_v2 steamBoilerPID(Kp, Ki, Kd, PID::Direct);   // Define Steam Boiler PID
const int WindowSize = 5000;
unsigned long windowStartTime;
unsigned long timeEnteredRange = 0;
unsigned long timeEnteredSteamRange = 0;
bool withinBrewTempRange = false;
bool withinSteamTempRange = false;
double BrewTemp = 90.0; // Brew temperature
double SteamTemp = 120.0; // Steam temperature
const double TempRange = 5.0; // +/- 5 degrees Celsius **** THERMAL RUNAWAY ****
const unsigned long RequiredTimeInRange = 5000; // 5 seconds 
int boilerFilled = false;
const int fluidLevelPin = 40; //   STEAM BOILER FILL LEVEL
unsigned long motorEnableStartTime = 0;
bool isMotorEnableActive = false;

bool isAutoEnabled = 0;
int potPercent = 0;
int autoPercent = 0;
Servo flowServo;    // Define Servo
const int servoPin = 16;  
int servoPos = 180;
int flowValue = 0;  // Global variable to store Flow value

#define LED_PIN 7
#define NUM_LEDS 3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

unsigned long previousMillis = 0;  // Stores the last time LEDs were updated
const long interval = 100;         // Interval at which to blink (milliseconds)

// SOS Morse code timing
const int dotTime = 250;       // Duration of a dot
const int dashTime = 750;      // Duration of a dash
const int elementSpace = 250;  // Space between elements (dots/dashes) within a letter
const int letterSpace = 750;   // Space between letters
const int wordSpace = 1500;    // Space between words (SOS pattern repetitions)

const char sosPattern[] = "... --- ...";
int sosIndex = 0;                 // Current index in the SOS pattern
unsigned long patternMillis = 0;  // Timing for SOS pattern
 
bool isDisplayingSOS = false;   // Flag to check if SOS display is active

int calculateSMA(int* readings, int numReadings) {
      int sum = 0;
      for (int i = 0; i < numReadings; i++) {
        sum += readings[i];
      }
      return sum / numReadings;
  }

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML); // INDEX_HTML is your HTML content in index.h
  }

void flowValueCalc() {
  potPercent = map(analogRead(sensorPins[0]), 450, 2700, 0, 100);

  // if (potPercent > 30) {  // Debug
  //   Serial.println(potPercent);
  //  }

  if (softwareSwitches[7] == true)  {
    flowValue = autoPercent;
   } else {
    flowValue = potPercent;
   } 
  }

void brewPIDs() {
    const double input = analogRead(sensorPins[2]);
    const double output = brewBoilerPID.Run(input);


  // Check if temperature is within the desired range
  if (abs(input - BrewTemp) <= TempRange) {
    if (timeEnteredRange == 0) {
      // First time temperature is within the range
      timeEnteredRange = millis();
    } else if (millis() - timeEnteredRange >= RequiredTimeInRange) {
      // Temperature has been within the range for at least 5 seconds
      withinBrewTempRange = true;
    }
  } else {
    // Temperature is out of range
    withinBrewTempRange = false;
    timeEnteredRange = 0; // Reset the timer
  }

      while (millis() - windowStartTime > WindowSize) {
        // time to shift the Relay Window
        windowStartTime += WindowSize;
      }
      if (output < millis() - windowStartTime)
        digitalWrite(outputPins[0], HIGH);
      else
        digitalWrite(outputPins[0], LOW);
    }

void brewing() {
    if (flowValue > 30 && withinBrewTempRange == true) { 
      // digitalWrite(outputStates[2], true)
      // outputStates[2] = true;
    }
  }

void steamPIDs() {
    const double input = analogRead(sensorPins[3]);
    const double output = steamBoilerPID.Run(input);

    // Check if temperature is within the desired range for steam boiler
    if (abs(input - SteamTemp) <= TempRange) {
        if (timeEnteredSteamRange == 0) {
            timeEnteredSteamRange = millis();
        } else if (millis() - timeEnteredSteamRange >= RequiredTimeInRange) {
            withinSteamTempRange = true;
        }
    } else {
        withinSteamTempRange = false;
        timeEnteredSteamRange = 0;
    }

  while (millis() - windowStartTime > WindowSize) {
    // time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (output < millis() - windowStartTime)
    digitalWrite(outputPins[1], HIGH);
  else
    digitalWrite(outputPins[1], LOW);
  }

void handleLEDS() {
    unsigned long currentMillis = millis();
    unsigned long estopTriggeredMillis = 0;
    int eStopWait = 5000;
    int eStopActive = 0;

    // Handle sensor 3
    if (BrewBoilerCalc < 0) {
      displaySOS(currentMillis, 0);
    } else {
      if (logicStates[0] == true) {
        leds[0] = temperatureToColor(BrewBoilerCalc);
      } else {
        leds[0] = CRGB::Black;
      }
    }

    // Handle sensor 4
    if (SteamBoilerCalc < 0) {
      displaySOS(currentMillis, 1);
    } else {
      if (logicStates[1] == true) {
        leds[1] = temperatureToColor(SteamBoilerCalc);
      } else {
        leds[1] = CRGB::Black;
      }
    }

    // Handle LED 3 based on motor usage
    if (flowValue < 50) {
      leds[2] = CRGB::White;  // Set to a solid color when motor is in use
    } else {
      // Pulsing white LED for default behavior
      leds[2] = CRGB::White;
      uint8_t brightness = (sin(currentMillis / 200.0) + 1) * 127.5;
      leds[2].fadeLightBy(255 - brightness);
    }

    FastLED.show();
    previousMillis = currentMillis;
  }

  CRGB temperatureToColor(int temperature) {
    temperature = constrain(temperature, 15, 100);       // Constrain to your temperature range
    int redToGreen = map(temperature, 15, 100, 0, 255);  // Map temperature from red to green
    return CRGB(redToGreen, 255 - redToGreen, 0);
  }

  void displaySOS(unsigned long currentMillis, int ledIndex) {
    if (sosIndex < sizeof(sosPattern) - 1) {
      switch (sosPattern[sosIndex]) {
        case '.':
          if (currentMillis - patternMillis < dotTime) {
            leds[ledIndex] = CRGB::Red;  // Turn on the LED in red for a dot
          } else {
            leds[ledIndex] = CRGB::Black;  // Turn off the LED
            if (currentMillis - patternMillis >= dotTime + elementSpace) {
              patternMillis = currentMillis;
              sosIndex++;
            }
          }
          break;
        case '-':
          if (currentMillis - patternMillis < dashTime) {
            leds[ledIndex] = CRGB::Red;  // Turn on the LED in red for a dash
          } else {
            leds[ledIndex] = CRGB::Black;  // Turn off the LED
            if (currentMillis - patternMillis >= dashTime + elementSpace) {
              patternMillis = currentMillis;
              sosIndex++;
            }
          }
          break;
        case ' ':
          if (currentMillis - patternMillis >= letterSpace) {
            patternMillis = currentMillis;
            sosIndex++;
          }
          break;
      }
    } else {
      if (currentMillis - patternMillis >= wordSpace) {
        patternMillis = currentMillis;
        sosIndex = 0;  // Restart the SOS pattern
      }
    }
  }

  void setAllLeds(CRGB color) {
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
  }
  //////////////////////////// End of LED Code ////////////////


void fillBoiler() {
  unsigned long previousMillis = 0; // stores last time the update occurred
  const long interval = 1000;       // interval at which to wait (1 second)
  bool boilerFill = 0;


      if (isEstopTriggered == LOW) {
        if (boilerFilled == LOW) {
          digitalWrite(outputPins[3], HIGH);  // Activate Solenoid
          unsigned long currentMillis = millis();

          if (currentMillis - previousMillis >= interval) {
              // save the last time you updated the state
              previousMillis = currentMillis;   

          digitalWrite(outputPins[2], HIGH);
      }
        } else {
      digitalWrite(outputPins[3], LOW); // Close Solenoid
      }
    } 
  }


void emergencyStop() {
  //EMERGENCY STOP LOGIC !! DO NOT CHANGE
  Serial.println("Emergency Stop Activated");
  isEstopTriggered = true;
  for (int i = 0; i < numOutputs; i++) {
    digitalWrite(outputPins[i], LOW);
  }
  }

void firmwareRestart() {
  Serial.println("Firmware Restarting in 3");
  delay(1000);
  Serial.println("Firmware Restarting in 2");
  delay(1000);
  Serial.println("Firmware Restarting in 1");
  delay(1000);
  Serial.println("Firmware Restarting");
  ESP.restart();
  }


void calcTemp() {
  for (int i = 0; i < numSensors; i++) {
      sensorReadings[i][sensorIndex] = analogRead(sensorPins[i]);
      if (i == 0) {
        sensorValue1 = calculateSMA(sensorReadings[0], numReadings);
      }
      if (i == 1) {  // Sensor 3 Temperature Calculation
        int a = analogRead(sensorPins[1]);
        if (a != 4095) {
          float R = R0 * ((4095.0 / a) - 1.0);
          float tempK = 1.0 / (1.0 / (298.15) + (1.0 / beta) * log(R / R0));
          BrewBoilerCalc = tempK - 273.15;
        } else {
          BrewBoilerCalc = NAN;
        }
      }
      if (i == 2) {  // Sensor 4 Temperature Calculation
        int a = analogRead(sensorPins[2]);
        if (a != 4095) {
          float R = R0 * ((4095.0 / a) - 1.0);
          float tempK = 1.0 / (1.0 / (298.15) + (1.0 / beta) * log(R / R0));
          SteamBoilerCalc = tempK - 273.15;
        } else {
          SteamBoilerCalc = NAN;
        }
      }
    }

  sensorIndex = (sensorIndex + 1) % numReadings;
  }

void handleOutputs() {
  // for(int i = 0; i < numOutputs; i++) {
  //     if(outputStates[i]) {
  //         digitalWrite(outputPins[i], HIGH); // Set the pin to high if the state is true
  //     } else {
  //         digitalWrite(outputPins[i], LOW); // Set the pin to low if the state is false
  //     }
  //  }
  }

void softwareButtons() {
  if (softwareSwitches[0] == 1) {
    // BrewBoiler()
    logicStates[0] = 1;
  } else if (physicalSwitchStates[0] == 0 && softwareSwitches[0] == 0) {
    logicStates[0] = 0;
  }

  if (softwareSwitches[1] == 1) {
    // SteamBoiler()
    logicStates[1] = 1;
  } else if (physicalSwitchStates[1] == 0 && softwareSwitches[1] == 0) {
      logicStates[1] = 0;
  }

  if (softwareSwitches[2] == true) {
    //EStop()
    logicStates[2] = 1;
    emergencyStop();
  } else if (physicalSwitchStates[2] == 0 && softwareSwitches[2] == 0) {
      logicStates[2] = 0;
  }

  if (softwareSwitches[3] == true) {
    //EnableAll
    logicStates[3] = 1;
  } else {
      logicStates[3] = 0;
  }


    if (softwareSwitches[4] == true && !isMotorEnableActive) {
        // fillBoiler();
        digitalWrite(outputStates[2], HIGH);
        Serial.println("Motor enabled");
        motorEnableStartTime = millis();
        isMotorEnableActive = true;
    }

    if (isMotorEnableActive && millis() - motorEnableStartTime > 20000) { // 20 seconds
        digitalWrite(outputStates[2], LOW);
        Serial.println("Motor disabled");
        isMotorEnableActive = false;
        softwareSwitches[4] = 0;
    }

  if (softwareSwitches[5] == true) {
    //Solenoid *** IF PUMP ISNT ACTIVE OR IF BOILERFILLED IS FALSE ***
    if (outputStates[2] = false || boilerFilled == false) {
      outputStates[3] = true;
    }
   }

  if (softwareSwitches[6] == true && isEstopTriggered == true) {
    // Firmware Restart ** IF Estop active **
      firmwareRestart();
   }

  if (softwareSwitches[7] == true) {
    // Manual / Auto
    isAutoEnabled = 1;
   } else {
    isAutoEnabled = 0;
   }
  }

void physicalButtons() {
  for(int i = 0; i < numPhysicalSwitches; i++) {
    if(physicalSwitchesPins[i] == HIGH) {
      physicalSwitchStates[i] = true;
    } else {
      physicalSwitchStates[i] = false;
    }
   }
  }

void setup() {
  Serial.begin(115200);

  WiFi.setHostname("The_Valkyrie"); // Replace 'espDeviceName' with your desired hostname

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  server.on("/", handleRoot); // Route for root / web page
  server.begin();

  // Start mDNS with hostname 'espEspresso'
  if (!MDNS.begin("espEspresso")) {
      Serial.println("Error setting up MDNS responder!");
      while (1) {
          delay(1000);
      }
    }
    Serial.println("mDNS responder started");



  server.on("/getData", HTTP_GET, handleGetDataRequest);
  for (int i = 0; i < numbSoftwareSwitches; i++) {
      server.on("/toggleSoftwareSwitch" + String(i), HTTP_GET, [i](){ handleSoftwareSwitchToggle(i); });
  }

  server.on("/setAutoPercent", HTTP_GET, []() {
      if (server.hasArg("value")) {
          autoPercent = server.arg("value").toInt();
          server.send(200, "text/plain", "Auto Percent set to " + String(autoPercent));
      } else {
          server.send(400, "text/plain", "Value not provided");
      }
  });

  server.on("/setBrewTemp", HTTP_GET, []() {
      if (server.hasArg("value")) {  // changed from "temp" to "value"
          BrewTemp = server.arg("value").toFloat();  // changed from "temp" to "value"
          server.send(200, "text/plain", "BrewTemp set to " + String(BrewTemp));
      } else {
          server.send(400, "text/plain", "Temperature not provided");
      }
  });


  server.on("/setSteamTemp", HTTP_GET, []() {
      if (server.hasArg("value")) {  // changed from "temp" to "value"
          SteamTemp = server.arg("value").toFloat();  // changed from "temp" to "value"
          server.send(200, "text/plain", "SteamTemp set to " + String(SteamTemp));
      } else {
          server.send(400, "text/plain", "Temperature not provided");
      }
  });



  pinMode(fluidLevelPin, INPUT);
  for (int i = 0; i < numOutputs; i++) { // Set Output pins to Output
      pinMode(outputPins[i], OUTPUT);
  }
  for (int i = 0; i < numSensors; i++) { // Set Sensor pins to Input
      pinMode(sensorPins[i], INPUT);
  }

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  analogReadResolution(12);  // Set the analog read resolution to 12-bit for ESP32
  
  windowStartTime = millis();
    brewBoilerPID.SetOutputLimits(0, WindowSize);                               ////////////////////////////////////////////////
    brewBoilerPID.Start(analogRead(sensorPins[2]),  // input                    ////////////////////////////////////////////////
                        0,                          // current output           ////////////////////////////////////////////////
                        BrewTemp);                  // setpoint                 ////////////////                    ////////////
                                                                                ////////////////    Boiler Start    ////////////
    steamBoilerPID.SetOutputLimits(0, WindowSize);                              ////////////////   And Set Limits   ////////////
    steamBoilerPID.Start(analogRead(sensorPins[3]),  // input                   ////////////////                    ////////////
                        0,                          // current output           ////////////////////////////////////////////////
                        SteamTemp);                 // setpoint                 ////////////////////////////////////////////////


  ESP32PWM::allocateTimer(0);                                 ////////////////////////////////////////////////
  ESP32PWM::allocateTimer(1);                                 ////////////////////////////////////////////////
  ESP32PWM::allocateTimer(2);                                 ////////////////    Servo PWM       ////////////
  ESP32PWM::allocateTimer(3);                                 ////////////////     Handling       ////////////
  flowServo.setPeriodHertz(50);  // standard 50 hz servo      ////////////////////////////////////////////////
  flowServo.attach(servoPin, 100, 3000);                      ////////////////////////////////////////////////

  }

void loop() {
  if (physicalSwitchStates[2] == true || softwareSwitches[2] == true) {  // EMERGENCY STOP LOGIC !! DO NOT CHANGE
    emergencyStop();
  }

  // Serial.println(String(logicStates[0]) + "," +     // DEBUG
  //              String(softwareSwitches[0]));
  //Serial.println("BrewTemp = " + String(BrewTemp));
  //Serial.println("SteamTemp = " + String(SteamTemp));
  // Serial.println(boilerFilled);

  calcTemp();
  flowValueCalc();
  handleOutputs();
  handleLEDS();
  server.handleClient();
  softwareButtons();
  boilerFilled = digitalRead(fluidLevelPin);


  if (isEstopTriggered == false) {  // MASTER ESTOP LOGIC
    physicalButtons();
    brewing();

    if (logicStates[0] == true) { // Enable BrewPID Boiler
        brewPIDs();
    } else {
        digitalWrite(outputPins[0], LOW); // Turn off the heating element
    }

    if (logicStates[1] == true){    // Enable SteamPID Boiler
      outputStates[1] = true;
        if (boilerFilled == true) {
          steamPIDs();
        } else {
          Serial.println("Error Boiler Empty, Filling Now");
          fillBoiler(); 
        }
    } else {
      digitalWrite(outputPins[1], LOW); // Turn off the heating element
    }


    }
  }
 
void handleGetDataRequest() {
    String xml = "<data>\n";

    // Estop
    xml += "<isEstopTriggered><value>" + String(isEstopTriggered) + "</value></isEstopTriggered>\n";

    // BrewBoilerCalc
    xml += "<BrewBoilerCalc><value>" + String(BrewBoilerCalc) + "</value></BrewBoilerCalc>\n";

    // SteamBoilerCalc
    xml += "<SteamBoilerCalc><value>" + String(SteamBoilerCalc) + "</value></SteamBoilerCalc>\n";

    // BrewBoilerCalc
    xml += "<BrewBoilerTarget><value>" + String(BrewTemp) + "</value></BrewBoilerTarget>\n";

    // SteamBoilerCalc
    xml += "<SteamBoilerTarget><value>" + String(SteamTemp) + "</value></SteamBoilerTarget>\n";

    // potPercent
    xml += "<potPercent><value>" + String(potPercent) + "</value></potPercent>\n";

    // isAutoEnabled
    xml += "<autoEnabled><value>" + String(isAutoEnabled) + "</value></autoEnabled>\n";

    // boilerFilled
    xml += "<boilerFilled><value>" + String(boilerFilled) + "</value></boilerFilled>\n";

    // outputStates
    xml += "<outputStates>\n";
    for (int i = 0; i < numOutputs; i++) {
        xml += "<state id=\"" + String(i) + "\" value=\"" + String(outputStates[i]) + "\"/>\n";
    }
    xml += "</outputStates>\n";

    // logicStates
    xml += "<logicStates>\n";
    for (int i = 0; i < numbLogicStates; i++) {
        xml += "<state id=\"" + String(i) + "\" value=\"" + String(logicStates[i]) + "\"/>\n";
    }
    xml += "</logicStates>\n";

    xml += "</data>";

    server.send(200, "text/xml", xml);
  }



void handleSoftwareSwitchToggle(int switchNum) {
    bool state = !softwareSwitches[switchNum];
    softwareSwitches[switchNum] = state;
    Serial.print("Switch number: ");
    Serial.print(switchNum);
    Serial.print(" toggled to: ");
    Serial.println(state ? "on" : "off");
    server.send(200, "text/plain", state ? "on" : "off");
}



