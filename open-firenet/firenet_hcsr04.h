#pragma once

#include <math.h>
#include <stdio.h>

namespace firenet {

struct Hcsr04Config {
  bool enabled = false;
  int triggerPin = 16, echoPin = 17;
  int minHeightCm = 0, maxHeightCm = 100, alertHeightCm = 10;
  int fullDistanceCm = 2;
};

inline int hcsr04FillPercent(float distanceCm, int fullDistanceCm, int emptyDistanceCm) {
  const float percent = (emptyDistanceCm - distanceCm) * 100.0f /
                        (emptyDistanceCm - fullDistanceCm);
  if (percent <= 0.0f) return 0;
  if (percent >= 100.0f) return 100;
  return (int)lroundf(percent);
}

inline bool hcsr04ParseConfig(const char* serialized, Hcsr04Config& cfg) {
  int enabled = 0;
  int fullDistanceCm = 2;
  const int fields = sscanf(serialized, "%d,%d,%d,%d,%d,%d,%d", &enabled, &cfg.triggerPin,
                            &cfg.echoPin, &cfg.minHeightCm, &cfg.maxHeightCm, &cfg.alertHeightCm,
                            &fullDistanceCm);
  if ((fields != 6 && fields != 7) || (enabled != 0 && enabled != 1)) return false;

  cfg.enabled = enabled == 1;
  cfg.fullDistanceCm = fields == 7 ? fullDistanceCm : (cfg.maxHeightCm > 2 ? 2 : 0);
  return cfg.minHeightCm >= 0 && cfg.minHeightCm < cfg.maxHeightCm && cfg.maxHeightCm <= 400 &&
         cfg.fullDistanceCm >= 0 && cfg.fullDistanceCm < cfg.maxHeightCm &&
         cfg.alertHeightCm >= cfg.minHeightCm && cfg.alertHeightCm <= cfg.maxHeightCm;
}

inline bool hcsr04SerializeConfig(const Hcsr04Config& cfg, char* output, size_t outputSize) {
  const int written = snprintf(output, outputSize, "%d,%d,%d,%d,%d,%d,%d", cfg.enabled ? 1 : 0,
                               cfg.triggerPin, cfg.echoPin, cfg.minHeightCm, cfg.maxHeightCm,
                               cfg.alertHeightCm, cfg.fullDistanceCm);
  return written >= 0 && (size_t)written < outputSize;
}

}  // namespace firenet
