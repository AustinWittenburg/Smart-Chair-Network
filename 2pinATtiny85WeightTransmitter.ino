#include "HX711.h"
#include "NRFLite.h"

const static uint8_t RADIO_ID = 4;
const static uint8_t DESTINATION_RADIO_ID = 0;
//const static uint8_t PIN_RADIO_CE = 3;
//const static uint8_t PIN_RADIO_CSN = 3;
const static uint8_t PIN_RADIO_MOMI = 4;
const static uint8_t PIN_RADIO_SCK = 1;

struct RadioPacket
{
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
    uint32_t FailedTxCount;
    long SensorReading;
};

NRFLite _radio;
RadioPacket _radioData;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 0;
const int LOADCELL_SCK_PIN = 2;

HX711 scale;

void setup() {
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(4.522);
  scale.tare();
  if (!_radio.initTwoPin(RADIO_ID, PIN_RADIO_MOMI, PIN_RADIO_SCK))
    {
        while (1); // Cannot communicate with radio.
    }

    _radioData.FromRadioId = RADIO_ID;
}

void loop() {

  if (scale.is_ready()) {
    _radioData.OnTimeMillis = millis();

    if (!_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData)))
    {
        _radioData.FailedTxCount++;
    }
    long reading = scale.read();
    //long reading = analogRead(3);
    _radioData.SensorReading = reading;
  }
  delay(500);
}
