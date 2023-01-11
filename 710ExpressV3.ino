////// 710 EXPRESS Ver 3 - TEST /////
////// TEMPLATE 1/11/2023 ///////////

// INCLUDES
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

const int Led_Red = 19;
const int Led_Green = 18;
int val = 0;
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "b3e179c6-e28b-43f3-bd3f-96f0611e0c89"
#define CHARACTERISTIC_UUID     "dce6732b-1ca2-49ea-95bb-46aa026ace4a"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting 710Express work!");
  pinMode(Led_Red, OUTPUT); 
  pinMode(Led_Green, OUTPUT);
  Serial.println("710Express LED Choo Choo-ing");


  // Create the BLE Device
  BLEDevice::init("710EXPRESS");
  Serial.println("710Express BLE DEVICE CREATED");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  Serial.println("710Express BLE SERVER CREATED");

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  Serial.println("710Express BLE SERVICE CREATED");

  // Create a BLE Characteristic for temperature
  pCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE );
  Serial.println("710Express BLE CHARACTERISTIC CREATED");

  
  pCharacteristic->setValue("Current Temp: N/A");
  Serial.println("710Express BLE CHARACTERISTIC DEFAULT VALUE SET");

  // Start the service
  pService->start();
  Serial.println("710Express BLE SERVER STARTED");

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting for a Toker to Connect!!");
}

void loop() {
    // CONNECTED DO THIS
    if (deviceConnected) 
    {
      for (int t = 420; t > 0; t++)
      {
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print("*F");
        Serial.print("\n");

        char buffer [30];
        sprintf(buffer, "THE TEMP IS AT %d", t);
        Serial.print(buffer);
        Serial.print("\n");

        pCharacteristic->setValue(buffer);
        pCharacteristic->notify();

        analogWrite(Led_Red, 127+val);  /// Yellow
        delay(1000);
        analogWrite(Led_Red, val);   //GREEN
        delay(1000);
        analogWrite(Led_Red, 127+val);  /// Yellow

        delay(2000); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms

        if(!deviceConnected)
        {
          break;
        }

      }

    }

    // IF NO CONNECTION DO THIS
    if (!deviceConnected)
    {
      analogWrite(Led_Red, 255-val); /// RED
      delay(1000);
      analogWrite(Led_Red, 127+val);  /// Yellow
      delay(1000);
      analogWrite(Led_Red, 255-val); /// RED
      delay(1000);

      Serial.print("NO CONNECTION CURRENTLY");
      Serial.print("\n");      

    }

    // IF LOSS CONNECTIONS DO THIS
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}