# Arduino-Based Naval Safety Monitoring and Obstacle Detection System

An Arduino-based multi-sensor embedded system developed for environmental monitoring, vessel-safety assessment, obstacle detection, emergency warning, and location tracking in a naval or marine environment.

This project was originally developed in **2023** as part of **ECE 2200: Electronic Shop Practice** in the Department of Electrical & Computer Engineering at **Rajshahi University of Engineering & Technology (RUET)**.

The system integrates multiple environmental and vessel-condition sensors with an Arduino controller. It also includes a servo-controlled ultrasonic scanning mechanism for detecting nearby obstacles and estimating their approximate local direction.

> **Implementation Note:**  
> The original source code developed during the 2023 course project is no longer available. The source code currently included in this repository is a reconstructed implementation based on the original project concept, remembered functionality, and intended system architecture. The exact original circuit configuration and component models are not claimed unless explicitly documented.

---

## Project Objectives

The project was designed to:

- Develop an Arduino-based safety-monitoring system for a marine or naval environment.
- Integrate multiple analog and digital sensors into a single embedded platform.
- Continuously monitor environmental and vessel-related operating conditions.
- Detect potentially hazardous conditions using threshold-based decision logic.
- Generate automatic audible and visual warnings during abnormal conditions.
- Detect nearby obstacles using ultrasonic ranging.
- Estimate the approximate angular position of nearby obstacles using servo-based scanning.
- Monitor vessel geographical position using GPS.
- Gain practical experience in embedded programming, sensor interfacing, and hardware-software integration.

---

## System Overview

The system consists of an Arduino controller connected to several environmental, safety, navigation, and obstacle-detection sensors.

The Arduino continuously collects measurements from the connected sensors and evaluates them against predefined safety thresholds.

If any monitored condition becomes unsafe, the system activates:

- A warning LED
- An audible buzzer
- A corresponding alert message in the Serial Monitor

When all monitored conditions remain within the configured limits, the system indicates a normal or safe operating state.

The ultrasonic scanning unit rotates through multiple angular positions and measures the distance to surrounding objects. The system identifies the nearest detected object and reports its approximate local angular position.

GPS functionality can additionally provide:

- Latitude
- Longitude
- Movement speed

---

## System Architecture

```text
                    NAVAL / MARINE ENVIRONMENT
                               |
          +--------------------+--------------------+
          |                    |                    |
          v                    v                    v
   Environmental          Vessel Safety        Navigation /
      Sensors                Sensors          Obstacle Sensors
          |                    |                    |
          |                    |             +------+------+
          |                    |             |             |
          |                    |         Ultrasonic       GPS
          |                    |          Scanner        Module
          |                    |             |             |
          +--------------------+-------------+-------------+
                               |
                               v
                       Arduino Controller
                               |
                     Sensor Data Processing
                               |
                     Safety Decision Logic
                               |
              +----------------+----------------+
              |                |                |
              v                v                v
            Buzzer        Warning LEDs     Serial Monitor
```
# Monitored Conditions

The system monitors multiple environmental, vessel-safety, power, emergency, and navigation-related conditions through an Arduino-based embedded platform.

---

## 1. Temperature Monitoring

An analog temperature sensor is used to monitor the surrounding environmental temperature.

The Arduino reads the sensor output and converts it into a temperature value for continuous safety assessment.

---

## 2. Gas / Smoke Monitoring

A gas or smoke sensor is used to monitor changes in surrounding air conditions.

The sensor output is processed by the Arduino and compared with a predefined safety threshold to identify potentially hazardous gas or smoke conditions.

---

## 3. Water / Bilge-Level Monitoring

A water-level sensor is used to monitor possible water accumulation or water ingress inside the vessel.

The measured level is continuously evaluated to detect unsafe water conditions.

---

## 4. Flame / Fire Detection

A flame sensor is incorporated into the system to detect possible fire-related hazards.

When a flame condition is detected, the Arduino immediately includes it in the overall safety evaluation and activates the warning system.

---

## 5. Vessel Tilt / Inclination Monitoring

A tilt sensor is used to detect abnormal inclination of the monitored platform.

This provides a basic method of identifying potentially unsafe vessel orientation or instability.

---

## 6. Battery-Voltage Monitoring

Battery voltage is monitored through an Arduino analog input using a voltage-divider circuit.

The measured voltage is continuously evaluated to detect low-power conditions that may affect system operation.

---

## 7. Emergency Push Button

A manual emergency push button provides an additional safety input.

When activated, the button immediately triggers the system's emergency-warning state.

---

## 8. Nearby Obstacle Detection

A servo-mounted ultrasonic sensor is used to detect nearby objects.

The system measures object distance at multiple angular positions and determines the closest detected obstacle.

The servo angle also provides an approximate local direction of the detected object.

---

## 9. GPS-Based Location Monitoring

A GPS module is used to obtain geographical position information.

The reconstructed implementation supports monitoring of:

- Latitude
- Longitude
- Movement speed

GPS information complements the ultrasonic obstacle scanner by providing vessel-position data rather than short-range obstacle information.

---

# Ultrasonic Obstacle Scanning

## Overview

The system uses a servo-mounted ultrasonic distance sensor to scan the surrounding area.

Although similar Arduino projects are often referred to as "Arduino radar," this implementation uses ultrasonic sound waves rather than radio-frequency radar signals.

Therefore, the more accurate description is:

**Servo-Controlled Ultrasonic Obstacle Scanner**

---

## Scanning Principle

The ultrasonic sensor is mounted on a servo motor and rotated through multiple angular positions.

At each position:

1. The servo moves to the selected angle.
2. The ultrasonic sensor transmits an acoustic pulse.
3. The pulse travels toward the surrounding environment.
4. Reflected echoes are received from nearby objects.
5. The Arduino measures the echo travel time.
6. Object distance is calculated.
7. The measured distance is associated with the current servo angle.
8. The nearest detected object is identified.

---

## Distance Measurement

Object distance is estimated using ultrasonic time-of-flight measurement.

The transmitted sound pulse travels from the sensor to an object and returns to the receiver. The Arduino uses the measured travel time and the approximate speed of sound to determine the object distance.

---

## Approximate Object Direction

Because the ultrasonic sensor is mounted on a rotating servo, the servo position provides an approximate local angular direction of the detected object.

This direction is relative to the scanner orientation and is not intended to represent a geographic compass bearing.

---

# GPS-Based Vessel Location

The reconstructed implementation supports a GPS module such as the NEO-6M for geographical position monitoring.

The GPS subsystem can provide:

- Latitude
- Longitude
- Movement speed

The GPS and ultrasonic systems perform different functions:

- **Ultrasonic scanner:** Detects nearby objects and their approximate local direction.
- **GPS module:** Provides the geographical position and movement information of the vessel.

GPS data is processed using the TinyGPSPlus Arduino library.

---

# Hardware Used / Reconstructed Hardware Setup

The reconstructed system is designed around the following hardware components.

| Hardware | Purpose |
|---|---|
| Arduino Uno or compatible board | Main embedded controller |
| Analog temperature sensor | Temperature monitoring |
| Gas / smoke sensor | Air-quality and smoke monitoring |
| Water-level sensor | Water or bilge-level monitoring |
| Flame sensor | Fire / flame detection |
| Tilt sensor | Vessel-inclination monitoring |
| HC-SR04 ultrasonic sensor | Nearby-object distance measurement |
| SG90 or compatible servo motor | Rotating ultrasonic scanning platform |
| NEO-6M or compatible GPS module | Geographic location and speed monitoring |
| Voltage-divider resistors | Battery-voltage measurement |
| Emergency push button | Manual emergency input |
| Piezo buzzer | Audible warning |
| Red LED | Hazard indication |
| Green LED | Safe-status indication |
| Breadboard / prototype board | Circuit assembly |
| Jumper wires | Electrical connections |
| Resistors | Supporting circuitry and voltage division |
| Arduino-compatible power supply | System power |

> **Hardware Documentation Note:**  
> The exact original 2023 component list is no longer available. The table above represents the hardware architecture supported by the reconstructed implementation and should not be interpreted as an exact archival list of all original component models.

---

# Arduino Pin Configuration

| Device | Arduino Pin |
|---|---|
| Temperature Sensor | A0 |
| Gas / Smoke Sensor | A1 |
| Water-Level Sensor | A2 |
| Battery Voltage Input | A3 |
| Flame Sensor | D2 |
| Tilt Sensor | D3 |
| Emergency Push Button | D4 |
| Ultrasonic Trigger | D5 |
| Ultrasonic Echo | D6 |
| Servo Motor | D7 |
| Buzzer | D8 |
| Warning LED | D9 |
| Safe LED | D10 |
| GPS RX | D11 |
| GPS TX | D12 |

The pin assignments can be modified directly in the Arduino source code.

---

# Safety Decision Logic

The Arduino continuously evaluates all monitored conditions.

The system enters a hazard state whenever one or more monitored conditions exceed their configured safety limits or an emergency input is detected.

When a hazard is identified, the controller activates the corresponding warning outputs and reports the detected condition through the Serial Monitor.

---

# Alarm Response

The safety-alert system uses:

- Warning LED
- Safe-status LED
- Audible buzzer
- Serial Monitor messages

During a hazardous condition:

- The warning LED is activated.
- The safe-status LED is disabled.
- The buzzer generates an audible warning.
- The detected condition is reported through the Serial Monitor.

During normal operation:

- The warning LED remains off.
- The safe-status LED remains active.
- The buzzer remains off.

---

# Software Requirements

## Arduino IDE

The project can be compiled and uploaded using the Arduino IDE.

---

## Required Libraries

The source code uses:

```cpp
#include <Servo.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
```
Course Information
Course Title: Electronic Shop Practice
Course Code: ECE 2200
Credit: 1.50 Credits
Department: Electrical & Computer Engineering
Institution: Rajshahi University of Engineering & Technology (RUET)
Project Type: Academic Embedded-System Project
Original Project Year: 2023
The project was completed as part of practical coursework emphasizing:
- Electronic-system implementation
- Sensor interfacing
- Embedded programming
- Hardware prototyping
- Circuit integration
- Engineering problem solving
Learning Outcomes
Through this project, practical experience was gained in:
- Designing an Arduino-based embedded monitoring system
- Interfacing multiple analog and digital sensors
- Processing real-time sensor measurements
- Implementing threshold-based safety decisions
- Integrating multiple sensors into a common controller
- Programming servo motors for angular scanning
- Measuring distance using ultrasonic time-of-flight
- Estimating the approximate local direction of nearby objects
- Understanding GPS-based position tracking
- Designing visual and audible warning systems
- Monitoring power-supply conditions
- Developing and debugging embedded C/C++ programs
- Working with serial communication
- Integrating hardware and software components
- Building and testing prototype electronic systems
- Understanding practical limitations of low-cost sensors
Author
Puja Roy Dipa
B.Sc. in Electrical & Computer Engineering
Department of Electrical & Computer Engineering
Rajshahi University of Engineering & Technology (RUET)
Bangladesh
