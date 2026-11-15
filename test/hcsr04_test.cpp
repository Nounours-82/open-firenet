#include "firenet_hcsr04.h"
#include <stdio.h>
#include <string.h>

static int failures = 0;

static void check(const char* name, bool actual, bool expected) {
  if (actual == expected) return;
  printf("FAIL %s: got %s, expected %s\n", name, actual ? "true" : "false", expected ? "true" : "false");
  failures++;
}

static void checkInt(const char* name, int actual, int expected) {
  if (actual == expected) return;
  printf("FAIL %s: got %d, expected %d\n", name, actual, expected);
  failures++;
}

int main() {
  checkInt("empty hopper", firenet::hcsr04FillPercent(60.0f, 5, 60), 0);
  checkInt("half full", firenet::hcsr04FillPercent(32.5f, 5, 60), 50);
  checkInt("full at calibrated distance", firenet::hcsr04FillPercent(5.0f, 5, 60), 100);
  checkInt("closer than full calibration", firenet::hcsr04FillPercent(2.0f, 5, 60), 100);

  firenet::Hcsr04Config config;
  config.enabled = true;
  config.maxHeightCm = 60;
  config.fullDistanceCm = 5;
  char serialized[48];
  check("configuration serializes", firenet::hcsr04SerializeConfig(config, serialized, sizeof(serialized)), true);

  firenet::Hcsr04Config restored;
  check("configuration parses after persistence", firenet::hcsr04ParseConfig(serialized, restored), true);
  checkInt("full distance survives persistence", restored.fullDistanceCm, 5);
  check("old configuration remains readable", firenet::hcsr04ParseConfig("1,16,17,0,60,10", restored), true);
  checkInt("old configuration gets compatible full distance", restored.fullDistanceCm, 2);
  check("full distance cannot equal empty distance", firenet::hcsr04ParseConfig("1,16,17,0,60,10,60", restored), false);

  if (failures) return 1;
  puts("HC-SR04 calibration tests passed");
  return 0;
}
