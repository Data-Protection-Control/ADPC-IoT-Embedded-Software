/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE_Carousel.h>
#include <iostream>
#include <algorithm>
#include <ArduinoJson.h>
using namespace std;

#define PACKET_SIZE 24
#define HEADER_SIZE 3 // Object ID 1 byte, Object size 1 byte  Packet Number 1 byte

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // UUID dedicated to consent transmission
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" // Characterisitic that will receive the consent
int idx = 0;
BLEAdvertising *pAdvertising;
BLE_Carousel carousel = BLE_Carousel(PACKET_SIZE,HEADER_SIZE);

// Define a callback to handle consent reception
// (ie when the corresponding characteristic is modify by a write)
//  found here : http://bbs.esp32.com/viewtopic.php?f=19&t=4181

// for the GATT service as well
class MyConsentCallBack: public BLECharacteristicCallbacks {


// displays the consents retrieved in the terminal
void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
     if (rxValue.length() > 0) {
      Serial.println("*********");
      Serial.println("Received a new consent: ");
      Serial.print("Length:");
      Serial.println(rxValue.length());
      Serial.print("Value: ");
      for (int i = 0; i < rxValue.length(); i++)
        Serial.print(rxValue[i]);
      Serial.println();
      Serial.println("*********");
      // Insert Consent Parser here
    }
}
    
};


// main function, in which we define the ADPC notice and various required functions
void setup() {
  // Upload speed in baud, used to display on the terminal
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("InCon_Beacon");
  BLEServer *pServer = BLEDevice::createServer();
  
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  // Configure the characteristic        
  // The maximum length of an attribute value shall be 512 octets.
  pCharacteristic->setValue("Consent");
  pCharacteristic->setCallbacks(new MyConsentCallBack());
  Serial.println("Characteristic defined!");
  Serial.println("Ready to receive consents!");

  
  // Allocate the buffer that will contain the ADPC notice
  String apdc_iot_notice = "[{ \"id\": \"q1analytics\", \"text\": \"We track and analyse your visit(s) in this mall, for improving our products.\" },{ \"id\": \"q2recommendation\", \"text\": \"We observe your interaction with our content to personalise your experience by recommending content you may find of interest.\"}]";
  // Escape quotes are important, and so is the fact that string is allocated in one line
  int pp_size = 400;  
  // Here buffer size is guessed, be wary!        
  byte pp[pp_size] = {};
  apdc_iot_notice.getBytes(pp, 400);

  // Initialize the Carousel
  carousel.set_Data(pp,pp_size);
  
  // Configure the payload of advertising packets
  byte*  packet = (byte*)carousel.get_Packet( 0);  
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
  
  std::string strServiceData = "";
  
  strServiceData += (char)11;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += "0123456789";
  oAdvertisementData.addData(strServiceData);

  
   pAdvertising = pServer->getAdvertising();
  //pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
  set_new_adv_payload(idx);

  // Start service and advertising
  pService->start();
  pAdvertising->start();

}


void set_new_adv_payload(int idx){
  //Serial.println(idx);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
  std::string strServiceData = "";
  //Serial.println(carousel.get_Nb_Packets());
  strServiceData += (char)(PACKET_SIZE+1);     // Len
  strServiceData += (char)0xFF;   // Type

   byte*  packet = (byte*)carousel.get_Packet( idx);
  for(int i=0; i < PACKET_SIZE; i++){
    strServiceData += (char) packet[i];
  }
  free(packet);
  //Serial.println(idx);
  oAdvertisementData.addData(strServiceData); 
  pAdvertising->setAdvertisementData(oAdvertisementData);
}

void loop() {
  delay(200);
  //Serial.println(idx);

  set_new_adv_payload(idx);
  
  idx = (idx + 1) % carousel.get_Nb_Packets();
}
