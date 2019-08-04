/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini

   https://github.com/nkolban/ESP32_BLE_Arduino/tree/master/examples
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <sstream>

//originally filtering by name, but name is unreliably reported by the BLEScan library
const std::string DEVICE_PREFIX = "Temp-Humidity";
//UUID for main service on Jinou BLE remperature sensors
const BLEUUID temp_humidity_service = BLEUUID("0000aa20-0000-1000-8000-00805f9b34fb");

int scanTime = 8; //In seconds
BLEScan* pBLEScan;

const bool DEBUG = true;
const bool TRACK_ALL = false;
int sensorsFound = 0;


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    std::string getManufacturerDataHexString(BLEAdvertisedDevice advertisedDevice){
        std::stringstream ss;
        char *pHex = BLEUtils::buildHexData(nullptr, 
                                            (uint8_t*)advertisedDevice.getManufacturerData().data(), 
                                            advertisedDevice.getManufacturerData().length());
        ss << pHex;
        free(pHex);
        return ss.str();
    }

    int twoHexCharsToInteger(char sixteens, char ones) {
        int decimal = hexCharToInt(sixteens);
        decimal = decimal * 16;
        int ones_int = ones - '0';
        ones_int = hexCharToInt(ones);
        decimal += ones_int;
        return decimal;
    }
    int hexCharToInt(char hex){
      int val = hex - '0';
      if (hex > '9') val -= 39;
      return val;
    }
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (DEBUG) Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());     

      if (advertisedDevice.isAdvertisingService(temp_humidity_service)) {
        sensorsFound += 1;
        if (DEBUG) Serial.printf("** %s: %s\n", DEVICE_PREFIX.c_str(), advertisedDevice.toString().c_str());
        Serial.printf("%s - %s\n", DEVICE_PREFIX.c_str(), advertisedDevice.getAddress().toString().c_str());

        // currently dealing with this as a string 
        std::string str_data = getManufacturerDataHexString(advertisedDevice);
        if (DEBUG) Serial.printf("** data: %s \n", str_data.c_str());
        
        int decimal_temp = twoHexCharsToInteger(str_data.at(2), str_data.at(3));
        int decimal_sub_temp = twoHexCharsToInteger(str_data.at(4), str_data.at(5));
        Serial.printf("-  %d.%d°C\n", decimal_temp, decimal_sub_temp);
        int decimal_hum = twoHexCharsToInteger(str_data.at(8), str_data.at(9));
        int decimal_sub_hum = twoHexCharsToInteger(str_data.at(10), str_data.at(11));
        Serial.printf("-  %d.%d%% humidity\n", decimal_hum, decimal_sub_hum);
        
        int decimal_battery = twoHexCharsToInteger(str_data.at(12), str_data.at(13));
        //the battery power measurement is not reliable and often comes out >100%
        // the below adjustment is a bit weird, but it's closer to reality than the raw value is
        if (decimal_battery>100) decimal_battery = 99;
        Serial.printf("-  %d%% battery\n", decimal_battery);        
      } else {
        if (TRACK_ALL) Serial.printf("   Boring device: %s \n", advertisedDevice.toString().c_str());
      }
    }
};


void setup() {
  Serial.begin(115200);
  delay(300);
  initBle();
}
void initBle(){
  Serial.println("Initialising scan...");
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
  pBLEScan->setInterval(260);
  pBLEScan->setWindow(220);  // less or equal setInterval value
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("New Scan");
  sensorsFound = 0;
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Sensors seen: ");
  Serial.println(sensorsFound);
  Serial.print("Total devices seen: ");
  Serial.println(foundDevices.getCount());
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  //use random delay to reduce risk of missing sepcific devices
  delay( random(100, 3000));

}
