/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "nebula.hpp"
#include "f16.hpp"
#include "elements.hpp"

namespace doh {

  //visualize a series of nested shells of different atoms, inflating from within from light pressure, until surface tension between shells is broken and the atoms spill out past each other. The heavier atoms are nearer the core, and receive more energy from the core collapse. Some of that energy is emparted onto the lighter atoms as they are pushed out of the way. Surface tension and the viscus effect can be observed within sub-clouds of atoms flowing away from the dying star. The heavier atoms are more prone to forming larger coherent masses in deep space.
  void generateNebula(const glm::uvec3& location, nebula_data_t& out) {
    for(int32_t z = 0;z < nebulaSize;z++)
      for(int32_t y = 0;y < nebulaSize;y++)
	for(int32_t x = 0;x < nebulaSize;x++) {
	  uint32_t idx = (z * nebulaSize + y) * nebulaSize + x;
	  out.sectors[idx].composition.data[0] = { 
	}
  };

  void updateNebulaExposure(nebula_data_t& nebulaData) {
  };

  //this should probably be async
  void updateNebula(const nebula_data_t& nebulaData, nebulaMap_t& out) {
    for(int32_t z = 0;z < nebulaSize;z++)
      for(int32_t y = 0;y < nebulaSize;y++)
	for(int32_t x = 0;x < nebulaSize;x++) {
	  uint32_t idx = (z * nebulaSize + y) * nebulaSize + x;
	  out[idx] = f16Pack4(nebulaData.sectors[idx].exposure * nebulaData.sectors[idx].diffusion);
	}
  };

}
