#include "ledStripDriver.h"
#include "colour.h"
#include "colours.h"
#include "config.h"

const Colour COLOUR_DEFAULT = Colour(50, 0, 0);
const Colour COL_BLACK = COLOUR_BLACK;
const Colour COL_WHITE = COLOUR_WHITE;

const Colour COL_DL_YELLOW = COLOUR_DL_YELLOW;
const Colour COL_DL_GREEN = COLOUR_DL_GREEN;
const Colour COL_DL_BLUE = COLOUR_DL_BLUE;
const Colour COL_DL_RED = COLOUR_DL_RED;

#define DUTY_DIR_INC 1
#define DUTY_DIR_DEC -1
#define DUTY_MAX 99
#define DUTY_MIN 1

static void calcLinearOffsets(int32_t *offsets, Colour *startCol);
static void calcLinearGradients(int32_t *offsets, double *gradients, Colour *endCol, double steps);
static uint8_t calcGradientColourValue(double gradient, double offset, uint32_t step);
static Colour* getDefaultLoopStartColour(Colour** colours, uint32_t currentIndex);
static Colour* getDefaultLoopEndColour(Colour** colours, uint32_t currentIndex);
static void calculateFadedColour(Colour *output,
                                 Colour *startCol,
                                 Colour *endCol,
                                 uint32_t step,
                                 uint32_t totalSteps);

void calcLinearOffsets(int32_t *offsets, Colour *startCol) {
  offsets[INDEX_RED] = startCol->getRed();
  offsets[INDEX_GREEN] = startCol->getGreen();
  offsets[INDEX_BLUE] = startCol->getBlue();
}

void calcLinearGradients(int32_t *offsets, double *gradients, Colour *endCol, double steps) {
  gradients[INDEX_RED] = (endCol->getRed() - offsets[INDEX_RED]) / steps;
  gradients[INDEX_GREEN] = (endCol->getGreen() - offsets[INDEX_GREEN]) / steps;
  gradients[INDEX_BLUE] = (endCol->getBlue() - offsets[INDEX_BLUE]) / steps;
}

void LedStripDriver::initState(led_strip_state_t *state) {
  state->counter = 0;
  state->dutyCycle = 1;
  state->dutyDirection = DUTY_DIR_INC;
  state->fadeDirection = 1;

  state->weatherRainCounter = 0;
  state->weatherRainPosition = 0;

  state->weatherWarningCounter = 0;
  state->weatherWarningFadeState = fadeIn;

  state->defaultLoopCurColIdx = 0;

  state->windSpeedTransition = false;
  state->windSpeedTimeoutCounter = 0;
}

LedStripDriver::LedStripDriver(led_strip_config_t *config) {
  mConfig = config;
  mPeriodMs = 1000;
  mColourOn = (Colour*)&COLOUR_DEFAULT;
  mColourOff = (Colour*)&COL_BLACK;
  mPattern = colour;

  mDutyCycle = 50;

  mSnakeDirection = Direction::forward;
  mSnakeLength = 3;

  mProgressInitial = 0;
  mProgressFinal = config->numLeds;
  mProgressIncrement = 1;
  mProgressIncrementDelayMs = 1000;
  mProgressResetDelayMs = 0;
  mProgressDirection = Direction::forward;

  mWeatherTempFadeIntervalSecs = 4;
  mWeatherRainBandHeightLeds = 0; // no rain by default
  mWeatherRainBandIncDelayMs = 0;
  mWeatherRainBandSpacingLeds = 0;
  mWeatherRainBandColour = (Colour*)&COL_WHITE;
  mWeatherRainDirection = Direction::forward;

  mWeatherWarningColour = (Colour*)&COL_WHITE;
  mWeatherWarningFadeInMs = 0;
  mWeatherWarningFadeOutMs = 0;
  mWeatherWarningOffDwellMs = 0;

  mDefaultLoopColours[0] = (Colour*)&COL_DL_YELLOW;
  mDefaultLoopColours[1] = (Colour*)&COL_DL_GREEN;
  mDefaultLoopColours[2] = (Colour*)&COL_DL_BLUE;
  mDefaultLoopColours[3] = (Colour*)&COL_DL_RED;

  mWindDirectionColour = (Colour*)&COL_BLACK;

  mPatternTimeoutMs = 0;
  mCurrentWindPattern = direction;
  mClearPatternTimeout = false;

  mPrevWindDirectionColour = (Colour*)&COL_BLACK;

  mResetState = false;

  mLayer2Colour1 = (Colour*)&COL_BLACK;
  mLayer2Colour2 = (Colour*)&COL_BLACK;
  mActiveLayer = 1;
};

inline void reverseDutyCycleDirection(led_strip_state_t *state) {
  state->dutyDirection = -state->dutyDirection;
}

void writeColourValues(uint8_t *values, uint32_t numLeds, Colour *colour) {
  for (uint32_t i=0; i<numLeds; i++) {
    values[i * COLOURS_PER_LED + INDEX_RED] = colour->getRed();
    values[i * COLOURS_PER_LED + INDEX_GREEN] = colour->getGreen();
    values[i * COLOURS_PER_LED + INDEX_BLUE] = colour->getBlue();

    #ifdef INDEX_WHITE
      values[i * COLOURS_PER_LED + INDEX_WHITE] = colour->calcWhiteValue();
    #endif
  }
}

double calcDutyCycleIncrement(uint8_t dutyCycle, uint32_t pulsePeriod, uint32_t pwmPeriod) {
  return (double)(100 * 100 / dutyCycle)/((double)pulsePeriod / pwmPeriod);
}

void updateDutyCycle(led_strip_state_t *state, double increment, double decrement) {
  if (state->dutyDirection == DUTY_DIR_INC) {
    state->dutyCycle += increment;
  } else {
    state->dutyCycle -= decrement;
  }
}

void LedStripDriver::handlePulsePattern(led_strip_state_t *state, uint8_t *values) {
  const uint32_t num_leds = mConfig->numLeds;
  const uint32_t steps = (mPeriodMs / mConfig->resolutionMs) - 1;
  uint32_t currentStep;
  Colour outputColour;
  Colour *startCol, *endCol;

  if (state->counter >= mPeriodMs) {
    state->counter = 0;
    state->dutyDirection *= -1;
  }

  currentStep = state->counter / mConfig->resolutionMs;

  if (state->dutyDirection > 0) {
    startCol = mColourOn;
    endCol = mColourOff;
  } else {
    startCol = mColourOff;
    endCol = mColourOn;
  }

  calculateFadedColour(&outputColour,
                       startCol,
                       endCol,
                       currentStep,
                       steps);

  writeColourValues(values, num_leds, &outputColour);
}

void LedStripDriver::handleBlinkPattern(led_strip_state_t *state, uint8_t *values) {
  uint32_t onTimeMs = (uint32_t)((mPeriodMs * mDutyCycle) / 100);
  Colour *colour;

  if (state->counter >= mPeriodMs) {
    state->counter = 0;
  }

  if (state->counter < onTimeMs) {
    colour = mColourOn;
  } else {
    colour = mColourOff;
  }

  writeColourValues(values, mConfig->numLeds, colour);
}

void LedStripDriver::handleColourPattern(led_strip_state_t *state, uint8_t *values) {
  writeColourValues(values, mConfig->numLeds, mColourOn);
}

void LedStripDriver::handleStrobePattern(led_strip_state_t *state, uint8_t *values) {
  uint32_t onTimeMs = (uint32_t)(mPeriodMs / 2);
  Colour *colour;

  if (state->counter >= mPeriodMs) {
    state->counter = 0;
  }

  if (state->counter < onTimeMs) {
    colour = mColourOn;
  } else {
    colour = mColourOff;
  }

  writeColourValues(values, mConfig->numLeds, colour);
}

void LedStripDriver::handleProgressPattern(led_strip_state_t *state, uint8_t *values) {
  uint32_t progressValue = mProgressInitial + state->progress;
  uint32_t ledsOn = (progressValue > mProgressFinal ? mProgressFinal : progressValue);
  uint32_t ledsOff = mConfig->numLeds - ledsOn;

  if ((mProgressFinal - ledsOn) == 0) {
    if (state->counter >= (mProgressIncrementDelayMs + mProgressResetDelayMs)) {
      state->counter = 0;
      state->progress = 0;
    }
  } else if (state->counter >= mProgressIncrementDelayMs) {
    state->progress += mProgressIncrement;
    state->counter = 0;
  }

  if (mProgressDirection == Direction::forward) {
    writeColourValues(values, ledsOn, mColourOn);
    writeColourValues(&values[ledsOn * COLOURS_PER_LED], ledsOff, mColourOff);
  } else {
    uint32_t onIndex = (mConfig->numLeds - ledsOn) * COLOURS_PER_LED;
    uint32_t offIndex = onIndex - (ledsOff * COLOURS_PER_LED);
    writeColourValues(&values[onIndex], ledsOn, mColourOn);
    writeColourValues(&values[offIndex], ledsOff, mColourOff);
  }
}

uint8_t calcGradientColourValue(double gradient, double offset, uint32_t step) {
  int32_t value = (int32_t)((gradient * (int32_t)step) + offset);

  if (value < 0) {
    value = 0;
  }

  if (value > 255) {
    value = 255;
  }

  return value;
}

void LedStripDriver::handleGradientPattern(led_strip_state_t *state, uint8_t *values) {
  const uint32_t steps = mConfig->numLeds - 1;
  int32_t offsets[COLOURS_PER_LED];
  double gradients[COLOURS_PER_LED];
  Colour outputColour;

  calcLinearOffsets(offsets, mColourOn);
  calcLinearGradients(offsets, gradients, mColourOff, (double)steps);

  //ensure no rounding errors for end values
  values[steps * COLOURS_PER_LED + INDEX_RED] = mColourOff->getRed();
  values[steps * COLOURS_PER_LED + INDEX_GREEN] = mColourOff->getGreen();
  values[steps * COLOURS_PER_LED + INDEX_BLUE] = mColourOff->getBlue();

  #ifdef INDEX_WHITE
    values[steps * COLOURS_PER_LED + INDEX_WHITE] = mColourOff->calcWhiteValue();
  #endif

  for (uint32_t i=0; i < steps; i++) {
    outputColour.update(
      calcGradientColourValue(gradients[INDEX_RED], offsets[INDEX_RED], i),
      calcGradientColourValue(gradients[INDEX_GREEN], offsets[INDEX_GREEN], i),
      calcGradientColourValue(gradients[INDEX_BLUE], offsets[INDEX_BLUE], i)
    );

    values[i * COLOURS_PER_LED + INDEX_RED] = outputColour.getRed();
    values[i * COLOURS_PER_LED + INDEX_GREEN] = outputColour.getGreen();
    values[i * COLOURS_PER_LED + INDEX_BLUE] = outputColour.getBlue();

    #ifdef INDEX_WHITE
      values[i * COLOURS_PER_LED + INDEX_WHITE] = outputColour.calcWhiteValue();
    #endif
  }
}

void LedStripDriver::handleSnakePattern(led_strip_state_t *state, uint8_t *values) {
  const uint32_t num_leds = mConfig->numLeds;
  const uint32_t PROGRESS_MAX = num_leds + mSnakeLength;
  const uint32_t INCREMENT_MS = mPeriodMs / (num_leds + mSnakeLength);
  uint32_t start;
  uint32_t end;

  if (mSnakeDirection == Direction::forward) {
    start = mSnakeLength > state->progress ? 0 : state->progress - mSnakeLength;
    end = state->progress;
  } else {
    end = PROGRESS_MAX - state->progress;
    start = state->progress < num_leds ? (end - mSnakeLength): 0;
  }


  for (uint8_t i=0; i < num_leds; i++) {
    Colour *colour = (i >= start && i < end) ? mColourOn : mColourOff;

    values[(i * COLOURS_PER_LED) + INDEX_RED] = colour->getRed();
    values[(i * COLOURS_PER_LED) + INDEX_GREEN] = colour->getGreen();
    values[(i * COLOURS_PER_LED) + INDEX_BLUE] = colour->getBlue();

    #ifdef INDEX_WHITE
      values[i * COLOURS_PER_LED + INDEX_WHITE] = colour->calcWhiteValue();
    #endif
  }

  if (state->counter >= INCREMENT_MS) {
    state->progress += 1;
    state->counter = 0;

    if (state->progress >= PROGRESS_MAX) {
      state->progress = 0;
    }
  }
}

void LedStripDriver::updateWeatherRainState(led_strip_state_t *state) {
  state->weatherRainCounter += mConfig->resolutionMs;

  if (state->weatherRainCounter >= mWeatherRainBandIncDelayMs) {
    state->weatherRainCounter = 0;
    state->weatherRainPosition += 1;

    if (state->weatherRainPosition >= mConfig->numLeds) {
      state->weatherRainPosition = 0;
    }
  }
}

void LedStripDriver::writeWeatherRainValues(led_strip_state_t *state, uint8_t *values) {
  const uint32_t num_leds = mConfig->numLeds;
  //get initial position with bands wrapping around
  uint32_t bandAndSpacingHeight = mWeatherRainBandHeightLeds + mWeatherRainBandSpacingLeds;
  uint32_t rainInitialPosition = state->weatherRainPosition % bandAndSpacingHeight;

  if (mWeatherRainDirection == Direction::forward) {
    for (uint32_t i = rainInitialPosition; i < num_leds; i += bandAndSpacingHeight) {
      for (uint32_t j = 0; j < mWeatherRainBandHeightLeds; j++) {
        if ((i+j) < num_leds) { //all bands may not fit on the LED strip, stop before overflow
          values[(i+j) * COLOURS_PER_LED + INDEX_RED] = mWeatherRainBandColour->getRed();
          values[(i+j) * COLOURS_PER_LED + INDEX_GREEN] = mWeatherRainBandColour->getGreen();
          values[(i+j) * COLOURS_PER_LED + INDEX_BLUE] = mWeatherRainBandColour->getBlue();

          #ifdef INDEX_WHITE
            values[(i+j) * COLOURS_PER_LED + INDEX_WHITE] = mWeatherRainBandColour->calcWhiteValue();
          #endif
        }
      }
    }
  } else {
    for (int32_t i = (num_leds - 1 - rainInitialPosition); i >= 0; i -= bandAndSpacingHeight) {
      for (uint32_t j = 0; j < mWeatherRainBandHeightLeds; j++) {
        //all bands may not fit on the LED strip, stop before overflow
        if ((i+j) < num_leds && (i+j) >= 0) {
          values[(i+j) * COLOURS_PER_LED + INDEX_RED] = mWeatherRainBandColour->getRed();
          values[(i+j) * COLOURS_PER_LED + INDEX_GREEN] = mWeatherRainBandColour->getGreen();
          values[(i+j) * COLOURS_PER_LED + INDEX_BLUE] = mWeatherRainBandColour->getBlue();

          #ifdef INDEX_WHITE
            values[(i+j) * COLOURS_PER_LED + INDEX_WHITE] = mWeatherRainBandColour->calcWhiteValue();
          #endif
        }
      }
    }
  }
}

void LedStripDriver::handleWeatherTempLayer(led_strip_state_t *state,
                                            uint8_t *values,
                                            Colour *outputColour) {
  uint32_t steps = ((mWeatherTempFadeIntervalSecs * 1000) / mConfig->resolutionMs) - 1;
  uint32_t currentStep = state->counter / mConfig->resolutionMs;
  Colour *colourStart, *colourEnd;

  if (currentStep >= steps) {
    state->counter = 0;
    state->fadeDirection *= -1;

    currentStep = 0;
  }

  if (state->fadeDirection > 0) {
    colourStart = mColourOn;
    colourEnd = mColourOff;
  } else {
    colourStart = mColourOff; //reverse colours
    colourEnd = mColourOn;
  }

  if (currentStep >= (steps - 1)) {
    outputColour->copy(colourEnd);
  } else {
    calculateFadedColour(outputColour,
                         colourStart,
                         colourEnd,
                         currentStep,
                         steps);
  }
}

void LedStripDriver::handleWeatherPattern(led_strip_state_t *state, uint8_t *values) {
  const uint32_t num_leds = mConfig->numLeds;
  Colour outputColour;

  // temperature layer
  handleWeatherTempLayer(state, values, &outputColour);
  writeColourValues(values, num_leds, &outputColour);

  // add rain bands
  updateWeatherRainState(state);

  if (mWeatherRainBandHeightLeds > 0) {
    writeWeatherRainValues(state, values);
  }

  updateWeatherWarningCounter(state);

  // draw weather warning on top of other layers if enabled
  if (mWeatherWarningFadeInMs > 0) {
    writeWeatherWarningValues(state, values);
  }
}

Colour* getDefaultLoopStartColour(Colour** colours, uint32_t currentIndex) {
  return colours[currentIndex];
}

Colour* getDefaultLoopEndColour(Colour** colours, uint32_t currentIndex) {
  if (currentIndex >= (DL_NUM_COL-1)) {
    return colours[0];
  }

  return colours[currentIndex+1];
}

void LedStripDriver::handleDefaultLoopPattern(led_strip_state_t *state, uint8_t *values) {
  const uint32_t steps = (mPeriodMs / mConfig->resolutionMs) - 1;
  uint32_t currentStep;
  Colour outputColour;
  Colour *startCol, *endCol;

  if (state->counter >= mPeriodMs) {
    state->counter = 0;

    ++state->defaultLoopCurColIdx;

    if (state->defaultLoopCurColIdx >= DL_NUM_COL) {
      state->defaultLoopCurColIdx = 0;
    }
  }

  startCol = getDefaultLoopStartColour(mDefaultLoopColours, state->defaultLoopCurColIdx);
  endCol = getDefaultLoopEndColour(mDefaultLoopColours, state->defaultLoopCurColIdx);

  currentStep = state->counter / mConfig->resolutionMs;

  calculateFadedColour(&outputColour,
                       startCol,
                       endCol,
                       currentStep,
                       steps);

  writeColourValues(values, mConfig->numLeds, &outputColour);
}

void calculateFadedColour(Colour *output,
                          Colour *startCol,
                          Colour *endCol,
                          uint32_t step,
                          uint32_t totalSteps) {
  int32_t offsets[COLOURS_PER_LED];
  double gradients[COLOURS_PER_LED];

  calcLinearOffsets(offsets, startCol);
  calcLinearGradients(offsets, gradients, endCol, (double)totalSteps);

  output->update(
    calcGradientColourValue(gradients[INDEX_RED], offsets[INDEX_RED], step),
    calcGradientColourValue(gradients[INDEX_GREEN], offsets[INDEX_GREEN], step),
    calcGradientColourValue(gradients[INDEX_BLUE], offsets[INDEX_BLUE], step));
}

void LedStripDriver::updateWeatherWarningCounter(led_strip_state_t *state) {
  state->weatherWarningCounter += mConfig->resolutionMs;

  switch(state->weatherWarningFadeState) {
    case fadeIn:
      if (state->weatherWarningCounter >= mWeatherWarningFadeInMs) {
        state->weatherWarningCounter = 0;
        state->weatherWarningFadeState = fadeOut;
      }
      break;

    case fadeOut:
      if (state->weatherWarningCounter >= mWeatherWarningFadeOutMs) {
        state->weatherWarningCounter = 0;
        state->weatherWarningFadeState = offDwell;
      }
      break;

    case offDwell:
      if (state->weatherWarningCounter >= mWeatherWarningOffDwellMs) {
        state->weatherWarningCounter = 0;
        state->weatherWarningFadeState = fadeIn;
      }
      break;
  }
}

void LedStripDriver::writeWeatherWarningValues(led_strip_state_t *state, uint8_t *values) {
  uint32_t resolutionMs = mConfig->resolutionMs;
  Colour warningColourStart = COLOUR_BLACK;
  Colour outputColour;
  uint32_t steps = 0, currentStep = 0;

    if (state->weatherWarningFadeState == fadeIn) {
      steps = mWeatherWarningFadeInMs / resolutionMs;
      currentStep = state->weatherWarningCounter / resolutionMs;

      calculateFadedColour(&outputColour,
                           &warningColourStart,
                           mWeatherWarningColour,
                           currentStep,
                           steps);
    } else if (state->weatherWarningFadeState == fadeOut) {
      steps = mWeatherWarningFadeOutMs / resolutionMs;
      currentStep = state->weatherWarningCounter / resolutionMs;

      calculateFadedColour(&outputColour,
                           mWeatherWarningColour,
                           &warningColourStart,
                           currentStep,
                           steps);
    } else {
      outputColour.copy(&warningColourStart);
    }

    if (steps > 0 && currentStep == (steps - 1) && !outputColour.isBlack()) {
      outputColour.copy(mWeatherWarningColour);
    }

    //black is transparent for the warning layer
    if (!outputColour.isBlack()) {
      writeColourValues(values, mConfig->numLeds, &outputColour);
    }
}

void LedStripDriver::updateWindCounters(led_strip_state_t *state) {
  if (mCurrentWindPattern == speed) {
    state->windSpeedTimeoutCounter += mConfig->resolutionMs;

    // return to wind direction pattern after timeout
    if (state->windSpeedTimeoutCounter >= mWindSpeedTimeoutMs) {
      state->windSpeedTimeoutCounter = 0;

      state->windSpeedTransition = true;
      state->fadeDirection = 1;
      state->counter = 0;
      mCurrentWindPattern = direction;
    }
  }

  if (state->counter >= mPeriodMs) {
    // Don't reverse fade direction if wind speed transition finishing
    if (!state->windSpeedTransition) {
      state->fadeDirection *= -1;
    }

    state->windSpeedTransition = false;
    state->counter = 0;
  }
}

void LedStripDriver::handleWindPattern(led_strip_state_t *state, uint8_t *values) {
  const uint32_t num_leds = mConfig->numLeds;
  uint32_t steps = (mPeriodMs / mConfig->resolutionMs) - 1;
  uint32_t currentStep;
  Colour outputColour;
  Colour *startCol, *endCol;

  updateWindCounters(state);

  if (mWindDirTransition) {
    state->windDirectionFadeCounter += mConfig->resolutionMs;

    if (state->windDirectionFadeCounter >= mPeriodMs) {
      mWindDirTransition = false; // don't fade anymore
      state->windDirectionFadeCounter = 0;
    }
  }

  currentStep = state->counter / mConfig->resolutionMs;

  if (mCurrentWindPattern == direction && !state->windSpeedTransition) {
    if (mWindDirTransition) {
      calculateFadedColour(&outputColour,
                           mPrevWindDirectionColour,
                           mWindDirectionColour,
                           currentStep,
                           steps);
    } else {
      outputColour.copy(mWindDirectionColour);
    }
  } else {
    if (mCurrentWindPattern == direction) {
      startCol = mColourOff;
      endCol = mWindDirectionColour;
    } else if (state->windSpeedTransition) {
      startCol = mWindDirectionColour;
      endCol = mColourOn;
    } else if (state->fadeDirection > 0) {
      startCol = mColourOn;
      endCol = mColourOff;
    } else {
      startCol = mColourOff;
      endCol = mColourOn;
    }

    calculateFadedColour(&outputColour,
                         startCol,
                         endCol,
                         currentStep,
                         steps);
  }

  writeColourValues(values, num_leds, &outputColour);

  updateWeatherWarningCounter(state);

  // draw weather warning on top of other layers if enabled
  if (mWeatherWarningFadeInMs > 0) {
    writeWeatherWarningValues(state, values);
  }
}

void LedStripDriver::handleAirPattern(led_strip_state_t *state, uint8_t *values) {
  uint32_t steps = (mPeriodMs / mConfig->resolutionMs) - 1;
  uint32_t currentStep;
  Colour outputColour;
  Colour *startCol, *endCol;

  if (state->counter >= mPeriodMs) {
    state->counter = 0;
    state->fadeDirection *= -1;
  }

  if (state->fadeDirection > 0) {
    if (mActiveLayer == 2) {
      startCol = mLayer2Colour1;
      endCol = mLayer2Colour2;
    } else {
      startCol = mColourOn;
      endCol = mColourOff;
    }
  } else {
    if (mActiveLayer == 2) {
      startCol = mLayer2Colour2;
      endCol = mLayer2Colour1;
    } else {
      startCol = mColourOff;
      endCol = mColourOn;
    }
  }

  currentStep = state->counter / mConfig->resolutionMs;

  calculateFadedColour(&outputColour,
                       startCol,
                       endCol,
                       currentStep,
                       steps);

  writeColourValues(values, mConfig->numLeds, &outputColour);
}

void LedStripDriver::onTimerFired(led_strip_state_t *state, uint8_t *values) {
  const uint32_t numLedValues = COLOURS_PER_LED * mConfig->numLeds;

  if (mResetState) {
    mResetState = false;
    resetState(state);
  }

  if (mClearPatternTimeout) {
    mClearPatternTimeout = false;

    state->timeoutCounter = 0;
  }

  if (mPatternTimeoutMs > 0) {
    state->timeoutCounter += mConfig->resolutionMs;

    if (state->timeoutCounter >= mPatternTimeoutMs) {
      state->timeoutCounter = 0;
      mPatternTimeoutMs = 0; // prevent timeout from occurring again
      onPatternTimeout(state, values);
    }
  } else {
    state->timeoutCounter = 0; // ensure pattern changes dont resume a previous counter value
  }

  switch(mPattern) {
    case air:
      handleAirPattern(state, values);
      break;

    case blink:
      handleBlinkPattern(state, values);
      break;

    case pulse:
      handlePulsePattern(state, values);
      break;

    case colour:
      handleColourPattern(state, values);
      break;

    case strobe:
      handleStrobePattern(state, values);
      break;

    case progress:
      handleProgressPattern(state, values);
      break;

    case gradient:
      handleGradientPattern(state, values);
      break;

    case snake:
      handleSnakePattern(state, values);
      break;

    case weather:
      handleWeatherPattern(state, values);
      break;

    case wind:
      handleWindPattern(state, values);
      break;

    default:
    case defaultLoop:
      handleDefaultLoopPattern(state, values);
      break;
  }

  mConfig->writeValueFn(values, numLedValues);

  state->counter += mConfig->resolutionMs;
};

void LedStripDriver::onPatternTimeout(led_strip_state_t *state, uint8_t *values) {
  switch(mPattern) {
    case blink:
    case colour:
    case defaultLoop:
    case gradient:
    case progress:
    case pulse:
    case snake:
    case strobe:
      break;

    case air:
    case weather:
    case wind:
      pattern(Pattern::defaultLoop)
        ->period(DEFAULT_LOOP_PERIOD_MS);
      break;
  }
};

void LedStripDriver::resetState(led_strip_state_t *state) {
  state->counter = 0;

  state->dutyDirection = 1;
  state->dutyCycle = 1;

  state->progress = 0;

  state->fadeDirection = Direction::forward;

  state->weatherRainCounter = 0;
  state->weatherRainPosition = 0;
  state->weatherWarningCounter = 0;
  state->weatherWarningFadeState = FadeState::fadeIn;

  state->defaultLoopCurColIdx = 0;

  state->windSpeedTransition = false;
  state->timeoutCounter = 0;
  state->windSpeedTimeoutCounter = 0;
  state->windDirectionFadeCounter = 0;
}

LedStripDriver* LedStripDriver::period(uint32_t valueMs) {
  mPeriodMs = valueMs;
  return this;
}

LedStripDriver* LedStripDriver::dutyCycle(uint8_t value) {
  mDutyCycle = value;
  return this;
};

LedStripDriver* LedStripDriver::colourOn(Colour *colour) {
  mColourOn = colour;
  return this;
};

LedStripDriver* LedStripDriver::colourOff(Colour *colour) {
  mColourOff = colour;
  return this;
};

LedStripDriver* LedStripDriver::pattern(Pattern pattern) {
  mPattern = pattern;
  mClearPatternTimeout = true; // reset inactivity timeout
  mWindDirTransition = true; // fade to new wind direction value (if applicable)
  return this;
};

  /* Set a pattern timeout, behaviour set in onPatternTimeout() */
LedStripDriver* LedStripDriver::timeout(uint32_t timeoutMs) {
  mPatternTimeoutMs = timeoutMs;
  return this;
}


/* Used by snake pattern to set length of snake in LEDs */
LedStripDriver* LedStripDriver::length(uint8_t numLeds) {
  mSnakeLength = numLeds;
  return this;
};

/*
* Used by snake pattern to set direction of snake
* eg forward = first LED to last
*    reverse = last LED to first
*/
LedStripDriver* LedStripDriver::snakeDirection(Direction direction) {
  mSnakeDirection = direction;
  return this;
};

/*
* Used by progress pattern to set direction of increment
* eg forward = first LED to last
*    reverse = last LED to first
*/
LedStripDriver* LedStripDriver::progressDirection(Direction direction) {
  mProgressDirection = direction;
  return this;
};

void LedStripDriver::cancelWindDirectionTransition() {
  mWindDirTransition = false;
}

LedStripDriver* LedStripDriver::resetState() {
  mResetState = true;
  return this;
}

/* Used by progress pattern to set inital progress value */
LedStripDriver* LedStripDriver::initialValue(uint8_t progress) {
  mProgressInitial = progress;
  return this;
};

/* Used by progress pattern to set number of LEDs per increment */
LedStripDriver* LedStripDriver::increment(uint8_t leds) {
  mProgressIncrement = leds;
  return this;
};

/* Used by progress pattern to set number of ms between increments */
LedStripDriver* LedStripDriver::incDelay(uint32_t delayMs) {
  mProgressIncrementDelayMs = delayMs;
  return this;
};

/* Used by progress pattern to set number of ms between patterns */
LedStripDriver* LedStripDriver::resetDelay(uint32_t delayMs) {
  mProgressResetDelayMs = delayMs;
  return this;
};

/* Used by progress pattern to set final progress value */
LedStripDriver* LedStripDriver::finalValue(uint8_t progress) {
  mProgressFinal = progress;
  return this;
};

/* Used by weather pattern to set temperature fade interval (secs) */
LedStripDriver* LedStripDriver::tempFadeInterval(uint32_t intervalSecs) {
  mWeatherTempFadeIntervalSecs = intervalSecs;
  return this;
};

/* Used by weather pattern to set rain band height */
LedStripDriver* LedStripDriver::rainBandHeight(uint8_t leds) {
  mWeatherRainBandHeightLeds = leds;
  return this;
}

/* Used by weather pattern to set the delay between band movements */
LedStripDriver* LedStripDriver::rainBandIncrementDelay(uint32_t delayMs) {
  mWeatherRainBandIncDelayMs = delayMs;
  return this;
}

/* Used by weather pattern to set rain band spacing (# leds apart) */
LedStripDriver* LedStripDriver::rainBandSpacing(uint8_t leds) {
  mWeatherRainBandSpacingLeds = leds;
  return this;
}

/* Used by weather pattern to set rain band colour */
LedStripDriver* LedStripDriver::rainBandColour(Colour *colour) {
  mWeatherRainBandColour = colour;
  return this;
}

LedStripDriver* LedStripDriver::rainDirection(Direction direction) {
  mWeatherRainDirection = direction;
  return this;
}

/* Used by weather pattern to set weather warning colour */
LedStripDriver* LedStripDriver::warningColour(Colour *colour) {
  mWeatherWarningColour = colour;
  return this;
}

/* Used by weather pattern to set weather warning fade in time (ms) */
LedStripDriver* LedStripDriver::warningFadeIn(uint32_t fadeTimeMs) {
  mWeatherWarningFadeInMs = fadeTimeMs;
  return this;
}

/* Used by weather pattern to set weather warning fade in time (ms) */
LedStripDriver* LedStripDriver::warningFadeOut(uint32_t fadeTimeMs) {
  mWeatherWarningFadeOutMs = fadeTimeMs;
  return this;
}

/* Used by weather pattern to set weather warning off dwell time (ms) */
LedStripDriver* LedStripDriver::warningOffDwell(uint32_t offDwellMs) {
  mWeatherWarningOffDwellMs = offDwellMs;
  return this;
}

/* Used by wind pattern to set colour */
LedStripDriver* LedStripDriver::windDirectionColour(Colour *colour) {
  mWindDirectionColour = colour;
  return this;
}

/* Used by wind pattern to fade to new wind direction colour */
LedStripDriver* LedStripDriver::prevWindDirectionColour(Colour *colour) {
  mPrevWindDirectionColour = colour;
  return this;
}

/* Used by wind pattern to set timeout for speed layer */
LedStripDriver* LedStripDriver::windSpeedTimeout(uint32_t timeoutMs) {
  mWindSpeedTimeoutMs = timeoutMs;
  return this;
}

LedStripDriver* LedStripDriver::showWindSpeed(led_strip_state_t *state, uint32_t timeoutMs) {
  state->windSpeedTransition = true;
  state->counter = 0;
  state->fadeDirection = 1;
  state->timeoutCounter = 0;
  mCurrentWindPattern = speed;
  mWindSpeedTimeoutMs = timeoutMs;
  // mPatternTimeoutMs = timeoutMs;

  return this;
}

LedStripDriver* LedStripDriver::activeLayer(uint32_t layer) {
  mActiveLayer = layer;
  return this;
}

LedStripDriver* LedStripDriver::layer2Colour1(Colour *colour) {
  mLayer2Colour1 = colour;
  return this;
}

LedStripDriver* LedStripDriver::layer2Colour2(Colour *colour) {
  mLayer2Colour2 = colour;
  return this;
}
