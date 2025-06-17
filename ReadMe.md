# Intelligent Bike Alarm System

Melanie Kukura
Final Project for:
ECE 554 - Embedded Systems
Professor Shaout
University of Michigan - Dearborn

This is the code base for an intelligent bike alarm system developed as a final project for an embedded systems course. The bike alarm will have the following functions:

* motion_detect: sets alarm triggered when armed and motion detected
* activate_buzzer: Triggers audible sound from buzzer
* bt_alert: Sends bluetooth alert to smartphone when alarm triggered
* rfid: Arm/disarm system when RFID scan detected
* sleep_check: checks if system should go into sleep mode

This system is implemented in bare-metal C with a custom polled loop RTOS and interrupt servicing for motion detected by the MPU6050 motion sensor module. The MFRC522 RFID module is used for RFID reading and an HM-10 Bluetooth Low Power module is used to send an alert to a smartphone when the alarm is triggered. An active buzzer is used to create an auditory beep for the alarm.
