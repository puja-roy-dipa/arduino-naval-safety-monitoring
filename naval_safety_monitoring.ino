/*
  ==================================================================
  Arduino-Based Naval Safety Monitoring & Obstacle Detection System
  ==================================================================

  Course:
  ECE 2200 - Electronic Shop Practice

  Department:
  Electrical & Computer Engineering

  Institution:
  Rajshahi University of Engineering & Technology (RUET)

  Author:
  Puja Roy Dipa

  ------------------------------------------------------------------
  IMPLEMENTATION NOTE
  ------------------------------------------------------------------

  The original source code developed during the 2023 academic
  project is no longer available.

  This program is a reconstructed implementation based on the
  original project concept, remembered functionality, and intended
  system architecture.

  ------------------------------------------------------------------
  MONITORED CONDITIONS
  ------------------------------------------------------------------

  1. Temperature
  2. Gas / smoke level
  3. Water / bilge level
  4. Flame / fire condition
  5. Vessel tilt / abnormal inclination
  6. Battery voltage
  7. Emergency push button
  8. Nearby obstacles
  9. Approximate obstacle bearing
  10. GPS latitude
  11. GPS longitude
  12. GPS movement speed

  ------------------------------------------------------------------
  OUTPUTS
  ------------------------------------------------------------------

  - Warning LED
  - Safe-status LED
  - Buzzer
  - Serial Monitor
  - Servo-based ultrasonic scanning

  ------------------------------------------------------------------
  IMPORTANT
  ------------------------------------------------------------------

  The servo-mounted ultrasonic system is an ultrasonic scanner,
  although similar projects are often referred to as "Arduino radar."

  GPS provides geographic position.

  The ultrasonic scanner provides local obstacle distance and
  approximate angular bearing.
*/


// ==================================================================
// LIBRARIES
// ==================================================================

#include <Servo.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>


// ==================================================================
// ANALOG SENSOR PINS
// ==================================================================

const int temperatureSensorPin = A0;
const int gasSensorPin         = A1;
const int waterSensorPin       = A2;
const int batterySensorPin     = A3;


// ==================================================================
// DIGITAL SENSOR PINS
// ==================================================================

const int flameSensorPin       = 2;
const int tiltSensorPin        = 3;
const int emergencyButtonPin   = 4;


// ==================================================================
// ULTRASONIC SENSOR PINS
// ==================================================================

const int trigPin = 5;
const int echoPin = 6;


// ==================================================================
// SERVO MOTOR
// ==================================================================

const int servoPin = 7;


// ==================================================================
// OUTPUT DEVICES
// ==================================================================

const int buzzerPin     = 8;
const int warningLEDPin = 9;
const int safeLEDPin    = 10;


// ==================================================================
// GPS COMMUNICATION PINS
// ==================================================================

/*
  Arduino Pin 11 <- GPS TX
  Arduino Pin 12 -> GPS RX

  GPS RX is normally the important connection when the Arduino
  only needs to receive location information.
*/

const int gpsRxPin = 11;
const int gpsTxPin = 12;


// ==================================================================
// OBJECTS
// ==================================================================

Servo radarServo;

TinyGPSPlus gps;

SoftwareSerial gpsSerial(
  gpsRxPin,
  gpsTxPin
);


// ==================================================================
// ADC SETTINGS
// ==================================================================

const float referenceVoltage = 5.0;
const int adcResolution = 1023;


// ==================================================================
// SAFETY THRESHOLDS
// ==================================================================

const float temperatureThreshold = 45.0;

const int gasThreshold = 60;

const int waterThreshold = 75;

const float lowBatteryThreshold = 10.5;

const float obstacleWarningDistance = 100.0;


// ==================================================================
// BATTERY VOLTAGE DIVIDER
// ==================================================================

/*
  Example voltage-divider values:

  R1 = 30 kOhm
  R2 = 7.5 kOhm

             R1
  Battery ---/\/\----+---- Arduino Analog Input
                     |
                    /\/\
                     R2
                     |
                    GND

  Relationship:

  Vin = Vout * (R1 + R2) / R2

  These resistor values are demonstration values and should be
  selected appropriately for the actual battery system.
*/

const float R1 = 30000.0;
const float R2 = 7500.0;


// ==================================================================
// ULTRASONIC SCANNER VARIABLES
// ==================================================================

float nearestObjectDistance = 999.0;

int nearestObjectAngle = -1;


// ==================================================================
// GPS VARIABLES
// ==================================================================

double currentLatitude  = 0.0;
double currentLongitude = 0.0;

double currentSpeedKmph = 0.0;

bool gpsLocationValid = false;


// ==================================================================
// TEMPERATURE SENSOR FUNCTION
// ==================================================================

float readTemperature()
{
  int rawValue =
    analogRead(temperatureSensorPin);

  float voltage =
    rawValue *
    (referenceVoltage / adcResolution);

  /*
    LM35-type sensor:

    10 mV per degree Celsius

    Temperature = Voltage * 100
  */

  float temperature =
    voltage * 100.0;

  return temperature;
}


// ==================================================================
// GENERIC ANALOG SENSOR FUNCTION
// ==================================================================

int readSensorPercentage(int pin)
{
  int rawValue =
    analogRead(pin);

  int percentage =
    map(
      rawValue,
      0,
      adcResolution,
      0,
      100
    );

  percentage =
    constrain(
      percentage,
      0,
      100
    );

  return percentage;
}


// ==================================================================
// BATTERY VOLTAGE FUNCTION
// ==================================================================

float readBatteryVoltage()
{
  int rawValue =
    analogRead(batterySensorPin);

  float measuredVoltage =
    rawValue *
    (referenceVoltage / adcResolution);

  float actualVoltage =
    measuredVoltage *
    ((R1 + R2) / R2);

  return actualVoltage;
}


// ==================================================================
// ULTRASONIC DISTANCE MEASUREMENT
// ==================================================================

float measureDistance()
{
  digitalWrite(
    trigPin,
    LOW
  );

  delayMicroseconds(2);


  digitalWrite(
    trigPin,
    HIGH
  );

  delayMicroseconds(10);


  digitalWrite(
    trigPin,
    LOW
  );


  /*
    Wait for the return echo.

    30,000 microseconds is used as a timeout.
  */

  unsigned long duration =
    pulseIn(
      echoPin,
      HIGH,
      30000
    );


  /*
    No valid echo received.
  */

  if (duration == 0)
  {
    return 999.0;
  }


  /*
    Speed of sound:

    approximately 0.0343 cm per microsecond.

    Divide by two because sound travels:

    Sensor -> Object -> Sensor
  */

  float distance =
    duration *
    0.0343 /
    2.0;


  /*
    Ignore unrealistic very short measurements.
  */

  if (distance < 2.0)
  {
    return 999.0;
  }


  return distance;
}


// ==================================================================
// ULTRASONIC SCANNING FUNCTION
// ==================================================================

void performRadarScan()
{
  nearestObjectDistance = 999.0;
  nearestObjectAngle = -1;


  Serial.println();

  Serial.println(
    "========================================"
  );

  Serial.println(
    " ULTRASONIC OBSTACLE SCAN"
  );

  Serial.println(
    "========================================"
  );


  /*
    Scan from 15 degrees to 165 degrees.

    Step size:
    15 degrees.
  */

  for (
    int angle = 15;
    angle <= 165;
    angle += 15
  )
  {
    radarServo.write(angle);


    /*
      Give the servo time to reach the
      requested angular position.
    */

    delay(120);


    float distance =
      measureDistance();


    Serial.print(
      "Angle: "
    );

    Serial.print(
      angle
    );

    Serial.print(
      " deg"
    );


    Serial.print(
      " | Distance: "
    );


    if (distance >= 999.0)
    {
      Serial.println(
        "No Echo"
      );
    }
    else
    {
      Serial.print(
        distance,
        1
      );

      Serial.println(
        " cm"
      );
    }


    /*
      Track the nearest detected object.
    */

    if (
      distance <
      nearestObjectDistance
    )
    {
      nearestObjectDistance =
        distance;

      nearestObjectAngle =
        angle;
    }
  }


  /*
    Return the scanner to the forward position.
  */

  radarServo.write(
    90
  );


  Serial.println();


  if (
    nearestObjectAngle >= 0 &&
    nearestObjectDistance < 999.0
  )
  {
    Serial.print(
      "Nearest Object: "
    );

    Serial.print(
      nearestObjectDistance,
      1
    );

    Serial.print(
      " cm at "
    );

    Serial.print(
      nearestObjectAngle
    );

    Serial.println(
      " degrees"
    );
  }
  else
  {
    Serial.println(
      "No valid nearby object detected."
    );
  }
}


// ==================================================================
// GPS READING FUNCTION
// ==================================================================

void updateGPS()
{
  unsigned long startTime =
    millis();


  /*
    Read GPS characters for a short interval
    during each monitoring cycle.
  */

  while (
    millis() - startTime < 500
  )
  {
    while (
      gpsSerial.available() > 0
    )
    {
      char gpsCharacter =
        gpsSerial.read();

      gps.encode(
        gpsCharacter
      );
    }
  }


  /*
    Update location if the GPS data is valid.
  */

  if (
    gps.location.isValid()
  )
  {
    currentLatitude =
      gps.location.lat();

    currentLongitude =
      gps.location.lng();

    gpsLocationValid =
      true;
  }


  /*
    Update movement speed if available.
  */

  if (
    gps.speed.isValid()
  )
  {
    currentSpeedKmph =
      gps.speed.kmph();
  }
}


// ==================================================================
// GPS DISPLAY FUNCTION
// ==================================================================

void displayGPS()
{
  Serial.println();

  Serial.println(
    "----------------------------------------"
  );

  Serial.println(
    " GPS STATUS"
  );

  Serial.println(
    "----------------------------------------"
  );


  if (gpsLocationValid)
  {
    Serial.print(
      "Latitude  : "
    );

    Serial.println(
      currentLatitude,
      6
    );


    Serial.print(
      "Longitude : "
    );

    Serial.println(
      currentLongitude,
      6
    );


    Serial.print(
      "Speed     : "
    );

    Serial.print(
      currentSpeedKmph,
      2
    );

    Serial.println(
      " km/h"
    );
  }
  else
  {
    Serial.println(
      "GPS location not available."
    );

    Serial.println(
      "Waiting for satellite fix..."
    );
  }
}


// ==================================================================
// ALARM CONTROL
// ==================================================================

void activateAlarm()
{
  digitalWrite(
    warningLEDPin,
    HIGH
  );

  digitalWrite(
    safeLEDPin,
    LOW
  );


  /*
    Produce an audible warning tone.
  */

  tone(
    buzzerPin,
    1000
  );
}


void deactivateAlarm()
{
  digitalWrite(
    warningLEDPin,
    LOW
  );

  digitalWrite(
    safeLEDPin,
    HIGH
  );

  noTone(
    buzzerPin
  );
}


// ==================================================================
// SYSTEM HEADER
// ==================================================================

void printSystemHeader()
{
  Serial.println();

  Serial.println(
    "================================================="
  );

  Serial.println(
    " NAVAL SAFETY & OBSTACLE MONITORING SYSTEM"
  );

  Serial.println(
    "================================================="
  );

  Serial.println();

  Serial.println(
    "System initialized successfully."
  );

  Serial.println();
}


// ==================================================================
// SETUP
// ==================================================================

void setup()
{
  // ---------------------------------------------------------------
  // SERIAL MONITOR
  // ---------------------------------------------------------------

  Serial.begin(
    9600
  );


  // ---------------------------------------------------------------
  // GPS SERIAL
  // ---------------------------------------------------------------

  gpsSerial.begin(
    9600
  );


  // ---------------------------------------------------------------
  // DIGITAL INPUTS
  // ---------------------------------------------------------------

  pinMode(
    flameSensorPin,
    INPUT
  );


  pinMode(
    tiltSensorPin,
    INPUT
  );


  /*
    Emergency button uses the Arduino
    internal pull-up resistor.

    Normal state:
    HIGH

    Button pressed:
    LOW
  */

  pinMode(
    emergencyButtonPin,
    INPUT_PULLUP
  );


  // ---------------------------------------------------------------
  // ULTRASONIC SENSOR
  // ---------------------------------------------------------------

  pinMode(
    trigPin,
    OUTPUT
  );

  pinMode(
    echoPin,
    INPUT
  );


  // ---------------------------------------------------------------
  // OUTPUT DEVICES
  // ---------------------------------------------------------------

  pinMode(
    buzzerPin,
    OUTPUT
  );

  pinMode(
    warningLEDPin,
    OUTPUT
  );

  pinMode(
    safeLEDPin,
    OUTPUT
  );


  // ---------------------------------------------------------------
  // SERVO INITIALIZATION
  // ---------------------------------------------------------------

  radarServo.attach(
    servoPin
  );

  radarServo.write(
    90
  );


  // ---------------------------------------------------------------
  // INITIAL OUTPUT STATE
  // ---------------------------------------------------------------

  digitalWrite(
    warningLEDPin,
    LOW
  );

  digitalWrite(
    safeLEDPin,
    HIGH
  );

  noTone(
    buzzerPin
  );


  // ---------------------------------------------------------------
  // STARTUP MESSAGE
  // ---------------------------------------------------------------

  printSystemHeader();


  delay(
    1000
  );
}


// ==================================================================
// MAIN LOOP
// ==================================================================

void loop()
{
  // ===============================================================
  // READ ANALOG CONDITIONS
  // ===============================================================

  float temperature =
    readTemperature();


  int gasLevel =
    readSensorPercentage(
      gasSensorPin
    );


  int waterLevel =
    readSensorPercentage(
      waterSensorPin
    );


  float batteryVoltage =
    readBatteryVoltage();


  // ===============================================================
  // READ DIGITAL CONDITIONS
  // ===============================================================

  /*
    Many flame sensor modules are active LOW.

    If the specific sensor being used is active HIGH,
    reverse this logic.
  */

  bool flameDetected =
    (
      digitalRead(
        flameSensorPin
      ) == LOW
    );


  /*
    Tilt sensor behavior depends on the module.

    This reconstructed example treats HIGH as
    an abnormal tilt condition.
  */

  bool abnormalTilt =
    (
      digitalRead(
        tiltSensorPin
      ) == HIGH
    );


  /*
    Emergency button uses INPUT_PULLUP.

    LOW means the button has been pressed.
  */

  bool emergencyPressed =
    (
      digitalRead(
        emergencyButtonPin
      ) == LOW
    );


  // ===============================================================
  // DISPLAY ENVIRONMENTAL & VESSEL STATUS
  // ===============================================================

  Serial.println();

  Serial.println(
    "----------------------------------------"
  );

  Serial.println(
    " ENVIRONMENTAL & VESSEL STATUS"
  );

  Serial.println(
    "----------------------------------------"
  );


  // ---------------------------------------------------------------
  // TEMPERATURE
  // ---------------------------------------------------------------

  Serial.print(
    "Temperature         : "
  );

  Serial.print(
    temperature,
    1
  );

  Serial.println(
    " C"
  );


  // ---------------------------------------------------------------
  // GAS / SMOKE
  // ---------------------------------------------------------------

  Serial.print(
    "Gas / Smoke Level   : "
  );

  Serial.print(
    gasLevel
  );

  Serial.println(
    " %"
  );


  // ---------------------------------------------------------------
  // WATER / BILGE LEVEL
  // ---------------------------------------------------------------

  Serial.print(
    "Water / Bilge Level : "
  );

  Serial.print(
    waterLevel
  );

  Serial.println(
    " %"
  );


  // ---------------------------------------------------------------
  // BATTERY
  // ---------------------------------------------------------------

  Serial.print(
    "Battery Voltage     : "
  );

  Serial.print(
    batteryVoltage,
    2
  );

  Serial.println(
    " V"
  );


  // ---------------------------------------------------------------
  // FLAME
  // ---------------------------------------------------------------

  Serial.print(
    "Flame Detection     : "
  );

  if (flameDetected)
  {
    Serial.println(
      "DETECTED"
    );
  }
  else
  {
    Serial.println(
      "Normal"
    );
  }


  // ---------------------------------------------------------------
  // TILT
  // ---------------------------------------------------------------

  Serial.print(
    "Vessel Tilt         : "
  );

  if (abnormalTilt)
  {
    Serial.println(
      "ABNORMAL"
    );
  }
  else
  {
    Serial.println(
      "Normal"
    );
  }


  // ---------------------------------------------------------------
  // EMERGENCY BUTTON
  // ---------------------------------------------------------------

  Serial.print(
    "Emergency Button    : "
  );

  if (emergencyPressed)
  {
    Serial.println(
      "PRESSED"
    );
  }
  else
  {
    Serial.println(
      "Normal"
    );
  }


  // ===============================================================
  // UPDATE GPS
  // ===============================================================

  updateGPS();

  displayGPS();


  // ===============================================================
  // PERFORM ULTRASONIC SCANNING
  // ===============================================================

  performRadarScan();


  // ===============================================================
  // EVALUATE INDIVIDUAL SAFETY CONDITIONS
  // ===============================================================

  bool highTemperature =
    (
      temperature >=
      temperatureThreshold
    );


  bool highGas =
    (
      gasLevel >=
      gasThreshold
    );


  bool highWater =
    (
      waterLevel >=
      waterThreshold
    );


  bool lowBattery =
    (
      batteryVoltage <=
      lowBatteryThreshold
    );


  bool nearbyObstacle =
    (
      nearestObjectDistance <=
      obstacleWarningDistance
    );


  // ===============================================================
  // OVERALL HAZARD STATE
  // ===============================================================

  bool hazardDetected =
    highTemperature ||
    highGas ||
    highWater ||
    flameDetected ||
    abnormalTilt ||
    lowBattery ||
    nearbyObstacle ||
    emergencyPressed;


  // ===============================================================
  // DISPLAY SAFETY ANALYSIS
  // ===============================================================

  Serial.println();

  Serial.println(
    "----------------------------------------"
  );

  Serial.println(
    " SAFETY ANALYSIS"
  );

  Serial.println(
    "----------------------------------------"
  );


  // ===============================================================
  // HAZARDOUS CONDITION
  // ===============================================================

  if (hazardDetected)
  {
    Serial.println();

    Serial.println(
      "WARNING: HAZARDOUS CONDITION DETECTED"
    );

    Serial.println();


    // -------------------------------------------------------------
    // TEMPERATURE ALERT
    // -------------------------------------------------------------

    if (highTemperature)
    {
      Serial.println(
        "[ALERT] Excessive temperature"
      );
    }


    // -------------------------------------------------------------
    // GAS / SMOKE ALERT
    // -------------------------------------------------------------

    if (highGas)
    {
      Serial.println(
        "[ALERT] High gas / smoke concentration"
      );
    }


    // -------------------------------------------------------------
    // WATER / BILGE ALERT
    // -------------------------------------------------------------

    if (highWater)
    {
      Serial.println(
        "[ALERT] High bilge / water level"
      );
    }


    // -------------------------------------------------------------
    // FIRE ALERT
    // -------------------------------------------------------------

    if (flameDetected)
    {
      Serial.println(
        "[ALERT] Flame / fire detected"
      );
    }


    // -------------------------------------------------------------
    // TILT ALERT
    // -------------------------------------------------------------

    if (abnormalTilt)
    {
      Serial.println(
        "[ALERT] Abnormal vessel inclination"
      );
    }


    // -------------------------------------------------------------
    // LOW BATTERY ALERT
    // -------------------------------------------------------------

    if (lowBattery)
    {
      Serial.println(
        "[ALERT] Low battery voltage"
      );
    }


    // -------------------------------------------------------------
    // EMERGENCY BUTTON ALERT
    // -------------------------------------------------------------

    if (emergencyPressed)
    {
      Serial.println(
        "[ALERT] Emergency button activated"
      );
    }


    // -------------------------------------------------------------
    // OBSTACLE ALERT
    // -------------------------------------------------------------

    if (nearbyObstacle)
    {
      Serial.print(
        "[ALERT] Nearby object at "
      );

      Serial.print(
        nearestObjectDistance,
        1
      );

      Serial.print(
        " cm, bearing approximately "
      );

      Serial.print(
        nearestObjectAngle
      );

      Serial.println(
        " degrees"
      );
    }


    // -------------------------------------------------------------
    // ACTIVATE WARNING OUTPUTS
    // -------------------------------------------------------------

    activateAlarm();
  }


  // ===============================================================
  // SAFE CONDITION
  // ===============================================================

  else
  {
    Serial.println();

    Serial.println(
      "STATUS: All monitored conditions normal."
    );

    deactivateAlarm();
  }


  // ===============================================================
  // END OF MONITORING CYCLE
  // ===============================================================

  Serial.println();

  Serial.println(
    "Monitoring cycle completed."
  );

  Serial.println(
    "Preparing next cycle..."
  );

  Serial.println();


  /*
    Delay before the next complete monitoring cycle.

    The ultrasonic scan itself also requires some time
    because the servo moves through multiple positions.
  */

  delay(
    2000
  );
}
