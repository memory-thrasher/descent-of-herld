/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

const uint nebulaSize = 128;

layout(std140, set = 0, binding = 0) uniform data_t {
  uvec4 origin;//bbox min corner, w=renderDistance
} instance;

#include cameraStuff.partial.glsl

layout(location = 0) out vec4 texLoc;//w = strength

void main() {
  const uvec3 vpa = gl_VertexIndex.xxx / uvec3(1, 6, nebulaSize * 6) % uvec3(6, nebulaSize, 4096);
  //would use u/imat3 if it existed
  const uvec3 axisSelector = uvec3(equal(uvec3(0, 1, 2), vpa.zzz));//is also the plane's normal
  const uvec3 worldPos = instance.origin.xyz + //this needs to happen twice, once as float and once as uint
    axisSelector.zxy * (uint(vpa.x & 1) * nebulaSize).xxx +
    axisSelector.yzx * (uint(vpa.x > 1 && vpa.x < 5) * nebulaSize).xxx +
    axisSelector.xyz * vpa.yyy;
  texLoc = vec4(vec3(axisSelector.zxy * (uint(vpa.x & 1) * nebulaSize).xxx +
		     axisSelector.yzx * (uint(vpa.x > 1 && vpa.x < 5) * nebulaSize).xxx +
		     axisSelector.xyz * (vpa.y + 0.5f).xxx) / nebulaSize,
		(1 - pow(length(ivec3(worldPos - cameraTransform.sector.xyz)) / instance.origin.w, 2)) / nebulaSize);
  gl_Position = projectSectorDivW(worldPos, uint(0x80000000) * axisSelector);
}

