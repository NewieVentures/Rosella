#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "Particle.h"
#include "colours.h"
#include "ledStripDriver.h"
#include "cloudFunctions.h"
#include "argParser.h"

#define NUM_LEDS 3
#define TIMER_RESOLUTION_MS 5
#define ARGS_LEN_MAX 63

LedStripDriver *ledStripDriver;
CloudFunctions *cloudFunctions;

static int registerFunction(String name, int (CloudFunctions::*fn)(String), CloudFunctions *cls) {
  mock().actualCall("registerFunction")
    .withParameter("name", name.c_str())
    .withParameter("fn", (void*)fn)
    .withParameter("cls", cls);

  return 0;
}

static void writeLedValues(uint8_t *values, uint32_t length) {}

static const led_strip_config_t CONFIG_LED_STRIP = {
    .numLeds = NUM_LEDS,
    .writeValueFn = writeLedValues,
    .resolutionMs = TIMER_RESOLUTION_MS,
};

TEST_GROUP(CloudFunctionsTestGroup)
{
    void setup() {
      ledStripDriver = new LedStripDriver((led_strip_config_t*)&CONFIG_LED_STRIP);
      mock().ignoreOtherCalls();
    }

    void teardown() {
      delete ledStripDriver;
    }
};

TEST(CloudFunctionsTestGroup, registersFunctions)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "blink")
    .withParameter("fn", (void*)&CloudFunctions::blink)
    .withParameter("cls", cloudFunctions);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "colour")
    .withParameter("fn", (void*)&CloudFunctions::colour)
    .withParameter("cls", cloudFunctions);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "defaultLoop")
    .withParameter("fn", (void*)&CloudFunctions::defaultLoop)
    .withParameter("cls", cloudFunctions);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "strobe")
    .withParameter("fn", (void*)&CloudFunctions::strobe)
    .withParameter("cls", cloudFunctions);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "gradient")
    .withParameter("fn", (void*)&CloudFunctions::gradient)
    .withParameter("cls", cloudFunctions);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "progress")
    .withParameter("fn", (void*)&CloudFunctions::progress)
    .withParameter("cls", cloudFunctions);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "snake")
    .withParameter("fn", (void*)&CloudFunctions::snake)
    .withParameter("cls", cloudFunctions);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "pulse")
    .withParameter("fn", (void*)&CloudFunctions::pulse)
    .withParameter("cls", cloudFunctions);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "weather")
    .withParameter("fn", (void*)&CloudFunctions::weather)
    .withParameter("cls", cloudFunctions);

  mock().expectOneCall("registerFunction")
    .withParameter("name", "wind")
    .withParameter("fn", (void*)&CloudFunctions::wind)
    .withParameter("cls", cloudFunctions);

  cloudFunctions->registerFunctions(&registerFunction);
  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, blinkReturnsSuccessForValidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->blink("1000,50,#FF0000,#000000"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, blinkPassesCorrectArgsToLedDriver)
{
  const uint32_t PERIOD_MS = 1000;
  const uint8_t DUTY_CYCLE = 50;
  Colour COLOUR_ON = Colour("#FF0000");
  Colour COLOUR_OFF = Colour("#000000");
  char args[ARGS_LEN_MAX];

  sprintf(args, "%d,%d,%s,%s",
          PERIOD_MS,
          DUTY_CYCLE,
          COLOUR_ON.toString().c_str(),
          COLOUR_OFF.toString().c_str());

  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->blink(args);

  CHECK(Pattern::blink == ledStripDriver->getPattern());
  LONGS_EQUAL(PERIOD_MS, ledStripDriver->getPeriod());
  LONGS_EQUAL(DUTY_CYCLE, ledStripDriver->getDutyCycle());
  STRCMP_EQUAL(COLOUR_ON.toString(), ledStripDriver->getColourOn()->toString());
  STRCMP_EQUAL(COLOUR_OFF.toString(), ledStripDriver->getColourOff()->toString());

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, colourReturnsSuccessForValidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->colour("#FF0000"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, colourPassesCorrectArgsToLedDriver)
{
  Colour COLOUR = Colour("#001800");

  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->colour(COLOUR.toString());

  STRCMP_EQUAL(COLOUR.toString(), ledStripDriver->getColourOn()->toString());

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, strobeReturnsSuccessForValidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->strobe("200,#028128"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, strobeReturnsErrorForInvalidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_INVALID_ARG,
              cloudFunctions->strobe(",#028128"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, strobePassesCorrectArgsToLedDriver)
{
  const uint32_t PERIOD_MS = 1000;
  Colour COLOUR = Colour("#FF0000");
  char args[ARGS_LEN_MAX];

  sprintf(args, "%d,%s", PERIOD_MS, COLOUR.toString().c_str());

  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->strobe(args);

  CHECK(Pattern::strobe == ledStripDriver->getPattern());
  LONGS_EQUAL(PERIOD_MS, ledStripDriver->getPeriod());
  STRCMP_EQUAL(COLOUR.toString(), ledStripDriver->getColourOn()->toString());
  STRCMP_EQUAL(COLOUR_BLACK.toString(), ledStripDriver->getColourOff()->toString());

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, gradientReturnsSuccessForValidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->gradient("#000000,#FFFFFF"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, gradientReturnsErrorForInvalidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_INVALID_ARG,
              cloudFunctions->gradient(",#028128"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, gradientPassesCorrectArgsToLedDriver)
{
  Colour COLOUR_START = Colour("#FF0000");
  Colour COLOUR_END = Colour("#0000FF");
  char args[ARGS_LEN_MAX];

  sprintf(args, "%s,%s", COLOUR_START.toString().c_str(), COLOUR_END.toString().c_str());

  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->gradient(args);

  CHECK(Pattern::gradient == ledStripDriver->getPattern());
  STRCMP_EQUAL(COLOUR_START.toString(), ledStripDriver->getColourOn()->toString());
  STRCMP_EQUAL(COLOUR_END.toString(), ledStripDriver->getColourOff()->toString());

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, progressReturnsSuccessForValidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->progress("12,20,2,250,3000,0,#FFFFFF,#000000"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, progressReturnsErrorForInvalidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_INVALID_ARG,
              cloudFunctions->progress(",20,2,250,3000,0,#FFFFFF,#000000"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, progressPassesCorrectArgsToLedDriver)
{
  uint8_t INITIAL = 2;
  uint8_t FINAL = 5;
  uint8_t INCREMENT = 1;
  uint32_t INC_DELAY = 200;
  uint32_t RESET_DELAY = 3000;
  Direction DIRECTION = Direction::reverse;
  Colour COLOUR_ON = Colour("#123456");
  Colour COLOUR_OFF = Colour("#AABBCC");
  char args[ARGS_LEN_MAX];

  sprintf(args, "%d,%d,%d,%d,%d,%d,%s,%s",
          INITIAL,
          FINAL,
          INCREMENT,
          INC_DELAY,
          RESET_DELAY,
          DIRECTION == Direction::forward ? 0 : 1,
          COLOUR_ON.toString().c_str(),
          COLOUR_OFF.toString().c_str());

  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->progress(args);

  CHECK(Pattern::progress == ledStripDriver->getPattern());
  BYTES_EQUAL(INITIAL, ledStripDriver->getProgressInitial());
  BYTES_EQUAL(FINAL, ledStripDriver->getProgressFinal());
  BYTES_EQUAL(INCREMENT, ledStripDriver->getProgressIncrement());
  LONGS_EQUAL(INC_DELAY, ledStripDriver->getProgressIncrementDelay());
  LONGS_EQUAL(RESET_DELAY, ledStripDriver->getProgressResetDelay());
  CHECK(DIRECTION == ledStripDriver->getProgressDirection());
  STRCMP_EQUAL(COLOUR_ON.toString(), ledStripDriver->getColourOn()->toString());
  STRCMP_EQUAL(COLOUR_OFF.toString(), ledStripDriver->getColourOff()->toString());

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, snakeReturnsSuccessForValidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->snake("100,0,8,#FFFFFF,#000000"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, snakeReturnsErrorForInvalidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_INVALID_ARG,
              cloudFunctions->snake(",0,2,#FFFFFF,#000000"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, snakePassesCorrectArgsToLedDriver)
{
  uint32_t PERIOD_MS = 1000;
  uint8_t LENGTH = 5;
  Direction DIRECTION = Direction::reverse;
  Colour COLOUR_ON = Colour("#9823AF");
  Colour COLOUR_OFF = Colour("#883322");
  char args[ARGS_LEN_MAX];

  sprintf(args, "%d,%d,%d,%s,%s",
          PERIOD_MS,
          DIRECTION == Direction::forward ? 0 : 1,
          LENGTH,
          COLOUR_ON.toString().c_str(),
          COLOUR_OFF.toString().c_str());

  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->snake(args);

  CHECK(Pattern::snake == ledStripDriver->getPattern());
  LONGS_EQUAL(PERIOD_MS, ledStripDriver->getPeriod());
  CHECK(DIRECTION == ledStripDriver->getSnakeDirection());
  LONGS_EQUAL(LENGTH, ledStripDriver->getSnakeLength());
  STRCMP_EQUAL(COLOUR_ON.toString(), ledStripDriver->getColourOn()->toString());
  STRCMP_EQUAL(COLOUR_OFF.toString(), ledStripDriver->getColourOff()->toString());

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, pulseReturnsSuccessForValidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->pulse("3000,#FFFFFF,#000000"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, pulseReturnsErrorForInvalidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_INVALID_ARG,
              cloudFunctions->pulse(",#FFFFFF,#000000"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, pulsePassesCorrectArgsToLedDriver)
{
  uint32_t PERIOD_MS = 1000;
  Colour COLOUR_ON = Colour("#9823AF");
  Colour COLOUR_OFF = Colour("#883322");
  char args[ARGS_LEN_MAX];

  sprintf(args, "%d,%s,%s",
          PERIOD_MS,
          COLOUR_ON.toString().c_str(),
          COLOUR_OFF.toString().c_str());

  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->pulse(args);

  CHECK(Pattern::pulse == ledStripDriver->getPattern());
  LONGS_EQUAL(PERIOD_MS, ledStripDriver->getPeriod());
  STRCMP_EQUAL(COLOUR_ON.toString(), ledStripDriver->getColourOn()->toString());
  STRCMP_EQUAL(COLOUR_OFF.toString(), ledStripDriver->getColourOff()->toString());

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, defaultLoopReturnsSuccessWithNoArguments)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->defaultLoop(""));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, defaultLoopPassesCorrectArgsToLedDriver)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->defaultLoop("");

  CHECK(Pattern::defaultLoop == ledStripDriver->getPattern());
  LONGS_EQUAL(2000, ledStripDriver->getPeriod());

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, windPassesCorrectArgsToLedDriver)
{
  uint32_t FADE_INTERVAL = 10;
  uint32_t WARN_FADE_IN = 2000;
  uint32_t WARN_FADE_OUT = 3000;
  uint32_t WARN_OFF_DWELL = 4000;
  Colour COLOUR_DIR = Colour("#34239A");
  Colour COLOUR_SPD_1 = Colour("#8B2A75");
  Colour COLOUR_SPD_2 = Colour("#501243");
  char args[ARGS_LEN_MAX];

  sprintf(args, "%s,%s,%s,%d,%d,%d,%d",
          COLOUR_DIR.toString().c_str(),
          COLOUR_SPD_1.toString().c_str(),
          COLOUR_SPD_2.toString().c_str(),
          FADE_INTERVAL,
          WARN_FADE_IN,
          WARN_FADE_OUT,
          WARN_OFF_DWELL);

  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->wind(args);

  CHECK(Pattern::wind == ledStripDriver->getPattern());
  STRCMP_EQUAL(COLOUR_DIR.toString(), ledStripDriver->getWindDirectionColour()->toString());
  STRCMP_EQUAL(COLOUR_SPD_1.toString(), ledStripDriver->getColourOn()->toString());
  STRCMP_EQUAL(COLOUR_SPD_2.toString(), ledStripDriver->getColourOff()->toString());
  LONGS_EQUAL(FADE_INTERVAL * 1000, ledStripDriver->getPeriod());
  LONGS_EQUAL(WARN_FADE_IN, ledStripDriver->getWeatherWarningFadeIn());
  LONGS_EQUAL(WARN_FADE_OUT, ledStripDriver->getWeatherWarningFadeOut());
  LONGS_EQUAL(WARN_OFF_DWELL, ledStripDriver->getWeatherWarningOffDwell());

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, windReturnsErrorForInvalidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_INVALID_ARG,
              cloudFunctions->wind("0,#FFFFFF,#000000,10,100,100,100"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, windReturnsSuccessForValidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->wind("#000000,#111111,#222222,10,100,100,100"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, windDoesntChangePatternOnParseError)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  cloudFunctions->wind("#000000,#111111,#222222,10,100,100,");
  CHECK_TEXT(ledStripDriver->getPattern() != Pattern::wind, "Pattern changed to wind!");

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, weatherPassesCorrectArgsToLedDriver)
{
  uint32_t TEMP_FADE_SECS = 10;
  uint32_t RAIN_BAND_DEPTH = 2;
  uint32_t RAIN_BAND_SPEED = 800;
  uint32_t RAIN_BAND_SPACING = 3;
  uint32_t WARN_FADE_IN = 2000;
  uint32_t WARN_FADE_OUT = 3000;
  uint32_t WARN_OFF_DWELL = 4000;

  Colour COLOUR_TEMP_1 = Colour("#29A5C3");
  Colour COLOUR_TEMP_2 = Colour("#2012BA");
  Colour COLOUR_RAIN = Colour("#FFFFFF");
  Colour COLOUR_WARNING = Colour("#FFFFFF");
  char args[ARGS_LEN_MAX];

  sprintf(args, "%s,%s,%d,%d,%d,%d,%d,%d,%d",
          COLOUR_TEMP_1.toString().c_str(),
          COLOUR_TEMP_2.toString().c_str(),
          TEMP_FADE_SECS,
          RAIN_BAND_DEPTH,
          RAIN_BAND_SPEED,
          RAIN_BAND_SPACING,
          WARN_FADE_IN,
          WARN_FADE_OUT,
          WARN_OFF_DWELL);

  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);
  cloudFunctions->weather(args);

  CHECK_TEXT(Pattern::weather == ledStripDriver->getPattern(), "Wrong pattern!");
  STRCMP_EQUAL_TEXT(COLOUR_TEMP_1.toString(),
                    ledStripDriver->getColourOn()->toString(),
                    "Temp 1 colour incorrect");
  STRCMP_EQUAL_TEXT(COLOUR_TEMP_2.toString(),
                    ledStripDriver->getColourOff()->toString(),
                    "Temp 2 colour incorrect");
  LONGS_EQUAL_TEXT(TEMP_FADE_SECS,
                   ledStripDriver->getWeatherTempFadeIntervalSecs(),
                   "Temp fade interval incorrect!");

  BYTES_EQUAL_TEXT(RAIN_BAND_DEPTH,
                   ledStripDriver->getWeatherRainBandHeightLeds(),
                   "Rain band depth incorrect!");
  LONGS_EQUAL_TEXT(RAIN_BAND_SPEED,
                   ledStripDriver->getWeatherRainBandIncDelayMs(),
                   "Rain band speed incorrect!");
  BYTES_EQUAL_TEXT(RAIN_BAND_SPACING,
                   ledStripDriver->getWeatherRainBandSpacingLeds(),
                   "Rain band spacing incorrect!");
  STRCMP_EQUAL_TEXT(COLOUR_RAIN.toString(),
                    ledStripDriver->getWeatherRainBandColour()->toString(),
                    "Rain colour incorrect");
  CHECK_EQUAL_TEXT(Direction::reverse,
                   ledStripDriver->getWeatherRainDirection(),
                   "Rain direction incorrect!");

  LONGS_EQUAL_TEXT(WARN_FADE_IN,
                   ledStripDriver->getWeatherWarningFadeIn(),
                   "Warning fade in incorrect");
  LONGS_EQUAL_TEXT(WARN_FADE_OUT,
                   ledStripDriver->getWeatherWarningFadeOut(),
                   "Warning fade out incorrect");
  LONGS_EQUAL_TEXT(WARN_OFF_DWELL,
                   ledStripDriver->getWeatherWarningOffDwell(),
                   "Warning off dwell incorrect");
  STRCMP_EQUAL_TEXT(COLOUR_WARNING.toString(),
                    ledStripDriver->getWeatherWarningColour()->toString(),
                    "Warning colour incorrect");

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, weatherReturnsErrorForInvalidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_INVALID_ARG,
              cloudFunctions->weather("0,#FFFFFF,10,100,100,100,1,1,1"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, weatherReturnsSuccessForValidInput)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  LONGS_EQUAL(argParser::RET_VAL_SUC,
              cloudFunctions->weather("#000000,#111111,4,2,500,3,500,2000,8000"));

  delete cloudFunctions;
}

TEST(CloudFunctionsTestGroup, weatherDoesntChangePatternOnParseError)
{
  cloudFunctions = new CloudFunctions(ledStripDriver);
  cloudFunctions->registerFunctions(&registerFunction);

  cloudFunctions->weather("#000000,#111111,10,100,100,");
  CHECK_TEXT(ledStripDriver->getPattern() != Pattern::weather, "Pattern changed to weather!");

  delete cloudFunctions;
}

// TODO reset state before each new pattern
