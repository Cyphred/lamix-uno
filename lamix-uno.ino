#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);	

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

#define MQ3 A1
#define PH A0
#define AGITATOR_RELAY 7
#define CLEANER_RELAY 6
#define FAN_RELAY 5
#define HEATER_RELAY 4
#define PUMP_RELAY 3

#define FERMENTATION_PHASE 0
#define DISTILLATION_PHASE 1
#define FILTRATION_PHASE 2

struct {
  int pin = PH;
  int samples = 10;
  float adc_resolution = 1024.0;
} phSensor;

struct {
  bool fan = false;
  bool heater = false;
  bool pump = false;
  bool cleaner = false;
  bool agitator = false;
} relay_states;

float voltageToPh(float voltage) {
    /*
    // Calculate the slope and offset based on the new calibration values
    float slope = (7.0 - 2.7) / (2.7 - 2.86);
    float offset = 7.0 - slope * 2.7;

    return slope * voltage + offset;
    */
    return 7 + ((2.5 - voltage) / 0.18);
}

float getPhReading() {
  int measurings = 0;

  for (int i = 0; i < phSensor.samples; i++) {
    measurings += analogRead(phSensor.pin);
    delay(10);
  }

  float voltage = 5.0 / phSensor.adc_resolution * measurings / phSensor.samples;
  return voltageToPh(voltage);
}

float getEthanolReading() {
  float value = analogRead(MQ3);
  return value;
}

void setFan(bool state) {
  if (state) {
    relay_states.fan = true;
    digitalWrite(FAN_RELAY, LOW);
  } else {
    relay_states.fan = false;
    digitalWrite(FAN_RELAY, HIGH);
  }
}

void setHeater(bool state) {
  if (state) {
    relay_states.heater = true;
    digitalWrite(HEATER_RELAY, LOW);
  } else {
    relay_states.heater = false;
    digitalWrite(HEATER_RELAY, HIGH);
  }
}

void setPump(bool state) {
  if (state) {
    relay_states.pump = true;
    digitalWrite(PUMP_RELAY, LOW);
  } else {
    relay_states.pump = false;
    digitalWrite(PUMP_RELAY, HIGH);
  }
}

void setCleaner(bool state) {
  if (state) {
    relay_states.cleaner = true;
    digitalWrite(CLEANER_RELAY, LOW);
  } else {
    relay_states.cleaner = false;
    digitalWrite(CLEANER_RELAY, HIGH);
  }
}

void setAgitator(bool state) {
  if (state) {
    relay_states.agitator = true;
    digitalWrite(AGITATOR_RELAY, LOW);
  } else {
    relay_states.agitator = false;
    digitalWrite(AGITATOR_RELAY, HIGH);
  }
}

void setup() {
  sensors.begin();
  Serial.begin(9600);
  pinMode(FAN_RELAY, OUTPUT);
  pinMode(HEATER_RELAY, OUTPUT);
  pinMode(PUMP_RELAY, OUTPUT);
  pinMode(CLEANER_RELAY, OUTPUT);
  pinMode(AGITATOR_RELAY, OUTPUT);

  // Turn off all relays on startup
  setFan(false);
  setHeater(false);
  setPump(false);
  setCleaner(false);
  setAgitator(false);

  delay(100);
}


void loop() {
  // This will interpret incoming data over serial
  // from the orange pi. It can be several consecutive
  // characters like '13579' which turns on everything
  while (Serial.available()) {
    byte input = Serial.read();
    
    switch (input) {
      case '1':
        setFan(true);
        break;
      case '2':
        setFan(false);
        break;
      case '3':
        setHeater(true);
        break;
      case '4':
        setHeater(false);
        break;
      case '5':
        setPump(true);
        break;
      case '6':
        setPump(false);
        break;
      case '7':
        setCleaner(true);
        break;
      case '8':
        setCleaner(false);
        break;
      case '9':
        setAgitator(true);
        break;
      case '0':
        setAgitator(false);
        break;
    }
  }

  sensors.requestTemperatures(); 

  float temp = sensors.getTempCByIndex(0);
  float ph = getPhReading();            // Has a 100ms delay
  float ethanol = getEthanolReading();  // 0ms delay on this one

  // Retain this format unless you've changed the
  // python-side code to accept a new/altered format
  // of passing the data via serial
  Serial.print(ph);
  Serial.print(",");
  Serial.print(ethanol);
  Serial.print(",");
  Serial.print(temp);
  Serial.print(",");
  Serial.print(relay_states.fan);
  Serial.print(",");
  Serial.print(relay_states.heater);
  Serial.print(",");
  Serial.print(relay_states.pump);
  Serial.print(",");
  Serial.print(relay_states.cleaner);
  Serial.print(",");
  Serial.print(relay_states.agitator);

  Serial.println();
}
