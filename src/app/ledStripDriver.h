#ifndef OBELISK_LED_STRIP_DRIVER_H
#define OBELISK_LED_STRIP_DRIVER_H

#include "colour.h"

// Number of colours in default loop pattern
#define DL_NUM_COL 4

enum Pattern {
  air,
  blink,
  colour,
  defaultLoop,
  gradient,
  progress,
  pulse,
  snake,
  strobe,
  weather,
  wind
};

enum Direction {
  forward,
  reverse
};

enum FadeState {
  fadeIn,
  fadeOut,
  offDwell
};

enum WindPattern {
  direction,
  speed,
};

typedef struct {
  uint32_t numLeds;
  void (*writeValueFn)(uint8_t *values, uint32_t length);
  uint32_t resolutionMs;
} led_strip_config_t;

typedef struct {
  uint32_t counter;

  int32_t dutyDirection;
  double dutyCycle;

  uint32_t progress;

  int8_t fadeDirection;

  uint32_t weatherRainCounter;
  uint8_t weatherRainPosition;
  uint32_t weatherWarningCounter;
  FadeState weatherWarningFadeState;

  uint32_t defaultLoopCurColIdx;

  bool windSpeedTransition;
  uint32_t timeoutCounter; // Pattern timeout
  uint32_t windSpeedTimeoutCounter;
  uint32_t windDirectionFadeCounter;
} led_strip_state_t;

class LedStripDriver {
private:
  led_strip_config_t* mConfig;
  uint32_t mPatternTimeoutMs;
  bool mClearPatternTimeout;
  bool mResetState; // flag to reset state on next loop

  void handleBlinkPattern(led_strip_state_t *state, uint8_t *values);
  void handlePulsePattern(led_strip_state_t *state, uint8_t *values);
  void handleColourPattern(led_strip_state_t *state, uint8_t *values);
  void handleStrobePattern(led_strip_state_t *state, uint8_t *values);
  void handleProgressPattern(led_strip_state_t *state, uint8_t *values);
  void handleGradientPattern(led_strip_state_t *state, uint8_t *values);
  void handleSnakePattern(led_strip_state_t *state, uint8_t *values);
  void handleWeatherPattern(led_strip_state_t *state, uint8_t *values);
  void handleDefaultLoopPattern(led_strip_state_t *state, uint8_t *values);
  void handleWindPattern(led_strip_state_t *state, uint8_t *values);
  void handleAirPattern(led_strip_state_t *state, uint8_t *values);

  void handleWeatherTempLayer(led_strip_state_t *state, uint8_t *values, Colour *outputColour);
  void updateWeatherRainState(led_strip_state_t *state);
  void writeWeatherRainValues(led_strip_state_t *state, uint8_t *values);
  void writeWeatherWarningValues(led_strip_state_t *state, uint8_t *values);
  void updateWeatherWarningCounter(led_strip_state_t *state);
  void updateWindCounters(led_strip_state_t *state);
  void resetState(led_strip_state_t *state);

protected:
  uint32_t mPeriodMs;
  Colour *mColourOn;
  Colour *mColourOff;
  Pattern mPattern;

  uint8_t mDutyCycle;

  Direction mSnakeDirection;
  uint32_t mSnakeLength;

  uint8_t mProgressInitial;
  uint8_t mProgressFinal;
  uint8_t mProgressIncrement;
  uint32_t mProgressIncrementDelayMs;
  uint32_t mProgressResetDelayMs;
  Direction mProgressDirection;

  uint32_t mWeatherTempFadeIntervalSecs;
  uint8_t mWeatherRainBandHeightLeds;
  uint32_t mWeatherRainBandIncDelayMs;
  uint8_t mWeatherRainBandSpacingLeds;
  Colour *mWeatherRainBandColour;
  Direction mWeatherRainDirection;
  Colour *mWeatherWarningColour;
  uint32_t mWeatherWarningFadeInMs;
  uint32_t mWeatherWarningFadeOutMs;
  uint32_t mWeatherWarningOffDwellMs;

  Colour* mDefaultLoopColours[DL_NUM_COL];

  Colour* mWindDirectionColour;
  Colour* mPrevWindDirectionColour; // used for fading to new value
  bool mWindDirTransition; // true when fading to new direction value
  WindPattern mCurrentWindPattern;
  uint32_t mWindSpeedTimeoutMs;

  Colour* mLayer2Colour1;
  Colour* mLayer2Colour2;
  uint32_t mActiveLayer;

public:
  void initState(led_strip_state_t *state);
  LedStripDriver(led_strip_config_t *config);
  void onTimerFired(led_strip_state_t *state, uint8_t *values);
  void onPatternTimeout(led_strip_state_t *state, uint8_t *values);

  LedStripDriver* period(uint32_t valueMs);
  LedStripDriver* colourOn(Colour *colour);
  LedStripDriver* colourOff(Colour *colour);
  LedStripDriver* pattern(Pattern pattern);

  /* Set a pattern timeout, behaviour set in onPatternTimeout() */
  LedStripDriver* timeout(uint32_t timeoutMs);

  /*
   * Used by pulse pattern to set % of time spent turning on vs off
   * eg 50 -> same time fading on as off
   *    20 -> 20% time used to fade on, 80% to fade off
   *    80 -> 80% time used to fade on, 20% to fade off
   */
  LedStripDriver* dutyCycle(uint8_t value);

  /* Used by snake pattern to set length of snake in LEDs */
  LedStripDriver* length(uint8_t numLeds);

  /*
  * Used by snake pattern to set direction of snake
  * eg forward = first LED to last
  *    reverse = last LED to first
  */
  LedStripDriver* snakeDirection(Direction direction);

  /* Used by progress pattern to set inital progress value */
  LedStripDriver* initialValue(uint8_t progress);

  /* Used by progress pattern to set number of LEDs per increment */
  LedStripDriver* increment(uint8_t leds);

  /* Used by progress pattern to set number of ms between increments */
  LedStripDriver* incDelay(uint32_t delayMs);

  /* Used by progress pattern to set number of ms between patterns */
  LedStripDriver* resetDelay(uint32_t delayMs);

  /* Used by progress pattern to set maximum progress value */
  LedStripDriver* finalValue(uint8_t progress);

  /* Used by weather pattern to set temperature fade interval (secs) */
  LedStripDriver* tempFadeInterval(uint32_t fadeTimeSecs);

  /* Used by weather pattern to set rain band height */
  LedStripDriver* rainBandHeight(uint8_t leds);

  /* Used by weather pattern to set the delay between band movements */
  LedStripDriver* rainBandIncrementDelay(uint32_t delayMs);

  /* Used by weather pattern to set rain band spacing (# leds apart) */
  LedStripDriver* rainBandSpacing(uint8_t leds);

  /* Used by weather pattern to set rain band colour */
  LedStripDriver* rainBandColour(Colour *colour);

  /*
  * Used by weather pattern to set direction of rain
  * eg forward = first LED to last
  *    reverse = last LED to first
  */
  LedStripDriver* rainDirection(Direction direction);

  /* Used by weather pattern to set weather warning colour */
  LedStripDriver* warningColour(Colour *colour);

  /* Used by weather pattern to set weather warning fade in time (ms) */
  LedStripDriver* warningFadeIn(uint32_t fadeTimeMs);

  /* Used by weather pattern to set weather warning fade in time (ms) */
  LedStripDriver* warningFadeOut(uint32_t fadeTimeMs);

  /* Used by weather pattern to set weather warning off dwell time (ms) */
  LedStripDriver* warningOffDwell(uint32_t offDwellMs);

  /* Used by wind pattern to set wind direction colour */
  LedStripDriver* windDirectionColour(Colour *colour);

  /* Used by wind pattern to fade to new wind direction colour */
  LedStripDriver* prevWindDirectionColour(Colour *colour);

  /* Used by wind pattern to set timeout for speed layer */
  LedStripDriver* windSpeedTimeout(uint32_t timeoutMs);

  /* Used by wind pattern to show the wind speed for a certain time */
  LedStripDriver* showWindSpeed(led_strip_state_t *state, uint32_t timeoutMs);

  /* Used by air pattern to change the displayed layer */
  LedStripDriver* activeLayer(uint32_t layer);

  LedStripDriver* layer2Colour1(Colour *colour);
  LedStripDriver* layer2Colour2(Colour *colour);

  /*
   * Reset the state on next loop iteration. Used when pattern changes if state reset required.
   */
  LedStripDriver* resetState();

  /*
  * Used by progress pattern to set direction of increment
  * eg forward = first LED to last
  *    reverse = last LED to first
  */
  LedStripDriver* progressDirection(Direction direction);

  /*
   * Stop the transition from one wind direction colour to the next. Useful for testing.
   */
  void cancelWindDirectionTransition();

  uint32_t getPeriod() { return mPeriodMs; };
  Colour* getColourOn() { return mColourOn; };
  Colour* getColourOff() { return mColourOff; };
  Pattern getPattern() { return mPattern; };

  uint8_t getDutyCycle() { return mDutyCycle; };

  uint8_t getProgressInitial() { return mProgressInitial; };
  uint8_t getProgressFinal() { return mProgressFinal; };
  uint8_t getProgressIncrement() { return mProgressIncrement; };
  uint32_t getProgressIncrementDelay() { return mProgressIncrementDelayMs; };
  uint32_t getProgressResetDelay() { return mProgressResetDelayMs; };
  Direction getProgressDirection() { return mProgressDirection; };

  uint32_t getSnakeLength() { return mSnakeLength; };
  Direction getSnakeDirection() { return mSnakeDirection; };
  WindPattern getCurrentWindPattern() { return mCurrentWindPattern; };
  uint32_t getPatternTimeout() { return mPatternTimeoutMs; };

  uint32_t getWeatherTempFadeIntervalSecs() { return mWeatherTempFadeIntervalSecs; };
  uint8_t getWeatherRainBandHeightLeds() { return mWeatherRainBandHeightLeds; };
  uint32_t getWeatherRainBandIncDelayMs() { return mWeatherRainBandIncDelayMs; };
  uint8_t getWeatherRainBandSpacingLeds() { return mWeatherRainBandSpacingLeds; };
  Colour* getWeatherRainBandColour() { return mWeatherRainBandColour; };
  Direction getWeatherRainDirection() { return mWeatherRainDirection; };

  Colour* getWeatherWarningColour() { return mWeatherWarningColour; }
  uint32_t getWeatherWarningFadeIn() { return mWeatherWarningFadeInMs; };
  uint32_t getWeatherWarningFadeOut() { return mWeatherWarningFadeOutMs; };
  uint32_t getWeatherWarningOffDwell() { return mWeatherWarningOffDwellMs; };

  Colour* getWindDirectionColour() { return mWindDirectionColour; };

  Colour* getLayer2Colour1() { return mLayer2Colour1; }
  Colour* getLayer2Colour2() { return mLayer2Colour2; }
};

#endif
