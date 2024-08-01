/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

//NOTE: cotangent of one pixel's FOV is ~688 for 45 degree total fov on a 1080p screen
//intensity saturation distance = 9k sectors (360ly) beyond which stars are unlikely to shade a pixel at all
//sector saturation distance = 687 sectors beyond which a sector might fall entirely between pixels
//size saturation distance (1080p) = 4 sectors (7000 au) where a star found will only be one pixel or smaller
//a renderer (type TBD) will render the star in the camera's current sector
//a mesh or geometry renderer that outputs points will shader stars in other sectors within 700 sectors
//a whole-screen analytical fragment shader will query stars behind each pixel beyond 700 sectors away

/* near-ish stars: geometry shader
   each input plane is a vertex (data in vertex buffer), override the instance count to max plane count (render depth)
   vertex shader projects the plane into camera space and passes along as much as we can pre-compute
   geometry shader, one invocation per plane, "points" in, actual points out.
 */

namespace doh {

  //global constants because we might need them in other shaders
  constexpr uint32_t starGridCount = 1;//for testing, later will have many more
  //note: due to max output vertex limitations and to take advantage of parallelism, favor more planes of lower density
  const glm::uvec2 starGrids[starGridCount] = {
    { glm::packInt4x8({ 19, 11, 29, 1 }), glm::packUint4x8({ 12, 42, 18, 19 }) },
    //y.xy*y.w and (y.y, y.-x)*y.w define the axis of a grid along the z=0 plan where ray origins are found. Each ray spawns a star every x.xyz*x.w sectors.
  };

}

