# esp32-arduino-ble-scan
BLE scanning using Arduino environment on an ESP32, initially set up for Jinou temperature sensors

## Sensor device info

Example manufacturer data from 2 Jinou JO-0628 BLE temperature/humidity sensors:
`001502002e045afe510aaad1aa`
`001508002d0261e50cd530712f`
Breaks down into:
`00 1508 00 2d02 61 e50cd530712f`
`00 [1508] 00 [2d02] [61] [e50cd530712f]`
* Temperature `[1508]`
  * 2-part hex 
    * [15] = decimal 21 (whole degrees)
    * [08] = decimal 8 (tenths of a degree)
    * 21.8°C
* Humidity `[2d02]`
  * 2-part hex 
    * [2d] = decimal 45 (whole percent)
    * [02] = decimal 2 (tenths of a percent)
    * 45.2%
* Battery `[61]` 
  * hex 
    * [61] = decimal 97 (whole percent)
    * 97%
* MAC address `[e50cd530712f]`



## TODO
* add wifi manager (or blufi) for easy setup
* add some config and control
  * destination url
  * interval, window, duration and delay between scans
  * maybe have configurable service ID string to support other devices (doesn't make much sense if you need to change the code to parse the services)
  * debug output (to be streamed like results)
  * full tracking (to record all devices seen)
* stream results over MQTT or HTTP
  * consider including total device count as a signal (possibly of room occupancy levels)