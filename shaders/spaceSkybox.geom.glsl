/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

layout(points) in;//TODO invocations? (max=32)
layout(points, max_vertices=256) out;

// layout(std140, set = 0, binding = 0) uniform data_t {
//   vec4 extents;//LTRB snorm screen
// } rectData;

layout(std140, set = 1, binding = 0) uniform cameraTransform_t {
  mat4x3 transform;
  uvec4 chunk;
  uvec4 sector;
} cameraTransform;

layout(std140, set = 1, binding = 1) uniform cameraData_t {
  vec4 geometry;//xy = size pixels, zw = cotan(fov.xy/2)
  vec4 renderDistances;
} cameraData;

//layout(location = 0) out vec2 screenSnormPos;

ivec4 unpackInt4x8(uint i) {
  return ivec4((((i >> 24) & 0xFF) ^ 0x80) - 0x80, (((i >> 16) & 0xFF) ^ 0x80) - 0x80,
	       (((i >>  8) & 0xFF) ^ 0x80) - 0x80, (((i >>  0) & 0xFF) ^ 0x80) - 0x80);
}

void main() {
  //TODO move as much of this as possible to vertex shader
  const uint instanceID = uint(gl_in[0].gl_Position.x);
  const ivec4 rawIncrements = unpackInt4x8(vert.x);
  const ivec3 worldGridIncrements = vert.xyz*vert.w;
  const vec3 rayForward = (vec3(worldGridIncrements.xy, 0) * cameraTransform.transform).xyz;
  const ivec4 rawOrigins = unpackInt4x8(vert.y);
  const ivec4 worldRayOriginGrid = ivec4(rawOrigins.xy, rawOrigins.y, -rawOrigins.x) * rawOrigins.w;
  const vec3 rayRight = (vec3(worldRayOriginGrid.xy, 0) * cameraTransform.transform).xyz;
  const vec3 rayUp = (vec3(worldRayOriginGrid.zw, 0) * cameraTransform.transform).xyz;
  //any point with a star will do, so get one near the camera location to reduce float precision issues
  uvec3 gridOriginInRayUnits;//note: trunction happens each time this is populated
  gridOriginInRayUnits.z = cameraTransform.sector.z / worldGridIncrements.z;
  const uvec2 cameraProjToZ0 = cameraTransform.sector.xy - worldGridIncrements.xy * gridOriginInRayUnits.z;
  gridOriginInRayUnits.y = (cameraProjToZ0.x * worldRayOriginGrid.y - cameraProjToZ0.y * worldRayOriginGrid.x) /
    (worldRayOriginGrid.x^2 + worldRayOriginGrid.y^2);
  gridOriginInRayUnits.x = (cameraProjToZ0.y + gridOriginInRayUnits.y * worldRayOriginGrid.x) / worldRayOriginGrid.y;
  const vec3 gridOrigin = (ivec3(uvec3(gridOriginInRayUnits.x * worldRayOriginGrid.xy + cameraProjToZ0.y * worldRayOriginGrid.zw, 0) + worldRayOriginGrid.z * worldGridIncrements - cameraTransform.sector) * cameraTransform.transform).xyz;
  //need a range of planes that intersect the observable volume. It'll be near one of the five corners.
}

/*
  o.x * rr.xy + o.y * ru.xy = i.xy
  ru.xy = (rr.y, -rr.x)
  o.x * rr.xy + o.y * (rr.y, -rr.x) = i.xy
  o.x * rr.x + o.y * rr.y = i.x
  o.x * rr.y - o.y * rr.x = i.y
  o.x = (i.y + o.y * rr.x) / rr.y
  o.x = (i.x - o.y * rr.y) / rr.x
  (i.y + o.y * rr.x) / rr.y = (i.x - o.y * rr.y) / rr.x
  (i.y + o.y * rr.x) * rr.x = (i.x - o.y * rr.y) * rr.y
  i.y * rr.x + o.y * rr.x^2 = i.x * rr.y - o.y * rr.y^2
  o.y * (rr.x^2 + rr.y^2) = i.x * rr.y - i.y * rr.x
  o.y = (i.x * rr.y - i.y * rr.x) / (rr.x^2 + rr.y^2)
 */

