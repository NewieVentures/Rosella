#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "utils.h"
#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <string>
#include <stdexcept>

using namespace std;

TEST_GROUP(HexStringToIntTestGroup) {

};

TEST(HexStringToIntTestGroup, returnsCorrectResultForSingleLowercaseHexChar)
{
    BYTES_EQUAL((uint8_t)15, hexStrToInt("f"));
}

TEST(HexStringToIntTestGroup, returnsCorrectResultForSingleUppercaseHexChar)
{
    BYTES_EQUAL((uint8_t)10, hexStrToInt("A"));
}

TEST(HexStringToIntTestGroup, returnsCorrectResultForSingleNumber)
{
    BYTES_EQUAL((uint8_t)0, hexStrToInt("0"));
}

TEST(HexStringToIntTestGroup, returnsCorrectResultForTwoChars)
{
    BYTES_EQUAL((uint8_t)245, hexStrToInt("F5"));
}

TEST_GROUP(StringToIntTestGroup) {

};

TEST(StringToIntTestGroup, convertsStringToIntWithValidInput) {
  const uint32_t NUM = 123456;
  uint32_t value;

  strToInt(&value, "123456");

  LONGS_EQUAL(NUM, value);
}

TEST(StringToIntTestGroup, returnsSuccessForValidInput) {
  uint32_t value;

  LONGS_EQUAL(0, strToInt(&value, "123456"));
}

TEST(StringToIntTestGroup, returnsErrorIfNotANumber) {
  uint32_t value;

  LONGS_EQUAL(-1, strToInt(&value, "123456a"));
}

TEST(StringToIntTestGroup, returnsErrorForEmptyString) {
  uint32_t value;
  LONGS_EQUAL(-1, strToInt(&value, ""));
}
