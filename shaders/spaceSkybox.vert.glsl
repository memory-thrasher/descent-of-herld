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

layout(location = 0) out ivec3 rayCluster;//xyz = delta between (potential) stars along each ray with origins every 2^z along plane Z=0
layout(location = 1) out uvec4 planeInfo;//xy = min ray base location, z = depth along ray of plane, w = unused

const uint raySpacing = 256;

void main() {
  // rayCluster = ivec3(int(((gl_VertexIndex + 12583) * 62053) & 0x7F) - 64,
  // 		     int(((gl_VertexIndex +  5153) * 48821) & 0x7F) - 64,
  // 		     int(((gl_VertexIndex + 15131) * 51539) & 0x3F) + 64);
  rayCluster = (((gl_VertexIndex.xxx + ivec3(-12583, 5153, 15131)) * ivec3(62053, 48821, 51539)) & ivec3(0x7F.xx, 0x3F)) + ivec3(-64, -64, 64);
  uvec3 bboxMax, bboxMin = bboxMax = cameraTransform.sector.xyz;
  for(int x = -1;x <= 1;++x) {
    for(int y = -1;y <= 1;++y) {
      const uvec3 corner = uvec3(ivec3(cameraTransform.transform * (vec4(vec2(x, y) * cameraData.geometry.zw, 1, 0) * cameraData.renderDistances.w)) + cameraTransform.sector.xyz);
      bboxMax = max(bboxMax, corner);
      bboxMin = min(bboxMin, corner);
    }
  }
  const uvec3 minOffsetFromZ0 = uvec3(rayCluster.xy, 1) * (bboxMin.z / rayCluster.z);
  const uint planeCount = bboxMax.z / rayCluster.z - minOffsetFromZ0.z + 1;
  if(gl_InstanceIndex > planeCount) {
    planeInfo.w = 0;
    return;
  }
  planeInfo.w = 1;
  const uint planeId = cameraTransform.sector.z > ((bboxMax.z + bboxMin.z) >> 1) ? planeCount - gl_InstanceIndex - 1 : gl_InstanceIndex;
  planeInfo.xy = (bboxMin.xy - minOffsetFromZ0.xy - rayCluster.xy * planeId) / raySpacing;
  planeInfo.z = minOffsetFromZ0.z + planeId;
  const uvec2 size = (bboxMax.xy - bboxMin.xy) / raySpacing + (1).xx;
  //TODO prioritize those nearest the camera (z or -z)?
  gl_Position = vec4(size.xy, size.x * size.y, 1);
  gl_PointSize = 1;
}

