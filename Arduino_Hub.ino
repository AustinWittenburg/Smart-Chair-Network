/*

Demonstrates simple RX and TX operation.
Any of the Basic_TX examples can be used as a transmitter.
Please read through 'NRFLite.h' for a description of all the methods available in the library.

Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

*/

#include "SPI.h"
#include "NRFLite.h"

#include <Firebase_Arduino_WiFiNINA.h>
#define FIREBASE_HOST "smartchairnetwork-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "2zABpgV9A9QKB4bHIePu35btAJLcGiNG1IUUzPA4"
#define WIFI_SSID "UI-DeviceNet"
#define WIFI_PASSWORD "UI-DeviceNet"
FirebaseData firebaseData;

String path = "/Weight_data";
String jsonStr;

const static uint8_t RADIO_ID = 0;       // Our radio's id.  The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
    uint32_t FailedTxCount;
    long SensorReading;
};

NRFLite _radio;
RadioPacket _radioData;

void setup() {
    Serial.begin(115200);

    // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
    // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
    // You can run the 'ChannelScanner' example to help select the best channel for your environment.
    // You can assign a different bitrate and channel as shown below.
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)

    int status = WL_IDLE_STATUS;
    while (status != WL_CONNECTED) {
      status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      Serial.println("No WiFi connection");
      delay(300);
    }

    Serial.println("Connected to WiFi");

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
    Firebase.reconnectWiFi(true);

    Serial.println("Connected to Firebase");
    
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }

    Serial.println("Connected to Radio");
}

void loop() {
    while (_radio.hasData()) {
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.
        
        String msg = "Radio ";
        msg += _radioData.FromRadioId;
        msg += ", ";
        msg += _radioData.OnTimeMillis;
        msg += " ms, ";
        msg += _radioData.FailedTxCount;
        msg += " Failed TX";

        if (Firebase.setFloat(firebaseData, path + "/1-setFloat", _radioData.SensorReading)) {
          Serial.print("Sending ");
          Serial.print(_radioData.SensorReading);
          Serial.println(" to the database");
        }
        /*
        jsonStr = "{\"X\":" + String(x,6) + ",\"Y\":" + String(y,6) + ",\"Z\":" + String(z,6) + "}";

        if (Firebase.pushJSON(firebaseData, path + "/2-pushJSON", jsonStr)) {
          Serial.println(firebaseData.dataPath() + " = " + firebaseData.pushName());
        }
        else {
          Serial.println("Error: " + firebaseData.errorReason());
        }
        */

        Serial.println(msg);

        Serial.print("Weight is ");
        Serial.print(_radioData.SensorReading);
        Serial.println(" milligrams");
        delay(1000);

    }
}
