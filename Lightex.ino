#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>
#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))
#define uS_TO_S_FACTOR 1000000
#define ON  HIGH
#define OFF LOW


//adjustable parameters
char UUID[] = "23311223-3112-2331-1223-311223233112"; // iBeacon ID
int trigger = 1500; // when the photoresistor flips the relay; 0 = complete darkness, 4095 = max luminosity
int active_scan = false; // active scan uses more power, but get results faster
int scanTime = 3; // In seconds
int delayAfterImGone = 60000; // 1min
int interval_heartbeat = 2000; // 2sec; beacon signal checkup (when in range or not)
int interval_deep_sleep = 900; // 15min; in seconds; saves power

//program parameters (don't touch these)
const int pin_photoresistor = 32;
const int pin_relay = 33;
const int pin_led = 2;
int BLE_interval = 100;
bool wasInRange = false;
int lightVal;
bool inRange;

BLEScan *pBLEScan;


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks{
    void onResult(BLEAdvertisedDevice advertisedDevice){
      if (advertisedDevice.haveName()){
        Serial.print("[!] Device name: ");
        Serial.println(advertisedDevice.getName().c_str());
        Serial.println("");
      }

      if (!advertisedDevice.haveServiceUUID()){ // if beacon
        if (advertisedDevice.haveManufacturerData() == true){ // if identifiable beacon
          std::string strManufacturerData = advertisedDevice.getManufacturerData();
          uint8_t cManufacturerData[100];
          strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

          if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00){ // if iBeacon
            BLEBeacon oBeacon = BLEBeacon();
            oBeacon.setData(strManufacturerData);

            if(oBeacon.getProximityUUID().toString() == UUID){ // our iBeacon
              Serial.println("[!] +++++++++++++++++++++++++++++++++++++++++++++++UUID FOUND++++++++++++++++++++++++++++++++++++++++++++++++");
              Serial.printf("[!] UUID: %s Major: %d Minor: %d ID: %04X Power: %d RSSI: %d\n", oBeacon.getProximityUUID().toString().c_str(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor()),oBeacon.getManufacturerId(), oBeacon.getSignalPower(),(int)advertisedDevice.getRSSI());
              Serial.printf("[!] %s\n",advertisedDevice.toString().c_str() );
              Serial.println("");
              inRange = true;
            }else{ // some other iBeacon
              Serial.printf("[!] UUID: %s Major: %d Minor: %d ID: %04X Power: %d RSSI: %d\n", oBeacon.getProximityUUID().toString().c_str(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor()),oBeacon.getManufacturerId(), oBeacon.getSignalPower(),(int)advertisedDevice.getRSSI());
              Serial.printf("[!] %s\n",advertisedDevice.toString().c_str() );
              Serial.println("");
            }
          }
        }
        return;
      }
    }
};

bool beaconInRange(){
  Serial.println("[*] Searching for iBeacons...\n");
  inRange = false;
  digitalWrite(pin_led, HIGH);
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  digitalWrite(pin_led, LOW);
  Serial.print("[?] iBeacon in range: ");
  Serial.println(inRange ? "Yes" : "No");
  return inRange;
}

bool isNight(){
  lightVal = analogRead(pin_photoresistor);
  Serial.printf("[?] Is night: [%d] ",(int)lightVal);

  if(lightVal < trigger){
    Serial.println("Yes");
    return true;  // if night
  }else{
    Serial.println("No");
    return false;
  }
}

void turnRelay(int x){digitalWrite(pin_relay, x);}


void setup()
{
  Serial.begin(115200);
  pinMode(pin_relay, OUTPUT);
  pinMode(pin_led, OUTPUT);
  esp_sleep_enable_timer_wakeup(interval_deep_sleep * uS_TO_S_FACTOR);
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(active_scan); //active scan uses more power, but get results faster
  pBLEScan->setInterval(BLE_interval);
  pBLEScan->setWindow(BLE_interval-1); // less or equal setInterval value
}

void loop() // main
{
  if( isNight() ){
    if( beaconInRange() ){ // expensive operation, heats up the MCU
      
      if( wasInRange ){ // keep ON
        delay(interval_heartbeat);
        
      }else{ // wasn't in range; turn ON
        turnRelay(ON);
        wasInRange = true;
      }
      
    }else{ // not in range
      if ( wasInRange ){
        wasInRange = false;
        Serial.println("[*] Waiting for delayAfterImGone...");
        delay(delayAfterImGone);
        turnRelay(OFF);
        
      }else{ // wasn't in range
        delay(interval_heartbeat);
      }
    }
  }else{ // if not night; go to deep sleep (saves power)
    Serial.println("[*] Going in to deep sleep...");
    delay(1000);
    Serial.flush(); 
    esp_deep_sleep_start();
  } 
}
