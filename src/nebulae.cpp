/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "nebulae.hpp"

namespace doh {

  //this should probably be async
  void generateNebula(const glm::uvec3& location, nebulaMap_t& out) {
    //TODO prettier
    //TODO query for nearby/within star locations to emulate reflectivity/diffusion
    constexpr glm::vec3 center { nebulaSize/2, nebulaSize/2, nebulaSize/2 };
    const float maxDst = glm::dot(center, center);
    for(int32_t z = 0;z < nebulaSize;z++)
      for(int32_t y = 0;y < nebulaSize;y++)
	for(int32_t x = 0;x < nebulaSize;x++) {
	  glm::vec3 pnt { x, y, z },
	    delta = pnt - center;
	  float dst = glm::dot(delta, delta),
	    strength = 1 - dst / maxDst;
	  out[(z * nebulaSize + y) * nebulaSize + x] = glm::packF2x11_1x10({ strength/3, strength, strength/2 });
	}
  };

}
