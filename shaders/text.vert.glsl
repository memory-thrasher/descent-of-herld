/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

layout(std140, set = 0, binding = 1) uniform instance_t {
  vec4 bbox;//LTRB
} instance;

layout(std140, set = 0, binding = 0) uniform style_t {
  vec4 color;
  vec4 charMetric;//xy = size of each character in signed normalized screen coords; zw = xy origin within that space
} style;

layout(location = 0) in uvec2 coords;

void main() {
  const float widthChars = floor((instance.bbox.z - instance.bbox.x)/style.charMetric.x);
  const uvec2 charPos = uvec2(mod(gl_InstanceIndex, widthChars), gl_InstanceIndex / widthChars);
  //TODO truncate at height?
  gl_Position = vec4(instance.bbox.xy + style.charMetric.zw + style.charMetric.xy * (charPos + coords * 0.0714f), 0, 1);
}
