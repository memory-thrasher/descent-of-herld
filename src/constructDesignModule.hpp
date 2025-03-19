/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

namespace doh {

  enum class moduleShape_e : uint64_t {
    cylinder,//for centrifuges and anything else with one large machine that handles cargo
    boxArray,//for a large number of smaller machines
    hexArray,//alt to boxGrid
    flatArray,//for photovoltaic cells and other components that benefit from LoS exposure
    //parabolicArray,//MAYBE, alt flatArray
    sphere,//for reactions involving reflectivity, especially nuclear and laser, and maybe some late-game stuff
  };

  struct constructDesignModule {
    dbRef<constructDesign> construct;
    moduleShape_e shape;
    glm::mat4 transform;//object origin is object center, local to (concatenate to) construct's transform
    glm::uvec3 size;//meters
    glm::mat3 symbolTransform;
    glm::uvec3 repetitions,//1D: cylinder (axial); 2D: flat; 3D: box, hex; unused: sphere
      compsPerRep;//components per repetition, same dims as above, how many total components per unit
  };

}
