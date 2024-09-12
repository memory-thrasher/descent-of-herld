/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

//if a shader wants only the transform then use cameraTransform.partial.glsl directly. Camera data only would mean it is at binding 0 so the below declaration is not reusable in that case so cameraDataOnly.partial.glsl exists to fill that need.

#include cameraTransform.partial.glsl

layout(std140, set = 1, binding = 1) uniform cameraData_t {
  vec4 geometry;//xy = size pixels, zw = cotan(fov.xy/2)
  uvec4 renderDistances;//x = near in chunks, y = mid in chunks, z = far in chunks, w = skybox in sectors
} cameraData;

vec4 projectSector(uvec3 worldPnt) {
  const vec3 pnt = vec3(((vec3(ivec3(worldPnt - cameraTransform.sector.xyz)) - (cameraTransform.chunk.xyz >> 16) / 65536.0f) * cameraTransform.transform).xyz);
  return vec4(pnt.xy / (abs(pnt.z) * cameraData.geometry.zw), pnt.z / cameraData.renderDistances.w, 1);
}

vec4 projectSectorDivW(uvec3 worldPnt, uvec3 chunkOffset) {
  uvec3 chunkBorrow;
  const uvec3 chunkDelta = usubBorrow(chunkOffset, cameraTransform.chunk.xyz, chunkBorrow);
  const vec3 pnt = vec3(((vec3(ivec3(worldPnt - cameraTransform.sector.xyz - chunkBorrow)) + (chunkDelta >> 16) / 65536.0f) * cameraTransform.transform).xyz);
  return vec4(pnt.xy / cameraData.geometry.zw, abs(pnt.z) * pnt.z / cameraData.renderDistances.w, abs(pnt.z));
}

//TODO projectChunk and projectNear similarly as needed
