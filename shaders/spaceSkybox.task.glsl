/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

#extension GL_EXT_mesh_shader : require

const uint meshWorkgroupOutput = 256;//should match max_vertices in mesh shader

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std140, set = 1, binding = 0) uniform cameraTransform_t {
  layout(row_major) mat4x3 transform;
  uvec4 chunk;
  uvec4 sector;
} cameraTransform;

layout(std140, set = 1, binding = 1) uniform cameraData_t {
  vec4 geometry;//xy = size pixels, zw = cotan(fov.xy/2)
  uvec4 renderDistances;
} cameraData;

taskPayloadSharedEXT uvec4 planeData;//xyz = bbox min, w = packed uint16 bbox size xy

void main() {
  uvec3 bboxMax, bboxMin = bboxMax = cameraTransform.sector.xyz;
  for(int x = -1;x <= 1;++x) {
    for(int y = -1;y <= 1;++y) {
      const uvec3 corner = uvec3(ivec3(cameraTransform.transform * (vec4(vec2(x, y) * cameraData.geometry.zw, 1, 0) * cameraData.renderDistances.w)) + cameraTransform.sector.xyz);
      bboxMax = max(bboxMax, corner);
      bboxMin = min(bboxMin, corner);
    }
  }
  bboxMax += (1).xxx;//correct potential rounding errors
  planeData.z = bboxMin.z + gl_WorkGroupID.x;
  if(planeData.z > bboxMax.z) {
    planeData = (0).xxxx;
  } else {
    //TODO limit bbox by projecting the viewable volume
    planeData.w = 1;
    ((bboxMax.x - bboxMin.x) << 16) | (bboxMax.y - bboxMin.y);
    planeData.xy = bboxMin.xy;
  }
  EmitMeshTasksEXT((bboxMax.x - bboxMin.x) / meshWorkgroupOutput + 1, bboxMax.y - bboxMin.y, planeData.w);
}
