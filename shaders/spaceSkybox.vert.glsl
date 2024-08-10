/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

layout(std140, set = 1, binding = 0) uniform cameraTransform_t {
  layout(row_major) mat4x3 transform;
  uvec4 chunk;
  uvec4 sector;
} cameraTransform;

layout(std140, set = 1, binding = 1) uniform cameraData_t {
  vec4 geometry;//xy = size pixels, zw = cotan(fov.xy/2)
  uvec4 renderDistances;
} cameraData;

layout(location = 0) in ivec4 rayCluster; //xyz = delta between (potential) stars along each ray with origins every 2^z along plane Z=0

layout(location = 0) out uvec4 rayClusterOut;//ditto
layout(location = 1) out uvec4 planeInfo;//xy = min ray base location, z = depth along ray of plane, w = unused

// uvec3 snapToGrid(const uvec3 worldPoint) {
//   const uvec3 offsetFromZ0 = rayCluster.xyz * (worldPoint.z / rayCluster.z);
//   return uvec3((worldPoint.xy - offsetFromZ0.xy) / planeMin.w * planeMin.w, 0) + offsetFromZ0;
// }

#define raySpacing (1 << rayCluster.w)

void main() {
  uvec3 bboxMax, bboxMin = bboxMax = cameraTransform.sector.xyz;
  for(int x = -1;x <= 1;++x) {
    for(int y = -1;y <= 1;++y) {
      const uvec3 corner = uvec3(ivec3(cameraTransform.transform * (vec4(vec2(x, y) * cameraData.geometry.zw, 1, 0) * cameraData.renderDistances.w * 1.5f)) + cameraTransform.sector.xyz);
      bboxMax = max(bboxMax, corner);
      bboxMin = min(bboxMin, corner);
    }
  }
  // uvec3 bboxMin = cameraTransform.sector.xyz - (700).xxx, bboxMax = bboxMin + (1400).xxx;
  const uvec3 minOffsetFromZ0 = uvec3(rayCluster.xy, 1) * (bboxMin.z / rayCluster.z);
  planeInfo.xy = (bboxMin.xy - minOffsetFromZ0.xy - rayCluster.xy * gl_InstanceIndex) / raySpacing;
  planeInfo.z = minOffsetFromZ0.z + gl_InstanceIndex;
  rayClusterOut = rayCluster;
  const uvec2 size = (bboxMax.xy - bboxMin.xy) / raySpacing + (1).xx;
  //TODO prioritize those nearest the camera (z or -z)?
  gl_Position = vec4(size.xy, size.x * size.y, 1);
  gl_PointSize = 1;
  planeInfo.w = (planeInfo.z * rayCluster.z > bboxMax.z) ? 0 : 1;
}

