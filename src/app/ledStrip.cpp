#include "Particle.h"
#include "config.h"
#include "colour.h"
#include "colours.h"
#include "dmx.h"
#include "ledStripDriver.h"
#include "ledStrip.h"

static LedStripDriver *ledDriver;
static led_strip_state_t ledState;
static uint8_t ledValues[NUM_LEDS * COLOURS_PER_LED];
static const Colour COLOUR_START = COLOUR_BLUE;
static const Colour COLOUR_END = COLOUR_GREEN;
static const Colour COLOUR_WIND = COLOUR_RED;
static const Colour COLOUR_WARNING = COLOUR_WHITE;
static const Colour COLOUR_AIR_L2_1 = COLOUR_RED;
static const Colour COLOUR_AIR_L2_2 = COLOUR_ORANGE;

static void updateLedsDmx(uint8_t *values, uint32_t length) {
  dmx::send(values, length);
}

static const led_strip_config_t CONFIG_LED_STRIP = {
  .numLeds = NUM_LEDS,
  .writeValueFn = updateLedsDmx,
  .resolutionMs = TIMER_RESOLUTION_MS,
};

void ledStrip::onTimerFired() {
  ledDriver->onTimerFired(&ledState, ledValues);
}

void onPatternLayer1Selected() {
  ledDriver->activeLayer(1);
}

void onPatternLayer2Selected() {
  ledDriver->activeLayer(2);
}

void initToggleSwitch() {
  // Active low SPDT rocker switch
  pinMode(GPIO_L1_BUTTON, INPUT_PULLUP);
  pinMode(GPIO_L2_BUTTON, INPUT_PULLUP);

  if (digitalRead(GPIO_L2_BUTTON) == LOW) {
    onPatternLayer2Selected();
  } else {
    onPatternLayer1Selected();
  }

  attachInterrupt(GPIO_L1_BUTTON, &onPatternLayer1Selected, FALLING);
  attachInterrupt(GPIO_L2_BUTTON, &onPatternLayer2Selected, FALLING);
}

void ledStrip::setup() {
  dmx::setup();

  ledDriver = new LedStripDriver((led_strip_config_t*)&CONFIG_LED_STRIP);
  ledDriver->initState(&ledState);

  // default pattern on power-up
  ledDriver->pattern(Pattern::defaultLoop)
           ->period(5000);
  // ledDriver->pattern(Pattern::colour)
    // ->colourOn((Colour*)&COLOUR_START);

  /*
  ledDriver->pattern(Pattern::pulse)
    ->period(2000)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END);
  */

  // ledDriver->pattern(Pattern::wind)
  //   ->colourOn((Colour*)&COLOUR_START)
  //   ->colourOff((Colour*)&COLOUR_END)
  //   ->windDirectionColour((Colour*)&COLOUR_WIND)
  //   ->timeout(PATTERN_TIMEOUT_MS)
  //   ->period(6000)
  //   ->warningColour((Colour*)&COLOUR_WARNING)
  //   ->warningFadeIn(0)
  //   ->warningFadeOut(2000)
  //   ->warningOffDwell(10000);

  /*
  ledDriver->pattern(Pattern::weather)
    ->colourOn((Colour*)&COLOUR_START)
    ->colourOff((Colour*)&COLOUR_END)
    ->tempFadeInterval(2)
    ->timeout(PATTERN_TIMEOUT_MS)
    ->rainBandHeight(2)
    ->rainBandSpacing(3)
    ->rainBandIncrementDelay(500)
    ->warningColour((Colour*)&COLOUR_WARNING)
    ->warningFadeIn(500)
    ->warningFadeOut(2000)
    ->warningOffDwell(10000);
  */

   // ledDriver->pattern(Pattern::air)
   //   ->colourOn((Colour*)&COLOUR_START)
   //   ->colourOff((Colour*)&COLOUR_END)
   //   ->layer2Colour1((Colour*)&COLOUR_AIR_L2_1)
   //   ->layer2Colour2((Colour*)&COLOUR_AIR_L2_2)
   //   ->period(4000);

   initToggleSwitch();
}

LedStripDriver* ledStrip::getDriver() {
  return ledDriver;
}
