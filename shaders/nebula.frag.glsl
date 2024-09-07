/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

const float nebulaSize = 128;

layout(set = 0, binding = 1) uniform sampler3D tex;

layout(location = 0) in vec4 texLoc;//w = strength

layout(location = 0) out vec4 outColor;

#include cameraStuff.partial.glsl

void main() {
  vec3 camToFNorm = normalize((vec3((gl_FragCoord.xy * 2 / cameraData.geometry.xy - (1).xx) * cameraData.geometry.zw, 1) *
			       cameraTransform.transform).xyz);
  vec3 planeNorm = vec3(equal(uvec3(0, 1, 2), (gl_PrimitiveID / ((nebulaSize + 1) * 2)).xxx));
  outColor = vec4(// texture(tex, texLoc.xyz).x *
		  pow(dot(planeNorm, camToFNorm), 2) * texLoc.w * vec3(1, 1, 1), 1);
}
