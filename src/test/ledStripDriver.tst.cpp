#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include <cstring>

#include "StringFrom.h"
#include "colour.h"
#include "colours.h"
#include "ledStripDriver.h"
#include "config.h"

#define MAX_LEDS 10

const Colour COLOUR_ON = COLOUR_RED;
const Colour COLOUR_OFF = COLOUR_GREEN;

static uint8_t values[MAX_LEDS * COLOURS_PER_LED];
static uint8_t *lastValuesWritten;

static void writeValueStub(uint8_t *values, uint32_t length) {
  memcpy(lastValuesWritten, values, length);
}

static LedStripDriver *driver;

static const led_strip_config_t CONFIG_LEDS_1 = {
  .numLeds = 1,
  .writeValueFn = writeValueStub,
  .resolutionMs = 1,
};

static const led_strip_config_t CONFIG_LEDS_3 = {
  .numLeds = 3,
  .writeValueFn = writeValueStub,
  .resolutionMs = 1,
};

static const led_strip_config_t CONFIG_LEDS_6 = {
  .numLeds = 6,
  .writeValueFn = writeValueStub,
  .resolutionMs = 1,
};

#define WEATHER_TEST_LED_CONFIG CONFIG_LEDS_6
#define WIND_TEST_LED_CONFIG CONFIG_LEDS_3
#define AIR_TEST_LED_CONFIG CONFIG_LEDS_3

static void verify_colours(Colour *expected,
                           uint8_t *values,
                           uint32_t len,
                           uint32_t offset = 0) {
  for (uint32_t i=offset; i<len; i++) {
    uint32_t index = i * COLOURS_PER_LED;
    Colour actual = Colour(values[index+INDEX_RED],
                           values[index+INDEX_GREEN],
                           values[index+INDEX_BLUE]);
    String message;

    message = message.format("Incorrect colour at index %d, expected: %s, actual: %s ",
                             index,
                             expected->toString().c_str(),
                             actual.toString().c_str());

    CHECK_TEXT(*expected == actual, message);
  }
}

static void verify_colour(Colour *expected, uint8_t *values) {
  verify_colours(expected, values, 1);
}

/***********************************************************************************************
 * Common code
 **********************************************************************************************/

TEST_GROUP(LedStripDriverCommonTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverCommonTestGroup, counterIncrementsByResolution)
{
  led_strip_state_t state = { .counter = 0 };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(CONFIG_LEDS_3.resolutionMs, state.counter);
}

/***********************************************************************************************
 * Pulse pattern
 **********************************************************************************************/

TEST_GROUP(LedStripDriverPulseTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverPulseTestGroup, writesCorrectInitialValues)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;

  driver->pattern(Pattern::pulse)
    ->period(3)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END);

  led_strip_state_t state = {
    .counter = 0,
    .dutyDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_START, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverPulseTestGroup, writesCorrectStepValues)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour COLOUR_MID = Colour(127, 127, 0);

  driver->pattern(Pattern::pulse)
    ->period(3000)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END);

  led_strip_state_t state = {
    .counter = 1500,
    .dutyDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverPulseTestGroup, writesCorrectFinalValues)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;

  driver->pattern(Pattern::pulse)
    ->period(3)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END);

  led_strip_state_t state = {
    .counter = 2,
    .dutyDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_END, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverPulseTestGroup, resetsCounterAndReversesDirection)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;

  driver->pattern(Pattern::pulse)
    ->period(3)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END);

  led_strip_state_t state = {
    .counter = 3,
    .dutyDirection = 1,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.counter);
  LONGS_EQUAL(-1, state.dutyDirection);
}

TEST(LedStripDriverPulseTestGroup, writesCorrectValuesForReverseDirection)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;

  driver->pattern(Pattern::pulse)
    ->period(3000)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END);

  led_strip_state_t state = {
    .counter = 0,
    .dutyDirection = -1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_END, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverPulseTestGroup, writesCorrectValuesWhenDirectionReversed)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;

  driver->pattern(Pattern::pulse)
    ->period(3000)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END);

  led_strip_state_t state = {
    .counter = 3000,
    .dutyDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_END, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

/***********************************************************************************************
 * Blink pattern
 **********************************************************************************************/
TEST_GROUP(LedStripDriverBlinkTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverBlinkTestGroup, writesOnValueForAllLedsWithCorrectColour)
{
  driver->pattern(Pattern::blink)
    ->period(10)
    ->dutyCycle(50)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_ON, lastValuesWritten, 3);
}

TEST(LedStripDriverBlinkTestGroup, writesOffValueForAllLedsWithCorrectColourAfterPeriod)
{
  driver->pattern(Pattern::blink)
    ->period(10)
    ->dutyCycle(50)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 5,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_OFF, lastValuesWritten, 3);
}

TEST(LedStripDriverBlinkTestGroup, resetsCounterAfterPeriod)
{
  const uint32_t PERIOD_MS = 10;

  driver->pattern(Pattern::blink)
    ->period(PERIOD_MS)
    ->dutyCycle(50)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = PERIOD_MS,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.counter);
}

/***********************************************************************************************
 * Colour pattern
 **********************************************************************************************/
TEST_GROUP(LedStripDriverColourTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverColourTestGroup, writesValueForAllLedsWithCorrectColour)
{
  const Colour& COLOUR = COLOUR_RED;

  driver->pattern(Pattern::colour)
    ->colourOn((Colour*)&COLOUR);

  led_strip_state_t state = {
    .counter = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR, lastValuesWritten, 3);
}

/***********************************************************************************************
 * Strobe pattern
 **********************************************************************************************/
TEST_GROUP(LedStripDriverStrobeTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverStrobeTestGroup, writesOnValueForAllLedsWithCorrectColour)
{
  driver->pattern(Pattern::strobe)
    ->period(10)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_ON, lastValuesWritten, 3);
}

TEST(LedStripDriverStrobeTestGroup, writesOffValueForAllLedsWithCorrectColour)
{
 const uint32_t PERIOD_MS = 10;

  driver->pattern(Pattern::strobe)
    ->period(PERIOD_MS)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = PERIOD_MS / 2,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_OFF, lastValuesWritten, 3);
}

TEST(LedStripDriverStrobeTestGroup, counterResetAfterPeriod)
{
 const uint32_t PERIOD_MS = 10;

  driver->pattern(Pattern::strobe)
    ->period(PERIOD_MS)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = PERIOD_MS,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.counter);
}

/***********************************************************************************************
 * Progress pattern
 **********************************************************************************************/
TEST_GROUP(LedStripDriverProgressTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverProgressTestGroup, writesCorrectInitialValue)
{
  driver->pattern(Pattern::progress)
    ->initialValue(1)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_ON, lastValuesWritten);
  verify_colours((Colour*)&COLOUR_OFF, &lastValuesWritten[COLOURS_PER_LED], 2);
}

TEST(LedStripDriverProgressTestGroup, incrementsProgressValueAfterDelay)
{
  const uint32_t DELAY_MS = 10;

  driver->pattern(Pattern::progress)
    ->incDelay(DELAY_MS);

  led_strip_state_t state = {
    .counter = DELAY_MS,
    .progress = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.progress);
}

TEST(LedStripDriverProgressTestGroup, resetsCounterAfterDelay)
{
  const uint32_t DELAY_MS = 10;

  driver->pattern(Pattern::progress)
    ->incDelay(DELAY_MS);

  led_strip_state_t state = {
    .counter = DELAY_MS,
    .progress = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.counter);
}

TEST(LedStripDriverProgressTestGroup, writesCorrectValueForForwardDirection)
{
  const uint32_t PROGRESS = 2;

  driver->pattern(Pattern::progress)
    ->initialValue(0)
    ->progressDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
    .progress = PROGRESS,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_ON, lastValuesWritten);
  verify_colours((Colour*)&COLOUR_OFF, &lastValuesWritten[PROGRESS * COLOURS_PER_LED], 1);
}

TEST(LedStripDriverProgressTestGroup, resetsProgressAfterFinalValueAndResetDelay)
{
  const uint32_t FINAL = 2;
  const uint32_t DELAY_MS = 10;
  const uint32_t RESET_DELAY_MS = 3;

  driver->pattern(Pattern::progress)
    ->finalValue(FINAL)
    ->incDelay(DELAY_MS)
    ->resetDelay(RESET_DELAY_MS)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = DELAY_MS + RESET_DELAY_MS,
    .progress = FINAL,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.progress);
}

TEST(LedStripDriverProgressTestGroup, doesNotUpdateProgressAfterAllLedsOnButBeforeResetDelay)
{
  const uint32_t INITIAL = 1;
  const uint32_t PROGRESS = CONFIG_LEDS_3.numLeds;
  const uint32_t DELAY_MS = 10;
  const uint32_t RESET_DELAY_MS = 3;

  driver->pattern(Pattern::progress)
    ->initialValue(INITIAL)
    ->incDelay(DELAY_MS)
    ->resetDelay(RESET_DELAY_MS)
    ->progressDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = DELAY_MS,
    .progress = PROGRESS,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(PROGRESS, state.progress);
}

TEST(LedStripDriverProgressTestGroup, writesCorrectValueForReverseDirection)
{
 const uint32_t PROGRESS = 2;

  driver->pattern(Pattern::progress)
    ->initialValue(0)
    ->progressDirection(Direction::reverse)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
    .progress = PROGRESS,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_OFF, lastValuesWritten);
  verify_colours((Colour*)&COLOUR_ON, &lastValuesWritten[COLOURS_PER_LED], 2);
}

TEST(LedStripDriverProgressTestGroup, allLedsOffIfFinalValueLessThanMaxLeds)
{
  const uint32_t FINAL = 1;
  const uint32_t DELAY_MS = 10;
  const uint32_t RESET_DELAY_MS = 3;

  driver->pattern(Pattern::progress)
    ->finalValue(FINAL)
    ->incDelay(DELAY_MS)
    ->resetDelay(RESET_DELAY_MS)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = DELAY_MS + RESET_DELAY_MS,
    .progress = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_OFF, &lastValuesWritten[0], 3);
}

TEST(LedStripDriverProgressTestGroup, incrementsByGivenAmount)
{
  const uint32_t INCREMENT = 2;
  const uint32_t INCREMENT_DELAY = 100;

  driver->pattern(Pattern::progress)
    ->initialValue(0)
    ->increment(INCREMENT)
    ->incDelay(INCREMENT_DELAY)
    ->progressDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = INCREMENT_DELAY,
    .progress = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(INCREMENT, state.progress);
}

/***********************************************************************************************
 * Gradient pattern
 **********************************************************************************************/
TEST_GROUP(LedStripDriverGradientTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverGradientTestGroup, writesCorrectValues)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour COLOUR_MID = Colour(127, 127, 0);

  driver->pattern(Pattern::gradient)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END);

  led_strip_state_t state = {
    .counter = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_START, lastValuesWritten);
  verify_colour((Colour*)&COLOUR_MID, &lastValuesWritten[COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_END, &lastValuesWritten[2 * COLOURS_PER_LED]);
}

/***********************************************************************************************
 * Snake pattern
 **********************************************************************************************/
TEST_GROUP(LedStripDriverSnakeTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverSnakeTestGroup, writesCorrectValuesForInitialState)
{
  driver->pattern(Pattern::snake)
    ->length(2)
    ->snakeDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
    .progress = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_OFF, lastValuesWritten, 3);
}

TEST(LedStripDriverSnakeTestGroup, incrementsProgressAtCorrectCounterValue)
{
  driver->pattern(Pattern::snake)
    ->period(1000)
    ->length(2)
    ->snakeDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 200,
    .progress = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.progress);
}

TEST(LedStripDriverSnakeTestGroup, doesNotIncrementProgressForEveryCounterValue)
{
  driver->pattern(Pattern::snake)
    ->period(1000)
    ->length(2)
    ->snakeDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
    .progress = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.progress);
}

TEST(LedStripDriverSnakeTestGroup, resetsCounterAtCorrectValue)
{
  driver->pattern(Pattern::snake)
    ->period(1000)
    ->length(2)
    ->snakeDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 200,
    .progress = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.counter);
}

TEST(LedStripDriverSnakeTestGroup, writesCorrectValuesForForward)
{
  driver->pattern(Pattern::snake)
    ->period(1000)
    ->length(2)
    ->snakeDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
    .progress = 2,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_ON, lastValuesWritten, 2);
  verify_colours((Colour*)&COLOUR_OFF, lastValuesWritten, 1, 2);
}

TEST(LedStripDriverSnakeTestGroup, writesCorrectValuesForReverse)
{
  driver->pattern(Pattern::snake)
    ->period(1000)
    ->length(2)
    ->snakeDirection(Direction::reverse)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
    .progress = 2,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_OFF, lastValuesWritten, 1);
  verify_colours((Colour*)&COLOUR_ON, lastValuesWritten, 2, 1);
}

TEST(LedStripDriverSnakeTestGroup, writesCorrectValuesWhenProgressLessThanLengthForward)
{
  driver->pattern(Pattern::snake)
    ->period(1000)
    ->length(4)
    ->snakeDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
    .progress = 2,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_ON, lastValuesWritten, 2);
  verify_colours((Colour*)&COLOUR_OFF, lastValuesWritten, 1, 2);
}

TEST(LedStripDriverSnakeTestGroup, writesCorrectValuesWhenProgressMoreThanNumLedsReverse)
{
  driver->pattern(Pattern::snake)
    ->period(1000)
    ->length(2)
    ->snakeDirection(Direction::reverse)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 0,
    .progress = 4,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_ON, lastValuesWritten, 1);
  verify_colours((Colour*)&COLOUR_OFF, lastValuesWritten, 2, 1);
}

TEST(LedStripDriverSnakeTestGroup, resetsProgressAfterSnakeNoLongerVisible)
{
 const uint32_t LENGTH = 2;

  driver->pattern(Pattern::snake)
    ->period(1000)
    ->length(LENGTH)
    ->snakeDirection(Direction::forward)
    ->colourOn((Colour*)&COLOUR_ON)
    ->colourOff((Colour*)&COLOUR_OFF);

  led_strip_state_t state = {
    .counter = 200,
    .progress = CONFIG_LEDS_3.numLeds + LENGTH,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.progress);
}

/***********************************************************************************************
 * Weather pattern
 **********************************************************************************************/
TEST_GROUP(LedStripDriverWeatherTestGroup)
{
  void setup() {
    const uint32_t valuesLength = WEATHER_TEST_LED_CONFIG.numLeds * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&WEATHER_TEST_LED_CONFIG);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, MAX_LEDS * COLOURS_PER_LED);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverWeatherTestGroup, writesCorrectInitialValuesForTemperatureLayerForwardDirection)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_ON, lastValuesWritten, WEATHER_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectMidValuesForTemperatureLayerForwardDirection)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_MID = Colour(127, 127, 0);
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS);

  led_strip_state_t state = {
    .counter = (TEMPERATURE_FADE_INTERVAL_SECS * 1000) / 2,
    .fadeDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, WEATHER_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectEndValuesForTemperatureLayerForwardDirection)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS);

  led_strip_state_t state = {
    .counter = (TEMPERATURE_FADE_INTERVAL_SECS * 1000) - WEATHER_TEST_LED_CONFIG.resolutionMs,
    .fadeDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_END, lastValuesWritten, WEATHER_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWeatherTestGroup, resetsCounter)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS);

  led_strip_state_t state = {
    .counter = (TEMPERATURE_FADE_INTERVAL_SECS * 1000),
    .fadeDirection = 1,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.counter);
}

TEST(LedStripDriverWeatherTestGroup, changesFadeDirectionWhenCounterReset)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS);

  led_strip_state_t state = {
    .counter = (TEMPERATURE_FADE_INTERVAL_SECS * 1000),
    .fadeDirection = -1,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.fadeDirection);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForEndTemperatureLayerBackwardDirection)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS);

  led_strip_state_t state = {
    .counter = (TEMPERATURE_FADE_INTERVAL_SECS * 1000),
    .fadeDirection = -1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,

    .weatherWarningCounter = 0,
    .weatherWarningFadeState = fadeIn,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_START, lastValuesWritten, WEATHER_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForInitialRainCounter)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval((uint32_t)TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(500)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[0]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[1 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[2 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[3 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[4 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[5 * COLOURS_PER_LED]);
}

TEST(LedStripDriverWeatherTestGroup, incrementsRainCounterCorrectly)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(WEATHER_TEST_LED_CONFIG.resolutionMs, state.weatherRainCounter);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForIncrementedRainPosition)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[0]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[1 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[2 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[3 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[4 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[5 * COLOURS_PER_LED]);
}

TEST(LedStripDriverWeatherTestGroup, incrementsRainPositionCorrectly)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = RAIN_INC_DELAY_MS - 1,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.weatherRainPosition);
}

TEST(LedStripDriverWeatherTestGroup, resetsRainCounterCorrectly)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = RAIN_INC_DELAY_MS - 1,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.weatherRainCounter);
}

TEST(LedStripDriverWeatherTestGroup, resetsRainPositionCorrectly)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = RAIN_INC_DELAY_MS - 1,
    .weatherRainPosition = (uint8_t)(WEATHER_TEST_LED_CONFIG.numLeds - 1),
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.weatherRainPosition);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesWhenRainBandsWrapAround)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 2,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[0]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[1 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[2 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[3 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[4 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[5 * COLOURS_PER_LED]);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForRainBandsReverseDirection)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(2)
    ->rainDirection(Direction::reverse)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[0]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[1 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[2 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[3 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[4 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[5 * COLOURS_PER_LED]);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForRainBandsIncrementedReverseDirection)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(2)
    ->rainDirection(Direction::reverse)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[0]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[1 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[2 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[3 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[4 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[5 * COLOURS_PER_LED]);
}

TEST(LedStripDriverWeatherTestGroup, usesCorrectColourForRainBands)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_BLUE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[0]);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForIncreasedRainBandHeight)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;
  const uint8_t RAIN_BAND_HEIGHT_LEDS = 3;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(RAIN_BAND_HEIGHT_LEDS)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[0 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[1 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[2 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[3 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[4 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[5 * COLOURS_PER_LED]);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForIncreasedRainBandSpacing)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;
  const uint8_t RAIN_BAND_HEIGHT_LEDS = 1;
  const uint8_t RAIN_BAND_SPACING_LEDS = 3;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(RAIN_BAND_HEIGHT_LEDS)
    ->rainBandSpacing(RAIN_BAND_SPACING_LEDS)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[0]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[1 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[2 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[3 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_RAIN, &lastValuesWritten[4 * COLOURS_PER_LED]);
  verify_colour((Colour*)&COLOUR_START, &lastValuesWritten[5 * COLOURS_PER_LED]);
}

TEST(LedStripDriverWeatherTestGroup, doesNotIncrementsRainPositionIfCounterLessThanIncDelay)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(1)
    ->rainBandSpacing(1)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = RAIN_INC_DELAY_MS - 2,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.weatherRainPosition);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForWeatherWarningInitialState)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_BLUE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = 0,
    .weatherWarningFadeState = fadeIn,
  };

  driver->onTimerFired(&state, values);

  //'transparent' at first
  verify_colours((Colour*)&COLOUR_START, lastValuesWritten, 6);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForWeatherWarningMidFadeInState)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const Colour COLOUR_MID = Colour(127, 127, 127);
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = WARNING_FADE_IN_MS/2 - 1,
    .weatherWarningFadeState = fadeIn,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, 6);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForWeatherWarningEndOfFadeInState)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = WARNING_FADE_IN_MS - 2,
    .weatherWarningFadeState = fadeIn,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_WARNING, lastValuesWritten, 6);
}

TEST(LedStripDriverWeatherTestGroup, incrementsWeatherWarningCounter)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = 0,
    .weatherWarningFadeState = fadeIn,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(WEATHER_TEST_LED_CONFIG.resolutionMs, state.weatherWarningCounter);
}

TEST(LedStripDriverWeatherTestGroup, resetsWeatherWarningCounterAfterFadeIn)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = WARNING_FADE_IN_MS-1,
    .weatherWarningFadeState = fadeIn,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.weatherWarningCounter);
}

TEST(LedStripDriverWeatherTestGroup, reversesWeatherWarningFadeDirectionAfterFadeIn)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = WARNING_FADE_IN_MS - WEATHER_TEST_LED_CONFIG.resolutionMs,
    .weatherWarningFadeState = fadeIn,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(fadeOut, state.weatherWarningFadeState);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForWeatherWarningMidFadeOutState)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const Colour COLOUR_MID = Colour(127, 127, 127);
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = WARNING_FADE_OUT_MS/2 - WEATHER_TEST_LED_CONFIG.resolutionMs,
    .weatherWarningFadeState = fadeOut,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, 6);
}

TEST(LedStripDriverWeatherTestGroup, writesCorrectValuesForWeatherWarningDwellTime)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = WARNING_OFF_DWELL_MS - (2 * WEATHER_TEST_LED_CONFIG.resolutionMs),
    .weatherWarningFadeState = offDwell,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_START, lastValuesWritten, 6);
}

TEST(LedStripDriverWeatherTestGroup, setsWarningFadeStateToFadeInAfterOffDwell)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = WARNING_OFF_DWELL_MS -  WEATHER_TEST_LED_CONFIG.resolutionMs,
    .weatherWarningFadeState = offDwell,
  };

  driver->onTimerFired(&state, values);

  CHECK_EQUAL(fadeIn, state.weatherWarningFadeState);
}

TEST(LedStripDriverWeatherTestGroup, doesNotDrawRainBandsPastEndOfLedStrip)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const Colour& COLOUR_INIT = COLOUR_BLACK;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;
  const uint8_t RAIN_BAND_HEIGHT_LEDS = 3;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(RAIN_BAND_HEIGHT_LEDS)
    ->rainBandSpacing(2)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  // last must be unchanged from init
  const int INDEX_OVERFLOW = WEATHER_TEST_LED_CONFIG.numLeds * COLOURS_PER_LED;
  verify_colour((Colour*)&COLOUR_INIT, &values[INDEX_OVERFLOW]);
}

TEST(LedStripDriverWeatherTestGroup, doesNotDrawRainBandsPastEndOfLedStripReverseDirection)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_RAIN = COLOUR_WHITE;
  const Colour& COLOUR_INIT = COLOUR_BLACK;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t RAIN_INC_DELAY_MS = 500;
  const uint8_t RAIN_BAND_HEIGHT_LEDS = 3;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->rainBandHeight(RAIN_BAND_HEIGHT_LEDS)
    ->rainBandSpacing(2)
    ->rainDirection(Direction::reverse)
    ->rainBandIncrementDelay(RAIN_INC_DELAY_MS)
    ->rainBandColour((Colour*)&COLOUR_RAIN);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
  };

  driver->onTimerFired(&state, values);

  // last must be unchanged from init
  const int INDEX_OVERFLOW = WEATHER_TEST_LED_CONFIG.numLeds * COLOURS_PER_LED;
  verify_colour((Colour*)&COLOUR_INIT, &values[INDEX_OVERFLOW]);
}

TEST(LedStripDriverWeatherTestGroup, warningUsesProvidedColour)
{
  const Colour& COLOUR_START = COLOUR_RED;
  const Colour& COLOUR_END = COLOUR_GREEN;
  const Colour& COLOUR_WARNING = COLOUR_BLUE;
  const uint32_t TEMPERATURE_FADE_INTERVAL_SECS = 4;
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  driver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(TEMPERATURE_FADE_INTERVAL_SECS)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(WARNING_FADE_IN_MS)
    ->warningFadeOut(WARNING_FADE_OUT_MS)
    ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .weatherRainCounter = 0,
    .weatherRainPosition = 0,
    .weatherWarningCounter = WARNING_FADE_IN_MS,
    .weatherWarningFadeState = fadeIn,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_WARNING, lastValuesWritten, WEATHER_TEST_LED_CONFIG.numLeds);
}

/***********************************************************************************************
 * Initial states
 **********************************************************************************************/
TEST_GROUP(LedStripDriverInitStateTestGroup)
{
  void setup() {
  }

  void teardown() {
  }
};

TEST(LedStripDriverInitStateTestGroup, initialisesCounter)
{
  led_strip_state_t state;
  LedStripDriver driver((led_strip_config_t*)&CONFIG_LEDS_1);
  driver.initState(&state);

  LONGS_EQUAL(0, state.counter);
}

TEST(LedStripDriverInitStateTestGroup, initialisesDutyCycle)
{
  led_strip_state_t state;
  LedStripDriver driver((led_strip_config_t*)&CONFIG_LEDS_1);
  driver.initState(&state);

  DOUBLES_EQUAL(1, state.dutyCycle, 0.00001);
}

TEST(LedStripDriverInitStateTestGroup, initialisesDutyDirection)
{
  led_strip_state_t state;
  LedStripDriver driver((led_strip_config_t*)&CONFIG_LEDS_1);
  driver.initState(&state);

  LONGS_EQUAL(1, state.dutyDirection);
}

TEST(LedStripDriverInitStateTestGroup, initialisesWeatherTempFadeDirection)
{
  led_strip_state_t state;
  LedStripDriver driver((led_strip_config_t*)&CONFIG_LEDS_1);
  driver.initState(&state);

  CHECK_EQUAL(1, state.fadeDirection);
}

TEST(LedStripDriverInitStateTestGroup, initialisesWeatherRainCounter)
{
  led_strip_state_t state;
  LedStripDriver driver((led_strip_config_t*)&CONFIG_LEDS_1);
  driver.initState(&state);

  LONGS_EQUAL(0, state.weatherRainCounter);
}

TEST(LedStripDriverInitStateTestGroup, initialisesWeatherRainPosition)
{
  led_strip_state_t state;
  LedStripDriver driver((led_strip_config_t*)&CONFIG_LEDS_1);
  driver.initState(&state);

  CHECK_EQUAL(0, state.weatherRainPosition);
}

TEST(LedStripDriverInitStateTestGroup, initialisesWeatherWarningCounter)
{
  led_strip_state_t state;
  LedStripDriver driver((led_strip_config_t*)&CONFIG_LEDS_1);
  driver.initState(&state);

  LONGS_EQUAL(0, state.weatherWarningCounter);
}

TEST(LedStripDriverInitStateTestGroup, initialisesWeatherWarningFadeState)
{
  led_strip_state_t state;
  LedStripDriver driver((led_strip_config_t*)&CONFIG_LEDS_1);
  driver.initState(&state);

  LONGS_EQUAL(fadeIn, state.weatherWarningFadeState);
}

/***********************************************************************************************
 * Default Loop pattern
 **********************************************************************************************/

TEST_GROUP(LedStripDriverDefaultLoopTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverDefaultLoopTestGroup, writesCorrectStartValues)
{
  const Colour& COLOUR_START = COLOUR_DL_YELLOW;

  driver->pattern(Pattern::defaultLoop)
        ->period(3);

  led_strip_state_t state = {
    .counter = 0,
    .defaultLoopCurColIdx = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_START, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverDefaultLoopTestGroup, writesCorrectMiddleValues)
{
  const Colour& COLOUR_MID = Colour(127, 242, 0);

  driver->pattern(Pattern::defaultLoop)
        ->period(3);

  led_strip_state_t state = {
    .counter = 1,
    .defaultLoopCurColIdx = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverDefaultLoopTestGroup, writesCorrectEndValues)
{
  const Colour& COLOUR_END = COLOUR_DL_GREEN;

  driver->pattern(Pattern::defaultLoop)
        ->period(3);

  led_strip_state_t state = {
    .counter = 2,
    .defaultLoopCurColIdx = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_END, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverDefaultLoopTestGroup, displaysCorrectStartColourForIndex1)
{
  const Colour& COLOUR_START_1 = COLOUR_DL_GREEN;

  driver->pattern(Pattern::defaultLoop)
        ->period(3);

  led_strip_state_t state = {
    .counter = 0,
    .defaultLoopCurColIdx = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_START_1, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverDefaultLoopTestGroup, displaysCorrectEndColourForIndex1)
{
  const Colour& COLOUR_END_1 = COLOUR_DL_BLUE;

  driver->pattern(Pattern::defaultLoop)
        ->period(3);

  led_strip_state_t state = {
    .counter = 2,
    .defaultLoopCurColIdx = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_END_1, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverDefaultLoopTestGroup, displaysCorrectStartColourForIndex2)
{
  const Colour& COLOUR_START_2 = COLOUR_DL_BLUE;

  driver->pattern(Pattern::defaultLoop)
        ->period(3);

  led_strip_state_t state = {
    .counter = 0,
    .defaultLoopCurColIdx = 2,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_START_2, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverDefaultLoopTestGroup, displaysCorrectEndColourForIndex2)
{
  const Colour& COLOUR_END_2 = COLOUR_DL_RED;

  driver->pattern(Pattern::defaultLoop)
        ->period(3);

  led_strip_state_t state = {
    .counter = 2,
    .defaultLoopCurColIdx = 2,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_END_2, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverDefaultLoopTestGroup, displaysCorrectStartColourForIndex3)
{
  const Colour& COLOUR_START_3 = COLOUR_DL_RED;

  driver->pattern(Pattern::defaultLoop)
        ->period(3);

  led_strip_state_t state = {
    .counter = 0,
    .defaultLoopCurColIdx = 3,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_START_3, lastValuesWritten, CONFIG_LEDS_3.numLeds);
}

TEST(LedStripDriverDefaultLoopTestGroup, resetsCounter)
{
  const uint32_t PERIOD_MS = 3;

  driver->pattern(Pattern::defaultLoop)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS,
    .defaultLoopCurColIdx = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.counter);
}

TEST(LedStripDriverDefaultLoopTestGroup, incrementsColourIndexWhenCounterResets)
{
  const uint32_t PERIOD_MS = 3;

  driver->pattern(Pattern::defaultLoop)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS,
    .defaultLoopCurColIdx = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.defaultLoopCurColIdx);
}

TEST(LedStripDriverDefaultLoopTestGroup, resetsColourIndex)
{
  const uint32_t PERIOD_MS = 3;

  driver->pattern(Pattern::defaultLoop)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS,
    .defaultLoopCurColIdx = DL_NUM_COL,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.defaultLoopCurColIdx);
}

/***********************************************************************************************
 * Wind pattern
 **********************************************************************************************/

TEST_GROUP(LedStripDriverWindTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&CONFIG_LEDS_3);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverWindTestGroup, writesCorrectStartValues)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .windSpeedTransition = false,
  };

  driver->cancelWindDirectionTransition(); // not testing transition here
  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_WIND_DIR, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, showsCorrectStartWindSpeedColourWhenCommanded)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .fadeDirection = 1,
  };

  driver->showWindSpeed(&state, 3);
  state.counter = 2;

  driver->onTimerFired(&state, values);

  //should fade from direction colour to first speed colour
  verify_colours((Colour*)&COLOUR_WIND_SPD_1, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, showsCorrectEndWindSpeedColourAfterTransition)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .fadeDirection = 1,
  };

  driver->showWindSpeed(&state, 3);
  state.counter = 2;
  state.windSpeedTransition = false; // transition finished

  driver->onTimerFired(&state, values);

  //should fade from direction colour to first speed colour
  verify_colours((Colour*)&COLOUR_WIND_SPD_2, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, resetsCounterWhenWindSpeedTriggered)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .counter = 1,
    .fadeDirection = 1,
    .windSpeedTransition = false,
  };

  driver->showWindSpeed(&state, 3);

  LONGS_EQUAL_TEXT(0, state.counter, "Counter not cleared when showWindSpeed() triggered!");
}

TEST(LedStripDriverWindTestGroup, clearsWindSpeedTransitionAfterTimeout)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .fadeDirection = 1,
  };

  driver->showWindSpeed(&state, 3);
  state.counter = 3;

  driver->onTimerFired(&state, values);

  CHECK_FALSE_TEXT(state.windSpeedTransition, "windSpeedTransition not cleared after timeout!");
}

TEST(LedStripDriverWindTestGroup, resetsCounterAfterWindSpeedTransitionTimeout)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .fadeDirection = 1,
  };

  driver->showWindSpeed(&state, 3);
  state.counter = 3;

  driver->onTimerFired(&state, values);

  LONGS_EQUAL_TEXT(1, state.counter, "Counter not cleared after transition timeout!");
}

TEST(LedStripDriverWindTestGroup, reversesFadeDirectionAfterPositiveTimeout)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .fadeDirection = 1,
  };

  driver->showWindSpeed(&state, 3);
  state.windSpeedTransition = false; // Transition already finished
  state.counter = 3;

  driver->onTimerFired(&state, values);

  LONGS_EQUAL_TEXT(-1, state.fadeDirection, "fade direction not changed after timeout!");
}

TEST(LedStripDriverWindTestGroup, doesntReverseFadeDirectionAfterTransitionTimeout)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .fadeDirection = 1,
  };

  driver->showWindSpeed(&state, 3);
  state.counter = 3;

  driver->onTimerFired(&state, values);

  LONGS_EQUAL_TEXT(1, state.fadeDirection, "fade direction incorrectly reversed after timeout!");
}

TEST(LedStripDriverWindTestGroup, showsCorrectEndColourForReverseDirection)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->windSpeedTimeout(3000)
        ->period(3);

  led_strip_state_t state = {
    .windSpeedTimeoutCounter = 0,
  };

  driver->showWindSpeed(&state, 3);
  state.windSpeedTransition = false; // Transition already finished
  state.counter = 2;
  state.fadeDirection = -1;

  driver->onTimerFired(&state, values);

  //should fade from direction colour to first speed colour
  verify_colours((Colour*)&COLOUR_WIND_SPD_1, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, setsStateCorrectlyAfterSpeedLayerTimeout)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const uint32_t SPEED_TIMEOUT_MS = 30000;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->windSpeedTimeout(SPEED_TIMEOUT_MS)
        ->period(3);

  led_strip_state_t state = {
    .fadeDirection = 1,
    .windSpeedTimeoutCounter = SPEED_TIMEOUT_MS,
  };

  driver->showWindSpeed(&state, 3);
  state.windSpeedTransition = false; // Transition already finished
  state.counter = 0;

  driver->onTimerFired(&state, values);

  CHECK_TRUE_TEXT(state.windSpeedTransition, "windSpeedTransition not set after timeout!");
  LONGS_EQUAL_TEXT(1, state.fadeDirection, "fadeDirection not set correctly after timeout!");
  LONGS_EQUAL_TEXT(1, state.counter, "counter not reset after pattern timeout!");
  CHECK_EQUAL_TEXT(WindPattern::direction, driver->getCurrentWindPattern(), "wind pattern not changed after timeout!");
  LONGS_EQUAL_TEXT(0, driver->getPatternTimeout(), "pattern timeout not cleared after timeout!");
}

TEST(LedStripDriverWindTestGroup, showsCorrectStartColoursForTransitionAfterTimeout)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->windSpeedTimeout(PATTERN_TIMEOUT_MS)
        ->period(3);

  led_strip_state_t state = {
    .fadeDirection = 1,
    .windSpeedTimeoutCounter = PATTERN_TIMEOUT_MS,
  };

  driver->showWindSpeed(&state, 3);
  state.windSpeedTransition = false; // Transition already finished
  state.counter = 1;

  driver->onTimerFired(&state, values);

  //should fade from direction colour to first speed colour
  verify_colours((Colour*)&COLOUR_WIND_SPD_2, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, showsCorrectEndColoursForTransitionAfterTimeout)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->timeout(0) // reset after timeout
        ->period(3);

  led_strip_state_t state = {
    .counter = 3,
    .fadeDirection = 1,
    .windSpeedTransition = true,
    .timeoutCounter = 0,
  };

  // driver->showWindSpeed(&state, 3);
  // state.windSpeedTransition = true; // Transition already finished
  // state.counter = 2;

  driver->cancelWindDirectionTransition(); // not testing transition here
  driver->onTimerFired(&state, values);

  //should fade from direction colour to first speed colour
  verify_colours((Colour*)&COLOUR_WIND_DIR, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, incrementsTransitionTimer)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .windSpeedTransition = true,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->showWindSpeed(&state, 3);

  driver->onTimerFired(&state, values);

  LONGS_EQUAL_TEXT(1, state.windSpeedTimeoutCounter, "Transition timeout counter not incremented!");
}

TEST(LedStripDriverWindTestGroup, doesntIncrementTransitionTimerWhenOnDirectionLayer)
{
  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->period(3);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
    .windSpeedTransition = true,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL_TEXT(0,
                   state.windSpeedTimeoutCounter,
                   "Transition timeout counter incremented when on direction layer!");
}

TEST(LedStripDriverWindTestGroup, writesCorrectValuesForWeatherWarningInitialState)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = 0,
    .weatherWarningFadeState = fadeIn,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->cancelWindDirectionTransition(); // not testing transition here
  driver->onTimerFired(&state, values);

  //'transparent' at first
  verify_colours((Colour*)&COLOUR_WIND_DIR, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, writesCorrectValuesForWeatherWarningMidFadeInState)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const Colour COLOUR_MID = Colour(127, 127, 127);

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_FADE_IN_MS/2 - 1,
    .weatherWarningFadeState = fadeIn,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, writesCorrectValuesForWeatherWarningEndOfFadeInState)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_FADE_IN_MS - 2,
    .weatherWarningFadeState = fadeIn,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_WARNING, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, incrementsWeatherWarningCounter)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = 0,
    .weatherWarningFadeState = fadeIn,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(WIND_TEST_LED_CONFIG.resolutionMs, state.weatherWarningCounter);
}

TEST(LedStripDriverWindTestGroup, resetsWeatherWarningCounterAfterFadeIn)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_FADE_IN_MS - 1,
    .weatherWarningFadeState = fadeIn,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.weatherWarningCounter);
}

TEST(LedStripDriverWindTestGroup, reversesWeatherWarningFadeDirectionAfterFadeIn)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_FADE_IN_MS - WIND_TEST_LED_CONFIG.resolutionMs,
    .weatherWarningFadeState = fadeIn,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(fadeOut, state.weatherWarningFadeState);
}

TEST(LedStripDriverWindTestGroup, writesCorrectValuesForWeatherWarningMidFadeOutState)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;
  const Colour& COLOUR_MID = Colour(127, 127, 127);

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_FADE_OUT_MS/2 - WIND_TEST_LED_CONFIG.resolutionMs,
    .weatherWarningFadeState = fadeOut,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, writesCorrectValuesForWeatherWarningOffDwellTime)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_OFF_DWELL_MS - (2 * WIND_TEST_LED_CONFIG.resolutionMs),
    .weatherWarningFadeState = offDwell,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->cancelWindDirectionTransition(); // not testing transition here
  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_WIND_DIR, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverWindTestGroup, setsWarningFadeStateToFadeInAfterOffDwell)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_OFF_DWELL_MS -  WIND_TEST_LED_CONFIG.resolutionMs,
    .weatherWarningFadeState = offDwell,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  CHECK_EQUAL(fadeIn, state.weatherWarningFadeState);
}

TEST(LedStripDriverWindTestGroup, setsWarningFadeStateToOffDwellAfterFadeOut)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_FADE_OUT_MS -  WIND_TEST_LED_CONFIG.resolutionMs,
    .weatherWarningFadeState = fadeOut,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  CHECK_EQUAL(offDwell, state.weatherWarningFadeState);
}

TEST(LedStripDriverWindTestGroup, resetsWeatherWarningCounterAfterFadeOut)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_FADE_OUT_MS - 1,
    .weatherWarningFadeState = fadeOut,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.weatherWarningCounter);
}

TEST(LedStripDriverWindTestGroup, resetsWeatherWarningCounterAfterOffDwell)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,

    .weatherWarningCounter = WARNING_OFF_DWELL_MS - 1,
    .weatherWarningFadeState = offDwell,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(0, state.weatherWarningCounter);
}

// detects a bug where the warning colour was written when currentStep == (steps - 1) when in off dwell
TEST(LedStripDriverWindTestGroup, shouldNotWriteWarningEndColourAtCounterIfInOffDwell)
{
  const uint32_t WARNING_FADE_IN_MS = 500;
  const uint32_t WARNING_FADE_OUT_MS = 2000;
  const uint32_t WARNING_OFF_DWELL_MS = 1000;
  const uint32_t PERIOD_MS = 1000;

  const Colour& COLOUR_WIND_DIR = COLOUR_RED;
  const Colour& COLOUR_WIND_SPD_1 = COLOUR_GREEN;
  const Colour& COLOUR_WIND_SPD_2 = COLOUR_BLUE;
  const Colour& COLOUR_WARNING = COLOUR_WHITE;

  driver->pattern(Pattern::wind)
        ->windDirectionColour((Colour*)&COLOUR_WIND_DIR)
        ->colourOn((Colour*)&COLOUR_WIND_SPD_1)
        ->colourOff((Colour*)&COLOUR_WIND_SPD_2)
        ->warningColour((Colour*)&COLOUR_WARNING)
        ->warningFadeIn(WARNING_FADE_IN_MS)
        ->warningFadeOut(WARNING_FADE_OUT_MS)
        ->warningOffDwell(WARNING_OFF_DWELL_MS)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS - 2,
    .fadeDirection = 1,

    .weatherWarningCounter = 0,
    .weatherWarningFadeState = offDwell,

    .windSpeedTransition = false,
    .timeoutCounter = 0,
    .windSpeedTimeoutCounter = 0,
  };

  driver->cancelWindDirectionTransition(); // not testing transition here
  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_WIND_DIR, lastValuesWritten, WIND_TEST_LED_CONFIG.numLeds);
}

/***********************************************************************************************
 * Air quality pattern
 **********************************************************************************************/
TEST_GROUP(LedStripDriverAirTestGroup)
{
  void setup() {
    const uint32_t valuesLength = MAX_LEDS * COLOURS_PER_LED;

    driver = new LedStripDriver((led_strip_config_t*)&AIR_TEST_LED_CONFIG);
    lastValuesWritten = new uint8_t[valuesLength];
    memset(lastValuesWritten, 0, valuesLength);
    memset(values, 0, valuesLength);
  }

  void teardown() {
    delete driver;
    delete[] lastValuesWritten;
  }
};

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerOneInitialState)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_RED;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_GREEN;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_WHITE;

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_AIR_L1_1, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerOneMidFade)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_WHITE;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_BLACK;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_RED;

  const Colour& COLOUR_MID = Colour(127, 127, 127);

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = 2000,
    .fadeDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerOneFinalState)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_RED;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_GREEN;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_WHITE;

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS-1,
    .fadeDirection = 1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_AIR_L1_2, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerOneMidRevFade)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_RED;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_GREEN;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_WHITE;

  const Colour& COLOUR_MID = Colour(127, 127, 0);

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = 2000,
    .fadeDirection = -1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerOneReverseFinalState)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_RED;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_GREEN;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_WHITE;

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS-1,
    .fadeDirection = -1,
  };

  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_AIR_L1_1, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerTwoInitialState)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_RED;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_GREEN;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_WHITE;

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = 0,
    .fadeDirection = 1,
  };

  driver->activeLayer(2);
  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_AIR_L2_1, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerTwoMidFade)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_WHITE;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_BLACK;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_RED;

  const Colour& COLOUR_MID = Colour(127, 0, 127);

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = 2000,
    .fadeDirection = 1,
  };

  driver->activeLayer(2);
  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerTwoFinalState)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_RED;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_GREEN;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_WHITE;

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS-1,
    .fadeDirection = 1,
  };

  driver->activeLayer(2);
  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_AIR_L2_2, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerTwoMidRevFade)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_RED;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_GREEN;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_WHITE;

  const Colour& COLOUR_MID = Colour(127, 127, 255);

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = 2000,
    .fadeDirection = -1,
  };

  driver->activeLayer(2);
  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_MID, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesForLayerTwoReverseFinalState)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_RED;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_GREEN;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_WHITE;

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS-1,
    .fadeDirection = -1,
  };

  driver->activeLayer(2);
  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_AIR_L2_1, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, writesCorrectValuesAfterReversal)
{
  const uint32_t PERIOD_MS = 4000;

  const Colour& COLOUR_AIR_L1_1 = COLOUR_RED;
  const Colour& COLOUR_AIR_L1_2 = COLOUR_GREEN;
  const Colour& COLOUR_AIR_L2_1 = COLOUR_BLUE;
  const Colour& COLOUR_AIR_L2_2 = COLOUR_WHITE;

  driver->pattern(Pattern::air)
        ->colourOn((Colour*)&COLOUR_AIR_L1_1)
        ->colourOff((Colour*)&COLOUR_AIR_L1_2)
        ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
        ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
        ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS,
    .fadeDirection = 1,
  };

  driver->activeLayer(1);
  driver->onTimerFired(&state, values);

  verify_colours((Colour*)&COLOUR_AIR_L1_2, lastValuesWritten, AIR_TEST_LED_CONFIG.numLeds);
}

TEST(LedStripDriverAirTestGroup, resetsCounter)
{
  const uint32_t PERIOD_MS = 3;
  const Colour COL_WHITE = COLOUR_WHITE;

  driver->pattern(Pattern::air)
    ->colourOn((Colour*)&COL_WHITE)
    ->colourOff((Colour*)&COL_WHITE)
    ->layer2Colour1((Colour*)&COL_WHITE)
    ->layer2Colour2((Colour*)&COL_WHITE)
    ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(1, state.counter);
}

TEST(LedStripDriverAirTestGroup, reversesFadeDirection)
{
  const uint32_t PERIOD_MS = 3;
  const Colour COL_WHITE = COLOUR_WHITE;

  driver->pattern(Pattern::air)
    ->colourOn((Colour*)&COL_WHITE)
    ->colourOff((Colour*)&COL_WHITE)
    ->layer2Colour1((Colour*)&COL_WHITE)
    ->layer2Colour2((Colour*)&COL_WHITE)
    ->period(PERIOD_MS);

  led_strip_state_t state = {
    .counter = PERIOD_MS,
    .fadeDirection = 1
  };

  driver->onTimerFired(&state, values);

  LONGS_EQUAL(-1, state.fadeDirection);
}
