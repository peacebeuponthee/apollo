# apollo

## Quick quide:
- Install Arduino IDE: https://www.arduino.cc/en/software/
- Once installed, open the app and go to Setting. At the bottom of the Settings tab add the following link to the "Additional boards manager URLs": https://dl.espressif.com/dl/package_esp32_index.json. Press OK to apply
- Go to Tools > Board > Board Manager. Type "esp32" in the searchbar at the top left corner, look for "esp32 by Espressif Systems" > install it
- Go to Tools > Manage Libraries. Type "PubSubClient" in the searchbar at the top left corner, look for "PubSubClient by Nick O'Leary" > install it
- It this point you're good to go for the basic module cuntionality
- Go to Tools > Board > esp32 and select ESP32 Dev Module
- Go to Tools > Events Run On: and select Core 0
- Go to Tools > Arduino Runs On: and select Core 1
- Connect your Test Module and go to Tools > Port and find your module in the list of USB devices
- You're ready to flash your app to the Test Module now. Make sure to add your WiFi SSID/Password and MQTT Broker IP address to the Test Module apps before you flash them to the Modules
- Once flashed you can open a Serial Monitor. Go to Tools > Serial Monitor. Upon Test Board connection you can see the Wifi and MQTT status. Errors will also show up in the Serial Monitor

## To-Do

- Remote Wifi configuration
- Remote MQTT Broker IP and Topic configuration
- Store Wifi and MQTT config in ESP32 flash memory
