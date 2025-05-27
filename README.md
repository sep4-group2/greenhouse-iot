# SmartGrow IoT

This project is a 4th semester IoT system called SmartGrow for greenhouse management and automation. It enables monitoring and control of environmental variables such as temperature, humidity, light, irrigation, fertilization, and more, using sensors and actuators connected to a VIA Shield board.

## Main Features
- Temperature and humidity monitoring
- Light and lightbulb control
- Irrigation and water pump management
- Servo and actuator control
- Automated fertilization
- Notifications and alerts
- Communication via MQTT and UART
- Periodic tasks and timing

## Project Structure
- `src/`: Main source code (`main.c`)
- `lib/`: Libraries and system modules
  - `actions/`: Control logic and actions
  - `clock/`, `timestamp/`: Time and timer management
  - `display/`: Display control
  - `fertilizer/`: Fertilization logic
  - `leds/`, `light/`, `lightbulb/`: Lighting control
  - `mqtt/`, `mqtt_buffer/`, `mqtt_received_publish/`, `mqtt_subscribe_topics/`, `MQTTPacket/`: MQTT communication
  - `notification/`: Notifications and alerts
  - `periodic_task/`: Periodic tasks
  - `preset/`: Configuration presets
  - `pump/`: Water pump control
  - `servo/`: Servo control
  - `soil/`: Soil sensors
  - `temperature/`: Temperature and humidity sensors
  - `uart/`, `uart_buffer/`, `uart_packet/`: UART communication
  - `wifi/`: WiFi communication
- `test/`: Unit and integration tests
- `doc/`: Documentation and diagrams

## How to Build and Run
1. Install PlatformIO in your development environment (VS Code recommended).
2. Clone this repository.
3. Open the project folder in VS Code.
4. Connect your Arduino board to your computer via USB.
5. Build and upload the firmware to the Arduino using PlatformIO:
   ```powershell
   pio run --target upload
   ```
6. The system will start running automatically after upload. You can monitor serial output using:
   ```powershell
   pio device monitor
   ```

## Testing
Tests are located in the `test/` folder. Run them with PlatformIO:
```powershell
pio test
```

## Credits
Developed by Group 2X: IoT team.

Maria Yepez 
Dominik Kielbowski 
Joan Hageneier 
Sebastian Villarroel
