#ifndef OBELISK_COLOUR_H
#define OBELISK_COLOUR_H

#include "Particle.h"

class Colour {
  private:
  uint8_t mRed, mGreen, mBlue;
  bool mIsValid;

  public:
  Colour();
  Colour(uint8_t red, uint8_t green, uint8_t blue);
  Colour(String);

  uint8_t getRed() const;
  uint8_t getGreen() const;
  uint8_t getBlue() const;

  void update(uint8_t red, uint8_t green, uint8_t blue);
  void copy(Colour* source);

  bool isBlack();
  bool isValid();

  String toString();
};

bool operator==(const Colour& lhs, const Colour& rhs);
bool operator!=(const Colour& lhs, const Colour& rhs);

#endif
