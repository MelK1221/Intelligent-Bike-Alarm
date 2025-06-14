# Intelligent Bike Alarm System

Melanie Kukura
Final Project for:
ECE 554 - Embedded Systems
Professor Shaout
University of Michigan - Dearborn

This is the code base for an intelligent bike alarm system developed as a final project for an embedded systems course. The bike alarm will have the following functions:

* Beep_Audio: Triggers audible sound from buzzer
* BT_Alert: Sends bluetooth alert to smartphone when alarm triggered
* Motion_Detect: Polls the accelerometer/gyroscope (MPU6050) to detect motion of bike
* RFID: Arm/disarm system when RFID scan interrupt detected

In addition, the following drivers are required to utilize modules added to the microcontroller:
* Buzzer: Play audio tones from Piezo buzzer
* HM10: Send bluetooth alert to smartphone
* MFRC522: Detect and read RFID card
* MPU6050: Read motion data
* Timer: Interrupt timer for task scheduling

There is also a sleep function that puts the system in power-saving mode when no other tasks are scheduled.