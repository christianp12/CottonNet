# IoT Project

## Description
IoT system that includes various components for managing and monitoring plant irrigation. It includes sensors, actuators, and remote management applications.

## Project Components
- **Illumination**: Manages lighting.
- **Plant Sensor**: Monitors plant conditions.
- **Registration Server**: Server for device registration.
- **Remote Control Application**: Application for remote control.
- **Soil Sensor**: Monitors soil moisture.
- **Sprinkler**: Controls the sprinklers.

## Repository Structure
- `illumination/`: Code and configurations for managing lighting.
- `plant_sensor/`: Code for plant sensors.
- `registration-server/`: Code for the device registration server.
- `remote-control-application/`: Code for the remote control application.
- `soil_sensor/`: Code for soil sensors.
- `sprinkler/`: Code for sprinkler control.

## Prerequisites
- C Compiler
- Java Environment
- Make

## Installation
1. Clone the repository:
    ```bash
    git clone https://github.com/christianp12/CottonNet.git
    ```
2. Navigate to the project directory:
    ```bash
    cd CottonNet
    ```
3. Start the registration server
4. Start the nodes (and border router) on Contiki environment 
5. Press the buttons for the sensors and actuators
    ``` it is recommended to register the sensors first
    then start the actuators, although it is possible to press all buttons together
6. Start the remote control application to use its features
7. Once finished, it is recommended to shut down the sensors using the appropriate command in the remote application


