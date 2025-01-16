/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <chrono>

#include "time.hpp"

namespace doh {

  namespace time_internals {

    uint64_t frameStartTime = 0, lastFrameStartTime = 0;

  }

  using namespace time_internals;

  void timeUpdate() {
    lastFrameStartTime = frameStartTime;
    frameStartTime = getNs();
  };

  uint64_t getNs() {
    return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
  };

  uint64_t getFrameNs() {
    return frameStartTime;
  };

  uint64_t getDeltaNs() {
    return frameStartTime - lastFrameStartTime;
  };

}
