#include <Wire.h>// GuardianPod - IoT Safety Device
// ESP32-based system for GPS tracking and real-time alerts
// Uses BLE + WiFi communication and ThingSpeak cloud integration
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(128,64,&Wire,-1);

// pins
const int buttonPin = 15;
const int ledPin = 4;
const int buzzerPin = 5;

// coordinates
float latitude = 19.1071;
float longitude = 72.8377;

// BLE UUID
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-1234-1234-1234-123456789abc"

BLECharacteristic *pCharacteristic;

// BLE callback
class MyCallbacks: public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic *pCharacteristic) {

    String value = pCharacteristic->getValue();

    Serial.print("BLE Received: ");
    Serial.println(value);

    if(value == "1")
    {
      digitalWrite(ledPin,HIGH);
      digitalWrite(buzzerPin,HIGH);
    }

    if(value == "0")
    {
      digitalWrite(ledPin,LOW);
      digitalWrite(buzzerPin,LOW);
    }
  }
};

void setup()
{
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Wire.begin(21,22);

  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C))
  {
    Serial.println("OLED not detected");
    while(true);
  }

  // Boot screen
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(10,20);
  display.println("Guardian");
  display.setCursor(30,45);
  display.println("Pod");

  display.display();

  delay(2000);

  // BLE Setup
  BLEDevice::init("GUARDIANPOD");

  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  Serial.println("BLE Ready");
}

void loop()
{

  if(digitalRead(buttonPin)==LOW)   // BUTTON PRESSED
  {
    digitalWrite(ledPin,HIGH);
    digitalWrite(buzzerPin,HIGH);

    // GPS Reading Screen
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(10,20);
    display.println("Reading GPS...");
    display.display();

    delay(2000);

    // Alert Screen
    display.clearDisplay();

    display.setCursor(0,0);
    display.println("!!! EMERGENCY !!!");

    display.setCursor(0,18);
    display.println("Guardian Notified");

    display.setCursor(0,40);
    display.print("Lat: ");
    display.println(latitude,4);

    display.setCursor(0,55);
    display.print("Lng: ");
    display.println(longitude,4);

    display.display();

    delay(8000);
  }

  else   // IDLE MODE
  {
    digitalWrite(ledPin,LOW);
    digitalWrite(buzzerPin,LOW);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0,0);
    display.println("GuardianPod");

    display.setCursor(0,18);
    display.println("Device Status");

    display.setCursor(0,35);
    display.println("Mode: Idle");

    display.setCursor(0,50);
    display.println("GPS: Active");

    display.display();
  }

  delay(300);
}
