#include <LoRaWan.h>

#define SERIAL_BAUD_RATE   115200
#define FREQ_MHZ           915
#define TX_PREAMBLE        8
#define RX_PREAMBLE        8
#define TX_POWER           20

#define LOOP_DELAY_MS      10

#define EVENT_BTN_PRESS    "Button"
#define PIN_BUTTON         8
#define BUTTON_VALUE_ON    LOW
#define BUTTON_VALUE_OFF   HIGH

#define GPIO_ON_COUNT_MAX  1

#define BUF_LEN            128

char buffer[BUF_LEN];

/**********************************************************
 * Send a button press event using LoRa P2P
 *********************************************************/
void sendButtonEvent() {
  SerialUSB.println("Sending button event");

  sprintf(buffer, "%s", EVENT_BTN_PRESS);
  lora.transferPacketP2PMode(buffer);
}

/**********************************************************
 * Filter the given value to output BUTTON_VALUE_ON for a 
 * maximum time of GPIO_ON_COUNT_MAX * LOOP_DELAY per 
 * off -> on transition of the pin.
 * 
 * Input:   value to filter
 * Returns: filtered value
 *********************************************************/
int filterGpioValue(int value) {
  static int prevValue = BUTTON_VALUE_OFF;
  static int counter = 0;
  static int output = BUTTON_VALUE_OFF;

  if (prevValue == BUTTON_VALUE_OFF and value == BUTTON_VALUE_ON) {
    output = BUTTON_VALUE_ON;
    counter = 0;
  }

  if (counter >= GPIO_ON_COUNT_MAX) {
    output = BUTTON_VALUE_OFF;
  } else {
    ++counter;
  }

  prevValue = value;

  return output;
}

void setup(void) {
  SerialUSB.begin(SERIAL_BAUD_RATE);

  lora.init();
  lora.initP2PMode(FREQ_MHZ, SF12, BW125, TX_PREAMBLE, RX_PREAMBLE, TX_POWER);

  pinMode(PIN_BUTTON, INPUT_PULLUP);

  SerialUSB.println("Waiting for button press");
}

void loop(void) {
  int buttonRawVal = digitalRead(PIN_BUTTON);
  int buttonValue = filterGpioValue(buttonRawVal);

  if (buttonValue == BUTTON_VALUE_ON) {
    sendButtonEvent();
  }

  delay(LOOP_DELAY_MS);
}
