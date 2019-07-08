#ifndef OBELISK_CLOUD_FUNCTIONS_H
#define OBELISK_CLOUD_FUNCTIONS_H

#include "Particle.h"
#include "colour.h"
#include "ledStripDriver.h"

class CloudFunctions {
  public:
  CloudFunctions(LedStripDriver *ledDriver);
  ~CloudFunctions();

  void registerFunctions(int (*regFn)(String,
                                      int (CloudFunctions::*cloudFn)(String),
                                      CloudFunctions*));

  int blink(String args);
  int colour(String args);
  int defaultLoop(String args);
  int strobe(String args);
  int gradient(String args);
  int progress(String args);
  int pulse(String args);
  int snake(String args);
  int weather(String args);
  int wind(String args);

  private:
  LedStripDriver *mLedDriver;
  Colour *mColourOn;
  Colour *mColourOff;
  Colour *mWeatherRainColour;
  Colour *mWeatherWarningColour;
  Colour *mWindDirectionColour;
  Colour *mPrevWindDirectionColour;

  void deleteColours();

};


#endif
