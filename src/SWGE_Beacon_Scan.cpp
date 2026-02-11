#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

// Counters and Timers
//#define CHANGEDELY 15*1000  // 15 second Delay
#define CHANGEDELY 5*1000   // 5 second Delay

// Location IDs (in Int)
#define NOBEACON      0
#define MARKETPLACE   1
#define DROIDDEPOT    2
#define RESISTANCE    3
#define UNKNOWN       4
#define ALERT         5
#define DOKONDARS     6
#define FIRSTORDER    7

// Filters
#define RSSI -75
#define BLE_DISNEY 0x0183
const String IGNOREHOST = "SITH-TLBX";

// Timing
uint32_t last_activity;

// Scan Values to store
int8_t scan_rssi;
uint16_t area_num = 0, last_area_num = 9;
String beacon_name = "";

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("SWGE_Beacon_Scan");
  Serial.println("Scanning...");
  Serial.println("----------------\n");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(10000);
}