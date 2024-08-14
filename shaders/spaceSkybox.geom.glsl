/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

const uint invocations = 16, maxVertices = 16, spacing = 256;

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

layout(location = 0) in ivec3[] rayCluster; //xyz = delta between (potential) stars along each ray with origins every 1024 along plane Z=0
layout(location = 1) in uvec4[] planeInfo;//xy = min ray base location, z = depth along ray of plane, w = bool enabled

void main() {
  if(planeInfo[0].w == 0) return;
  const uvec3 bboxSize = uvec3(gl_in[0].gl_Position.xyz + (0.5f).xxx);//z = area
  const uvec2 workRange = bboxSize.z * uvec2(gl_InvocationID, gl_InvocationID+1) / invocations;
  uint vertsEmitted = 0;
  const uvec4 raySpacing = uvec4(bboxSize, 1) * spacing;
  const uvec3 origin = uvec3(planeInfo[0].xy * raySpacing.w, 0) + rayCluster[0].xyz * planeInfo[0].z;
  const ivec4 prioritySignAndOffset = mix(ivec4(1, 1, origin.xy), ivec4(-1, -1, origin.xy + raySpacing.xy),
					  greaterThan(cameraTransform.sector.xy, origin.xy + (raySpacing.xy >> 1)).xyxy);
  for(uint w = workRange.x;w < workRange.y && vertsEmitted < maxVertices;++w) {
    //mix in a rough approximation of the camera's location within the sector before projecting
    const uvec3 worldPnt = uvec3(ivec2(w % bboxSize.x, w / bboxSize.x) * prioritySignAndOffset.xy * raySpacing.w + prioritySignAndOffset.zw, origin.z);
    gl_PrimitiveID = int(worldPnt.x*worldPnt.x*3 + worldPnt.x*7 + worldPnt.y*worldPnt.y*13 + worldPnt.y*17 + worldPnt.z*worldPnt.z*23 + worldPnt.z*29) & 0x1F;
    const vec3 pnt = ((vec3(ivec3(worldPnt - cameraTransform.sector.xyz)) - (cameraTransform.chunk.xyz >> 16) / 65536.0f) * cameraTransform.transform).xyz;
    const vec2 screenPnt = pnt.xy / (abs(pnt.z) * cameraData.geometry.zw);
    const float dstSqrNorm = dot(pnt, pnt) / (cameraData.renderDistances.w * cameraData.renderDistances.w);
    if(dstSqrNorm <= 1 && pnt.z > 0 && all(lessThan(abs(screenPnt.xy), (1).xx)) && gl_PrimitiveID < 20) {
      //TODO point size (need a star diameter var in the star types list, and also vk wants a flag set)
      gl_Position = vec4(screenPnt.xy, dstSqrNorm, 1);
      EmitVertex();
      vertsEmitted++;
    }
  }
}

//1024g 64inst 128v 32inv: 295
//no skybox fps: 1430
//single grid 128v 32inv 64inst: 1430
//1g 64inst 256v 32inv: 1430
//16g 64inst 256v 32inv: 1430
//16g 16inst 256v 32inv: 1430  can see planes spawning
//note: added plane priority and depth shading here, no immediate change on fps
//16g 16inst 256v 32inv: 1430 can still see planes spawning
//16g 32inst 256v 32inv: 1430 no spawning
//1024g 32inst 256v 32inv: 230 visible gaps
//1024g 1inst 256v 32inv: 1430 no visible stars
//64g 16inst 256v 32inv: 1430 low density, gap hard to see
//32g 32inst 256v 32inv: 1430 low density, gap gone
//128g 16inst 256v 32inv, new seed with only 1024 spacing: 1430, very low density, visible spawning
//128g 16inst 16v 32inv 1024sp: 1430, very low density, visible spawning
//128g 64inst 16v 32inv 1024sp: 1430, very low density, spawning gone
//128g 32inst 16v 32inv 1024sp: 1430, very low density, spawning gone
//1024g 32inst 16v 32inv 1024sp: 1430, moderate density, spawning gone, bands gone
//8192g 32inst 16v 32inv 1024sp: 1180, better density, spawning gone, moderate bands
//8192g 32inst 32v 32inv 1024sp: 600, better density, spawning gone, moderate bands
//8192g 32inst 8v 32inv 1024sp: 1420, better density, spawning gone, mild bands
//8192g 32inst 8v 8inv 1024sp: 1430, better density, spawning gone, mild bands
//now switching to runtime parameter randomization
//8192g 32inst 8v 8inv 1024sp: 1420, very very low density
//1g 32inst 256v 32inv 128sp: 1420 very grid as expected
//8192g 32inst 256v 32inv 128sp: 30 extreme banding
//8192g 32inst 256v 32inv 512sp: 30 extreme banding
//8192g 32inst 256v 32inv 512sp: 30 low density
//8192g 32inst 8v 8inv 512sp: 1400 low density
//8192g 32inst 8v 8inv 8sp: 30 distinct banding
//8192g 16inst 8v 8inv 1024sp: 1430 very low density
//8192g 16inst 8v 8inv 256sp: 1420 moderate density
//8192g 16inst 8v 8inv 64sp: 1200 distinct banding
//8192g 16inst 8v 32inv 64sp: 500 good density, moderate banding
//8192g 16inst 16v 16inv 64sp: 500 good density, moderate banding
//8192g 16inst 32v 8inv 64sp: 250 good density, moderate banding
//65536g 16inst 8v 8inv 256sp: 700 moderate density mild banding visible spawning
//65536g 16inst 8v 8inv 128sp: 600 moderate density mild banding visible spawning
//65536g 64inst 8v 8inv 128sp: 184 good density mild banding
//8192g 64inst 8v 8inv 128sp: 1200 good density, mild banding, very visible grid region flickering
//8192g 64inst 16v 16inv 128sp: 700 good density, mild banding
//8192g 16inst 16v 16inv 256sp: 1430 good density, mild banding, visible spawning
//8192g 32inst 32v 16inv 256sp: 1000 good density, very little banding
//8192g 16inst 32v 16inv 256sp: 1200 good density, visible spawning
//8192g 32inst 16v 16inv 256sp: 1200 good density
//65536g 32inst 16v 16inv 256sp: 200 good density
//rethinking cluister parameters
