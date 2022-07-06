# BLE_Privacy_Beacon
Implementation of a BLE device implementing information and consent mechanisms in the context of an IoT system collecting personnal information.
## settings
- the arduino should be updated to its latest version (the version used for the demo is 1.8.19)
- some additional libraries must be installed: arduinojson (latest 5.x), manual install of BLECarrousel
- you have to install esp32 boards (using https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json in settings additional boards manager urls), in our case we use the ESP32C3 dev module
- other settings must be left by default, but be careful about the port (/dev/ttyUSBx): one may require sudo privileges
- the code needs to be compiled (Ctrl+R), then uploaded to the device (Ctrl+U)
## structure
- setup() is the main function, in which we define the ADPC notice and various required functions
- MyConsentCallBack is for the GATT service, the function onWrite displays the consents retrieved in the terminal
- other functions are not to be touched
## limitations/known issues
- in the code, string s1 needs to escape with \ when using "", no newline (otherwise character encoding issue)
- the size of the notice communicated is hardcoded
- upload may fail due to corruption of the data communicated, simply try again in that case
- the mobile app may not disconnect properly, in that case simply reboot the device
