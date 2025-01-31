/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <WITE/WITE.hpp>
#include <set> //needed by targetPrimary_stub

#include "uxBase.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "math.hpp"

namespace doh {

  uxBase::uxBase() {};

  void uxBase::update() {};

  void uxBase::setVisible(bool visible) {
    wantsVisibility = visible;
  };

  bool uxBase::isVisible() const {
    return visible;
  };

  bool uxBase::isHovered() const {
    WITE::winput::compositeInputData mouseLocation;
    const glm::vec4& bounds = getBounds();
    bool isHovered = false;
    WITE::winput::getInput(WITE::winput::mouse, mouseLocation);
    WITE::scopeLock lock(&targetPrimary::allInstances_mutex);
    for(const targetPrimary& camera : targetPrimary::allInstances) {
      const auto size = camera.getWindow().getVecSize();
      glm::vec2 mouseInSnorm(mouseLocation.axes[0].current / size.x * 2 - 1,
			     mouseLocation.axes[1].current / size.y * 2 - 1);
      if(rectContainsPoint(bounds, mouseInSnorm)) {
	isHovered = true;
	break;
      }
    }
    return isHovered;
  };

}
