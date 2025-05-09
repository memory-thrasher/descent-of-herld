/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <set>
#include <iostream>

#include "time.hpp"
#include "fpsCounter.hpp"
#include "math.hpp"
#include "uiStyle.hpp"

namespace doh {

  fpsCounter::fpsCounter() :
    txt(guiTextVolatile::create()),
    txtData({ { -1, -1, 1, 1 } })
  {
    txt.setStyle(textOnlySmall().textBuf);
    guiTextFormat(txtContent, "%s", "FPS: XX");
    txt.writeIndirectBuffer(txtContent);
    txt.writeInstanceData(txtData);
  };

  fpsCounter::~fpsCounter() {
    txt.destroy();
  };

  void fpsCounter::update() {
    uint64_t delta = getDeltaNs();
    if(delta == 0 || delta > 1e9 * 3600 * 24)
      guiTextFormat(txtContent, "%s", "FPS: too many");
    else
      guiTextFormat(txtContent, "FPS: %0.2f  (%0.6f ms)", 1e9/delta, delta * 1e-6);
    txt.writeIndirectBuffer(txtContent);
    txt.writeInstanceData(txtData);
  };

}
