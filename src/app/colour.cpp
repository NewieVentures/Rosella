#include "colour.h"
#include "utils.h"

#define STR_LEN 7

uint8_t hexColourStrValToInt(String hexStr) {
  return (uint8_t) hexStrToInt(hexStr);
}

Colour::Colour() {
  mRed = 0;
  mGreen = 0;
  mBlue = 0;
  mIsValid = false;
}

Colour::Colour(uint8_t red, uint8_t green, uint8_t blue) {
  mRed = red;
  mGreen = green;
  mBlue = blue;
  mIsValid = true;
};

//value = '#rrggbb'
Colour::Colour(String value) {
  if (value.charAt(0) != '#' || value.length() != STR_LEN) {
    mIsValid = false;
  } else {
    const String &redStr = value.substring(1,3);
    const String &greenStr = value.substring(3,5);
    const String &blueStr = value.substring(5,7);

    mRed = hexColourStrValToInt(redStr);
    mGreen = hexColourStrValToInt(greenStr);
    mBlue = hexColourStrValToInt(blueStr);
    mIsValid = true;
  }
};

uint8_t Colour::getRed() const {
  return mRed;
}

uint8_t Colour::getGreen() const {
  return mGreen;
}

uint8_t Colour::getBlue() const {
  return mBlue;
}

bool Colour::isBlack() {
  return (mRed == 0 && mGreen == 0 && mBlue == 0);
}

bool Colour::isValid() {
  return mIsValid;
}

void Colour::update(uint8_t red, uint8_t green, uint8_t blue) {
  mRed = red;
  mGreen = green;
  mBlue = blue;
}

void Colour::copy(Colour* source) {
  mRed = source->getRed();
  mGreen = source->getGreen();
  mBlue = source->getBlue();
}

bool operator==(const Colour& lhs, const Colour& rhs) {
  return (
    lhs.getRed() == rhs.getRed() &&
    lhs.getGreen() == rhs.getGreen() &&
    lhs.getBlue() == rhs.getBlue()
  );
}

bool operator!=(const Colour& lhs, const Colour& rhs) {
  return !(lhs == rhs);
}

String Colour::toString() {
  char str[STR_LEN+1];

  sprintf(str, "#%02X%02X%02X", mRed, mGreen, mBlue);

  return String(str);
}
