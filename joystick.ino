// Requires Tools > USB Mode > "USB-OTG (TinyUSB)" and USB CDC On Boot > Enabled
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard keyboard;

// ADC2_0 = GPIO11 (labled 10 on ESP32-S3 board, wired to VRY on joystick board)
const int adcPin0 = 11;

// ADC1_9 = GPIO10 (kind of confused about this, wired to VRX on joystick board)
const int adcPin1 = 10;

// GPIO12 (labeled "11" on ESP32-S3 board, wired to SW on joystick board)
const int buttonPin = 12; // wired to GND, internal pull-up

const int lowThreshold = 500;
const int highThreshold = 3600;

const int sampleIntervalMs = 50; // 20 Hz

bool leftActive = false;
bool rightActive = false;
bool downActive = false;
bool upActive = false;
bool buttonActive = false;

void tapOnRisingEdge(bool conditionNow, bool &wasActive, uint8_t key) {
   if (conditionNow && !wasActive) {
      keyboard.write(key);
   }
   wasActive = conditionNow;
}

void setup() {
   Serial.begin(115200);
   pinMode(buttonPin, INPUT_PULLUP);

   keyboard.begin();
   USB.begin();
}

void loop() {
   int value0 = analogRead(adcPin0);
   int value1 = analogRead(adcPin1);
   bool buttonDown = digitalRead(buttonPin) == LOW;

   Serial.print("A0:");
   Serial.print(value0);
   Serial.print(" A1:");
   Serial.print(value1);
   Serial.print(" Button:");
   Serial.println(buttonDown ? "DOWN" : "UP");

   tapOnRisingEdge(value0 < lowThreshold, leftActive, KEY_LEFT_ARROW);
   tapOnRisingEdge(value0 > highThreshold, rightActive, KEY_RIGHT_ARROW);
   tapOnRisingEdge(value1 < lowThreshold, downActive, KEY_DOWN_ARROW);
   tapOnRisingEdge(value1 > highThreshold, upActive, KEY_UP_ARROW);
   tapOnRisingEdge(buttonDown, buttonActive, KEY_RIGHT_CTRL);

   delay(sampleIntervalMs);
}
