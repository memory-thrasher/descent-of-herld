/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

const uint invocations = 32, maxVertices = 128;//146 is effective max on my 3090ti when using vec3 or uint out (declared max of 1024). 204 using gl_PrimitiveID to sneak the star typ ethrough to fragment

layout(points, invocations=invocations) in;
layout(points, max_vertices=maxVertices) out;

// layout(std140, set = 0, binding = 0) uniform data_t {
//   vec4 extents;//LTRB snorm screen
// } rectData;

layout(std140, set = 1, binding = 0) uniform cameraTransform_t {
  layout(row_major) mat4x3 transform;
  uvec4 chunk;
  uvec4 sector;
} cameraTransform;

layout(std140, set = 1, binding = 1) uniform cameraData_t {
  vec4 geometry;//xy = size pixels, zw = cotan(fov.xy/2)
  uvec4 renderDistances;
} cameraData;

layout(location = 0) in uvec4[] planeMin;//min.xy, z = planeZ, w = unused

// void debugEmitToColor(uint value, uint y) {
//   gl_Position = vec4(0, y * 0.002f, 1, 1);
//   gl_PrimitiveID = int(((value >> 8) & 0xFFFF00) | 0xFF);
//   EmitVertex();
//   gl_Position = vec4(0.001f, y * 0.002f, 1, 1);
//   gl_PrimitiveID = int(((value << 8) & 0xFFFF00) | 0xFF);
//   EmitVertex();
// }

// void debugEmitToColor(float value, uint y) {
//   debugEmitToColor(floatBitsToUint(value), y);
//   gl_Position = vec4(0.002f, y * 0.002f, 1, 1);
//   gl_PrimitiveID = int((clamp(int(value / 65536), 0, 255) << 16) | (clamp(int(value / 256), 0, 255) << 8) | 0xFF);
//   EmitVertex();
//   gl_Position = vec4(0.003f, y * 0.002f, 1, 1);
//   gl_PrimitiveID = int((clamp(int(value), 0, 255) << 16) | (clamp(int(value * 256), 0, 255) << 8) | 0xFF);
//   EmitVertex();
//   gl_Position = vec4(0.004f, y * 0.002f, 1, 1);
//   gl_PrimitiveID = int((clamp(int(value * 65536), 0, 255) << 16) | (clamp(int(value * 65536*256), 0, 255) << 8) | 0xFF);
//   EmitVertex();
// }

void main() {
  if(planeMin[0].w == 0) return;
  const uvec3 bboxSize = uvec3(gl_in[0].gl_Position.xyz + (0.5f).xxx);//z = area
  const uvec2 workRange = bboxSize.z * uvec2(0, 1) / (planeMin[0].w * planeMin[0].w);
  uint vertsEmitted = 0;
  for(uint w = workRange.x;w < workRange.y && vertsEmitted < maxVertices;++w) {
    //mix in a rough approximation of the camera's location within the sector before projecting
    const vec3 pnt = ((vec3(ivec3(planeMin[0].xyz + uvec3((w * planeMin[0].w) % bboxSize.x, (w * planeMin[0].w * planeMin[0].w) / bboxSize.x, 0) - cameraTransform.sector.xyz)) - (cameraTransform.chunk.xyz >> 16) / 65536.0f) * cameraTransform.transform).xyz;
    const vec2 screenPnt = pnt.xy / (pnt.z * cameraData.geometry.zw);
    if(pnt.z <= cameraData.renderDistances.w && pnt.z > 0 && all(lessThan(abs(screenPnt.xy), (1).xx))) {
      //TODO point size (need a star diameter var in the star types list, and also vk wants a flag set)
      gl_Position = vec4(screenPnt.xy, 1, 1);
      gl_PrimitiveID = 0xFFFFFF;
      EmitVertex();
      vertsEmitted++;
    }
  }
}

