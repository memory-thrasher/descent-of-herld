/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

const uint instances = 512, invocations = 32, maxVertices = 256;//sync with spaceSkybox.hpp

layout(points, invocations=invocations) in;
layout(points, max_vertices=maxVertices) out;

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
  uvec4 renderDistances;
} cameraData;

layout(location = 0) in uvec2[] vert;

//layout(location = 0) out vec2 screenSnormPos;

ivec4 unpackInt4x8(uint i) {
  return ivec4((((i >> 24) & 0xFF) ^ 0x80) - 0x80, (((i >> 16) & 0xFF) ^ 0x80) - 0x80,
	       (((i >>  8) & 0xFF) ^ 0x80) - 0x80, (((i >>  0) & 0xFF) ^ 0x80) - 0x80);
}

void main() {
  //
  // const vec3 gridOrigin = vec3(0, 0, 0);//in camera space
  // //
  // const ivec4 rawIncrements = unpackInt4x8(vert[0].x);
  // const ivec3 worldGridIncrements = rawIncrements.xyz*rawIncrements.w;
  // const ivec4 rawOrigins = unpackInt4x8(vert[0].y);
  // const ivec2 worldRayOriginGrid = rawOrigins.xy * rawOrigins.w;
  // const mat3 camToRay = mat3();
  // ivec3 bboxMin = (gridOrigin).xxx, bboxMax = bboxMin;
  // for(int x = -1;x <= 1; x += 2) {
  //   for(int y = -1;y <= 1;y += 2) {
  //     const ivec3 corner = ivec3(/*camToRay * */(cameraData.renderDistances.w * vec3(x * cameraData.geometry.z, y * cameraData.geometry.w, 1) - gridOrigin) /* * camToRay */);
  //     bboxMax = max(bboxMax, corner);
  //     bboxMin = min(bboxMin, corner);
  //   }
  // }
  // bboxMax += (1).xxx;//many truncations happen above
  // // const ivec3 bboxMin = ivec3(-350, -350, 0), bboxMax = ivec3(350, 350, 700), lineGridSize = bboxMax - bboxMin;
  // // const vec3 rayUp = vec3(12, 42, 0), rayRight = vec3(42, -12, 0), rayForward = vec3(12, 42, 18) * 19;
  // //
  // const ivec2 lineGridSize = bboxMax.xy - bboxMin.xy;
  // const uint lines = lineGridSize.x * lineGridSize.y;
  // const uint linesPerWorker = max(uint(lines / float(invocations * instances) + 0.5f), 1);
  // //stuff below this line is not shared between instances and cannot be moved to the vertex shader
  // const uint workerId = uint(gl_in[0].gl_Position.x) * invocations + gl_InvocationID;
  // const uint endLine = min(linesPerWorker * (workerId + 1), lines);
  // uint vertsEmitted = 0;
  // for(uint lineId = linesPerWorker * workerId;lineId < endLine && vertsEmitted < maxVertices;lineId++) {
  //   const vec3 lineOrigin = gridOrigin + camToRay[1] * int(bboxMin.y + lineId / lineGridSize.x) + camToRay[0] * int(bboxMin.x + lineId % lineGridSize.x);
  //   for(int pntId = bboxMin.z;pntId <= bboxMax.z && vertsEmitted < maxVertices;++pntId) {
  //     //TODO refine min and max values for this ray's intersection?
  //     const vec3 pnt = lineOrigin + camToRay[2] * pntId;
  //     const vec2 screenPnt = cameraData.geometry.zw * pnt.xy / abs(pnt.z);
  //     if(all(lessThanEqual(abs(screenPnt.xy), (1).xx)) && pnt.z > 0 && pnt.z <= cameraData.renderDistances.w) {
  // 	gl_Position = vec4(screenPnt, 1, 1);
  // 	EmitVertex();
  // 	vertsEmitted++;
  //     }
  //   }
  // }
  //
  //TODO move as much of this as possible to vertex shader
  // const ivec4 rawIncrements = unpackInt4x8(vert[0].x);
  // const ivec3 worldGridIncrements = rawIncrements.xyz*rawIncrements.w;
  // const ivec4 rawOrigins = unpackInt4x8(vert[0].y);
  // const ivec4 worldRayOriginGrid = ivec4(rawOrigins.xy, rawOrigins.y, -rawOrigins.x) * rawOrigins.w;
  // //any point with a star will do, so get one near the camera location to reduce float precision issues
  // uvec3 gridOriginInRayUnits;//note: trunction happens each time this is populated
  // gridOriginInRayUnits.z = cameraTransform.sector.z / worldGridIncrements.z;
  // const uvec2 cameraProjToZ0 = cameraTransform.sector.xy - worldGridIncrements.xy * gridOriginInRayUnits.z;
  // gridOriginInRayUnits.y = (cameraProjToZ0.x * worldRayOriginGrid.y - cameraProjToZ0.y * worldRayOriginGrid.x) /
  //   (worldRayOriginGrid.x^2 + worldRayOriginGrid.y^2);
  // gridOriginInRayUnits.x = (cameraProjToZ0.y + gridOriginInRayUnits.y * worldRayOriginGrid.x) / worldRayOriginGrid.y;
  // const vec3 gridOrigin = (ivec3(uvec3(gridOriginInRayUnits.x * worldRayOriginGrid.xy + cameraProjToZ0.y * worldRayOriginGrid.zw, 0) + worldRayOriginGrid.z * worldGridIncrements - cameraTransform.sector.xyz) * cameraTransform.transform).xyz;
  // //TODO ^ account for chunk location as fractional system
  // //need a range of planes that intersect the observable volume.
  // const float rayOriginLenSqr = dot(worldRayOriginGrid.xy, worldRayOriginGrid.xy);
  // const vec3 rayRight = (vec3(worldRayOriginGrid.xy, 0) * cameraTransform.transform).xyz,
  //   rayUp = (vec3(worldRayOriginGrid.zw, 0) * cameraTransform.transform).xyz,
  //   rayForward = (vec3(worldGridIncrements.xy, 0) * cameraTransform.transform).xyz,
  //   rayRightScaled = rayRight / rayOriginLenSqr,
  //   rayUpScaled = rayUp / rayOriginLenSqr,
  //   rayForwardScaled = rayForward / dot(worldGridIncrements.xy, worldGridIncrements.xy),
  //   offset = vec3(dot(rayRightScaled, -gridOrigin), dot(rayUpScaled, -gridOrigin), dot(rayForwardScaled, -gridOrigin));
  // const mat4x3 camToRay = mat4x3(vec4(rayRightScaled, offset.x), vec4(rayUpScaled, offset.y), vec4(rayForwardScaled, offset.z));
  // ivec3 bboxMin = ivec3(offset), bboxMax = bboxMin;
  // for(int x = -1;x <= 1; x += 2) {
  //   for(int y = -1;y <= 1;y += 2) {
  //     const ivec3 corner = ivec3(camToRay * vec4(cameraData.renderDistances.w * vec3(x * cameraData.geometry.z, y * cameraData.geometry.w, 1), 1));
  //     bboxMax = max(bboxMax, corner);
  //     bboxMin = min(bboxMin, corner);
  //   }
  // }
  // bboxMax += (1).xxx;//many truncations happen above
  // const ivec2 lineGridSize = bboxMax.xy - bboxMin.xy;
  // const uint lines = lineGridSize.x * lineGridSize.y;
  // const uint linesPerWorker = max(uint(lines / float(invocations * instances) + 0.5f), 1);
  // //stuff below this line is not shared between instances and cannot be moved to the vertex shader
  // const uint workerId = uint(gl_in[0].gl_Position.x) * invocations + gl_InvocationID;
  // const uint endLine = min(linesPerWorker * (workerId + 1), lines);
  // uint vertsEmitted = 0;
  // for(uint lineId = linesPerWorker * workerId;lineId < endLine && vertsEmitted < maxVertices;lineId++) {
  //   const vec3 lineOrigin = gridOrigin + rayUp * int(bboxMin.y + lineId / lineGridSize.x) + rayRight * int(bboxMin.x + lineId % lineGridSize.x);
  //   for(int pntId = bboxMin.z;pntId <= bboxMax.z && vertsEmitted < maxVertices;++pntId) {
  //     //TODO refine min and max values for this ray's intersection?
  //     const vec3 pnt = lineOrigin + rayForward * pntId;
  //     const vec2 screenPnt = cameraData.geometry.zw * pnt.xy / abs(pnt.z);
  //     if(all(lessThanEqual(abs(screenPnt.xy), (1).xx)) && pnt.z > 0 && pnt.z <= cameraData.renderDistances.w) {
  // 	gl_Position = vec4(screenPnt, 1, 1);
  // 	EmitVertex();
  // 	vertsEmitted++;
  //     }
  //   }
  // }
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

