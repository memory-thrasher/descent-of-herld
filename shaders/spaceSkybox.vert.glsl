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

layout(location = 0) in ivec4 rayCluster;

layout(location = 0) out uvec4 planeMin;//min.xy, z = planeZ, w = ray spacing (rayCluster.w unpacked)

uvec3 snapToGrid(const uvec3 worldPoint) {
  const uvec3 offsetFromZ0 = rayCluster.xyz * (worldPoint.z / rayCluster.z);
  return uvec3((worldPoint.xy - offsetFromZ0.xy) / planeMin.w * planeMin.w, 0) + offsetFromZ0;
}

void main() {
  planeMin.w = rayCluster.w < 0 ? rayCluster.w + 256 : rayCluster.w;
  uvec3 bboxMax, bboxMin = bboxMax = cameraTransform.sector.xyz;
  for(int x = -1;x <= 1; x += 2) {
    for(int y = -1;y <= 1;y += 2) {
      const uvec3 corner = uvec3(ivec3(cameraTransform.transform * (vec4(vec2(x, y) * cameraData.geometry.zw, 1, 0) * cameraData.renderDistances.w)) + cameraTransform.sector.xyz);
      bboxMax = max(bboxMax, corner);
      bboxMin = min(bboxMin, corner);
    }
  }
  bboxMax = snapToGrid(bboxMax + (1).xxx);
  bboxMin = snapToGrid(bboxMin);
  planeMin.z = gl_InstanceIndex * rayCluster.z + bboxMin.z;
  planeMin.xy = bboxMin.xy + (rayCluster.xy * gl_InstanceIndex) % planeMin.w;
  //TODO prioritize those nearest the camera (z or -z)?
  gl_Position = vec4(bboxMax.xy - bboxMin.xy, (bboxMax.x - bboxMin.x) * (bboxMax.y - bboxMin.y), 1);
  gl_PointSize = 1;
  if(planeMin.z > bboxMax.z) planeMin.w = 0;
}

