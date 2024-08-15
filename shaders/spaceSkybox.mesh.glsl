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

const uint maxVertices = 256, maxInvocations = 32,
  testsPerInvocation = maxVertices / maxInvocations;
const int modulus = 0x1FFF;
const uvec3 seedA = uvec3(37, 139, 199),
  seedB = uvec3(103, 53, 157);

layout(local_size_x = maxInvocations, local_size_y = 1, local_size_z = 1) in;
layout(points, max_vertices=maxVertices, max_primitives=maxVertices) out;

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
shared uint outIdx;

//task spawns a number of mesh workgroups to cover the area bboxsize.xy. Each workgroup is responsible for the strip from x = [gl_WorkGroupID.x * maxVertices, (gl_WorkGroupID.x + 1) * maxVertices) and y = gl_WorkGroupID.y

uint dot(uvec3 a, uvec3 b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

// void main() {
//   if(planeData.w == 0) return;
//   if(gl_LocalInvocationID == (0).xxx)
//     atomicAnd(outIdx, 0);//barrier-free set to 0
//   memoryBarrierShared();
//   uint localCnt = 0;
//   vec4[testsPerInvocation] localFound;
//   //first check how many we are going to write
//   for(uint i = 0;i < testsPerInvocation;i++) {
//     const uvec3 worldPnt = planeData.xyz + uvec3(gl_WorkGroupID.x * maxVertices + gl_LocalInvocationID.x * testsPerInvocation + i, gl_WorkGroupID.y, 0);
//     const vec4 pnt = vec4(((vec3(ivec3(worldPnt - cameraTransform.sector.xyz)) - (cameraTransform.chunk.xyz >> 16) / 65536.0f) * cameraTransform.transform).xyz, (dot(worldPnt, worldPnt * seedA) + dot(worldPnt, seedB)) & modulus);
//     const vec2 screenPnt = pnt.xy / (abs(pnt.z) * cameraData.geometry.zw);
//     const float dstSqrNorm = dot(pnt, pnt) / (cameraData.renderDistances.w * cameraData.renderDistances.w);
//     if(dstSqrNorm <= 1 && pnt.z > 0 && all(lessThan(abs(screenPnt.xy), (1).xx)) && pnt.w < 20) {
//       localFound[localCnt++] = vec4(screenPnt.xy, dstSqrNorm, pnt.w);
//     }
//   }
//   const uint outStart = atomicAdd(outIdx, localCnt);//where this worker will put its found positions.
//   memoryBarrierShared();
//   const uint total = outIdx;
//   SetMeshOutputsEXT(total, total);
//   for(uint i = 0;i < localCnt;i++) {
//     uint dst = outStart + i;
//     gl_MeshPrimitivesEXT[dst].gl_PrimitiveID = int(localFound[i].w + 0.5f);
//     gl_MeshVerticesEXT[dst].gl_Position = vec4(localFound[i].xyz, 1);
//     //TODO point size (need a star diameter var in the star types list, and also vk wants a flag set)
//     gl_PrimitivePointIndicesEXT[dst] = dst;//dumb loop for point output meshes
//   }
// }

void main() {
  if(planeData.w == 0) return;
  if(gl_LocalInvocationID == (0).xxx)
    atomicAnd(outIdx, 0);//barrier-free set to 0
  memoryBarrierShared();
  uint localCnt = 0, localMin = 0xFFFF, localMax = 0;
  //first check how many we are going to write
  for(uint i = 0;i < testsPerInvocation;i++) {
    const uvec3 worldPnt = planeData.xyz + uvec3(gl_WorkGroupID.x * maxVertices + gl_LocalInvocationID.x * testsPerInvocation + i, gl_WorkGroupID.y, 0);
    const vec4 pnt = vec4(((vec3(ivec3(worldPnt - cameraTransform.sector.xyz)) - (cameraTransform.chunk.xyz >> 16) / 65536.0f) * cameraTransform.transform).xyz, (dot(worldPnt, worldPnt * seedA) + dot(worldPnt, seedB)) & modulus);
    const vec2 screenPnt = pnt.xy / (abs(pnt.z) * cameraData.geometry.zw);
    const float dstSqrNorm = dot(pnt, pnt) / (cameraData.renderDistances.w * cameraData.renderDistances.w);
    if(dstSqrNorm <= 1 && pnt.z > 0 && all(lessThan(abs(screenPnt.xy), (1).xx)) && pnt.w < 20) {
      localCnt++;
      if(localMin == 0xFFFF)
	localMin = i;
      localMax = i;
    }
  }
  uint dst = atomicAdd(outIdx, localCnt);//where this worker will put its found positions.
  memoryBarrierShared();
  const uint total = outIdx;
  SetMeshOutputsEXT(total, total);
  // SetMeshOutputsEXT(localCnt, localCnt);
  // uint dst = 0;
  for(uint i = localMin;i <= localMax;i++) {
    const uvec3 worldPnt = planeData.xyz + uvec3(gl_WorkGroupID.x * maxVertices + gl_LocalInvocationID.x * testsPerInvocation + i, gl_WorkGroupID.y, 0);
    const vec3 pnt = vec3(((vec3(ivec3(worldPnt - cameraTransform.sector.xyz)) - (cameraTransform.chunk.xyz >> 16) / 65536.0f) * cameraTransform.transform).xyz);
    gl_MeshPrimitivesEXT[dst].gl_PrimitiveID = int(dot(worldPnt, worldPnt * seedA) + dot(worldPnt, seedB)) & modulus;
    const vec2 screenPnt = pnt.xy / (abs(pnt.z) * cameraData.geometry.zw);
    const float dstSqrNorm = dot(pnt, pnt) / (cameraData.renderDistances.w * cameraData.renderDistances.w);
    if(dstSqrNorm <= 1 && pnt.z > 0 && all(lessThan(abs(screenPnt.xy), (1).xx)) &&
       gl_MeshPrimitivesEXT[dst].gl_PrimitiveID < 20) {
      gl_MeshVerticesEXT[dst].gl_Position = vec4(screenPnt.xy, dstSqrNorm, 1);
      //TODO point size (need a star diameter var in the star types list, and also vk wants a flag set)
      gl_PrimitivePointIndicesEXT[dst] = dst;//dumb loop for point output meshes
      dst++;
    }
  }
}

//groups render invocations modulus: fps, density (when measured)
//800g, 25r, 32inv, 1FFFmod: 1430
//800g, 250r, 1inv, 1FFFmod: 37
//800g, 250r, 16inv, 1FFFmod: 320
//800g, 250r, 32inv, 1FFFmod: 312
//800g, 250r, 64inv, 1FFFmod: 188
//800g, 250r, 128inv, 1FFFmod: 100
//switching to two-pass (no local storage)
//800g, 250r, 1inv, 1FFFmod: 35 (specialized single-pass)
//800g, 250r, 16inv, 1FFFmod: 310
//800g, 250r, 32inv, 1FFFmod: 335
//800g, 250r, 64inv, 1FFFmod: 200
//300g, 100r, 32inv,  7FFmod: 1430 23k
//300g, 100r, 32inv,  1FFmod: 1430 90k but visible lines from low depth
//450g, 150r, 32inv,   FFmod: 1200 619k but visible lines from low depth
//450g, 150r, 32inv,  3FFmod: 1100 154k banding
//380g, 150r, 32inv,  3FFmod: 1200 banding
//350g, 250r, 32inv,  1FFmod: 1200 banding
//700g, 250r, 32inv, 1FFFmod: 380 90k

