/*
  Copyright 2024-2025 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

layout(std140, set = 0, binding = 0) uniform data_t {
  vec4 extents;//LTRB snorm screen
} rectData;

layout(location = 0) out vec2 screenSnormPos;

void main() {
  const vec2 snorm = vec2(gl_VertexIndex & 1, gl_VertexIndex > 1 && gl_VertexIndex < 5) *
    (rectData.extents.zw - rectData.extents.xy) + rectData.extents.xy;
  gl_Position = vec4(snorm, 0, 1);
  screenSnormPos = snorm;
}

