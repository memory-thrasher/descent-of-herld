/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

layout(std140, set = 0, binding = 0) uniform starData_t {
  uvec4[5] starTypes;
} starData;

layout(location = 0) out vec4 outColor;

//TODO ask for early fragment tests

void main() {
  // outColor = vec4((gl_PrimitiveID >> 16) & 0xFF, (gl_PrimitiveID >> 8) & 0xFF, gl_PrimitiveID & 0xFF, 1) / 255.0f;
  // outColor = (1).xxxx;
  const uint pack = starData.starTypes[gl_PrimitiveID / 4][gl_PrimitiveID % 4];
  const uvec4 upack = ((pack).xxxx >> uvec4(24, 16, 8, 0)) & 0xFF;
  outColor = vec4(clamp(upack.xyz / 256.0f, 0, 1), 1);//TODO distance shading?
}

