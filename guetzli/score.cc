/*
 * Copyright 2016 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "guetzli/score.h"

#include <cmath>

namespace guetzli {

double ScoreJPEG(double butteraugli_distance, int size,
                 double butteraugli_target) {
  constexpr double kScale = 50;
  constexpr double kMaxExponent = 10;
  constexpr double kLargeSize = 1e30;
  // Smooth scoring function that also rewards distance below target.
  // When below target, a small bonus proportional to the margin is applied
  // so the optimizer prefers solutions with headroom rather than treating
  // all below-target results identically.
  double diff = butteraugli_distance - butteraugli_target;
  if (diff <= 0.0) {
    // Smoothly reward being further below the target distance.
    // The multiplier ranges from 1.0 (at target) down toward ~0.5 (far below).
    double below_bonus = 1.0 / (1.0 + (-diff) / butteraugli_target);
    return below_bonus * size;
  } else {
    double exponent = kScale * diff;
    if (exponent > kMaxExponent) {
      return kLargeSize * std::exp(kMaxExponent) * diff + size;
    } else {
      return std::exp(exponent) * size;
    }
  }
}

}  // namespace guetzli
