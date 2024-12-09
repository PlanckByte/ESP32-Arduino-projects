# ESP32-Arduino-projects
## Disclaimer
The sketches in this repository is shared for **testing**, **experimentation**, and **educational purposes only**. It reflects exploratory work conducted during my time at **CommonsLab** and in personal projects.

### Usage Note:
- These scripts are not guaranteed to be free of bugs or suitable for production-level applications.
- The primary purpose of sharing this repository is to inspire learning, troubleshooting, and experimentation with Arduino/ESP32 and various sensor integrations.


### Overview

This repository contains a collection of Arduino-based projects developed during my time at CommonsLab, a Social Cooperative Enterprise dedicated to IoT and automation solutions. Many of these projects were created for experimental purposes, troubleshooting, or testing various sensors and configurations especially on potential mini games. Others, such as the Weather Station, were deployed to be tested in practical applications, including the Valdibella ATTESTED pilot under the EU-funded DRG4FOOD Open Call.

## 1.) Velostat Pressure Sensor

A **Velostat Pressure Sensor** is a simple DIY sensor made using a piece of **velostat** (a pressure-sensitive conductive sheet) and **copper wire** or **solder wick**. The sensor works by taking advantage of the velostat's property to change its resistance when pressure is applied.

## Materials
1. Velostat sheet
2. Copper sheet or solder wick
3. Resistor ( in the range of KOhms I do not remember the exact value. Start at 10K and lower if needed)
4. Arduino Uno
5. Breadboard and jumper wires

### Construction:
1. Cut a piece of **velostat** sheet to the desired size.
2. Place **copper wires** (or copper sheets,solder wick) on opposite edges of the velostat. These act as the conductive contacts.
3. Secure the wires to the velostat using adhesive or by carefully applying heat.
4. Solder  jumper wires to the edges of the copper contacts to connect the sensor to a circuit.
5. Through a voltage divider feed the input from the one cable to an analog pin and the other to GND.
6. Upload code and play.

When pressure is applied to the velostat, its resistance decreases, allowing it to be used in pressure-sensing circuits. Ideal for DIY projects like pressure pads, flexible sensors, or responsive input devices.


## 2.) Light Dependent Resistor Laser Beeper.

Used as a sensor for a laser room game prototype. Kinda like mission impossible. We used 5mW green lasers to test. You can change thresholds when using different light sources

## Materials
1. A light depended resistor
2. Arduino Uno
3. Through hole prototyping board
4. Resistor (10K) for voltage divider to be used with LDR.
5. A buzzer module.

### Construction:
1. Connect to GND and Vcc the buzzer module and the I/O pin to pin 5.
2. Connect LDR with voltage divider as shown here : https://storage.googleapis.com/medium-feed.appspot.com/images%2F9353691196%2Fcb83db1ba8951-1718596792_520_Alarme-de-campainha-piezo-Arduino-com-LDR-resistor-dependente-de.jpg
3. Upload code and play.
