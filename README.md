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
