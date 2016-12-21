#include <SPI.h>
#include <BLEPeripheral.h>

#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif


#define LED_PIN LED_BUILTIN
#define BUTTON_PIN 22

// Values pulled from BluefruitConfig.h
//#define BLUEFRUIT_SPI_CS              8
#define BLE_REQ                         8

//#define BLUEFRUIT_SPI_IRQ             7
#define BLE_RDY                         7

//#define BLUEFRUIT_SPI_RST             4    // Optional but recommended, set to -1 if unused
#define BLE_RST                         4

// Global variables
int currentState;
int debounceState;
int switchState = 0;
int ledState = 0;

BLEPeripheral blePeripheral = BLEPeripheral(BLE_REQ, BLE_RDY, BLE_RST);
BLEService lightSwitch = BLEService("FF10");

BLECharCharacteristic switchCharacteristic = BLECharCharacteristic("FF11", BLERead | BLEWrite);
BLEDescriptor switchDescriptor = BLEDescriptor("2901","Switch");

BLECharCharacteristic stateCharacteristic = BLECharCharacteristic("FF12", BLENotify);
BLEDescriptor stateDescriptor = BLEDescriptor("2901","State");


void setup() {
  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("BlueBox Example Setup"));
  Serial.println(F("-------------------------------------"));

  Serial.println(F("Setting LED and BUTTON PINs"));
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  Serial.println(F("Setting Local Name"));
  blePeripheral.setLocalName("Light Switch");
  Serial.println(F("Setting Device Name"));
  blePeripheral.setDeviceName("Smart Light Switch");
  Serial.println(F("Setting UUID"));
  blePeripheral.setAdvertisedServiceUuid(lightSwitch.uuid());

  Serial.println(F("Adding Attributes"));
  blePeripheral.addAttribute(lightSwitch);
  blePeripheral.addAttribute(switchCharacteristic);
  blePeripheral.addAttribute(switchDescriptor);
  blePeripheral.addAttribute(stateCharacteristic);
  blePeripheral.addAttribute(stateDescriptor);

  Serial.println(F("Setting Event Handler"));
  //switchCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);
  Serial.println(F("Starting Bluetooth"));
  //blePeripheral.begin();

  Serial.println(F("BlueBox Example Setup Complete"));
  Serial.println(F("-------------------------------------"));
  Serial.println("Smart Light Switch");
}

void loop() {
  blePeripheral.poll();
  currentState = digitalRead(BUTTON_PIN);
  delay(10);
  debounceState = digitalRead(BUTTON_PIN);

  if( currentState == debounceState ){
    if(currentState != switchState ) {
      if( currentState == LOW ) {

        // Button Just Released
      } else {
        Serial.print(F("Button Event: "));
        if( ledState == 0 ){
          stateCharacteristic.setValue(1);
          switchCharacteristic.setValue(1);
          digitalWrite(LED_PIN, HIGH);
          ledState = 1;
          Serial.println(F("light on"));
        } else {
          stateCharacteristic.setValue(0);
          switchCharacteristic.setValue(0);
          digitalWrite(LED_PIN, LOW);
          ledState = 0;
          Serial.println(F("light off"));
        }
      }
      switchState = currentState;
    }
  }
}

void switchCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic){
  Serial.print(F("Characteristic Event: "));
  if( switchCharacteristic.value() ){
    Serial.println(F("light on"));
    digitalWrite(LED_PIN, HIGH);
    ledState = 1;
    stateCharacteristic.setValue(1);
  } else {
    Serial.println(F("light of"));
    digitalWrite(LED_PIN, LOW);
    ledState = 0;
    stateCharacteristic.setValue(0);
  }
}
