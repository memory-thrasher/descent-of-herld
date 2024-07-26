/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform instance_t {
  vec4 extents;//LTRB snorm screen
} rectInstance;

layout(std140, set = 0, binding = 1) uniform style_t {
  vec4 clip;//LTRB screen snorm width, distance to truncate corners.
  vec4 borderColor;
  vec4 borderThickness;//LTRB screen snorm width
  vec4 fillColor;//a=0|1 to turn fill on or off
} rectStyle;

layout(std140, set = 1, binding = 0) uniform cameraData_t {
  vec4 geometry;//xy = surface size in pixels
} cameraData;

layout(location = 0) in vec2 screenSnormPos;
layout(location = 0) out vec4 outColor;

void main() {
  const float aspect = cameraData.geometry.x / cameraData.geometry.y;
  const vec4 aspectAdj = vec4(1, aspect, 1, aspect);
  const vec4 clip = rectStyle.clip * aspectAdj;
  const vec4 borderThickness = rectStyle.borderThickness * aspectAdj;
  const vec4 posInRect = vec4(screenSnormPos.xy - rectInstance.extents.xy,
			      rectInstance.extents.zw - screenSnormPos.xy);//zw point relative to rect lr
  bool onBorder = any(lessThan(posInRect, borderThickness));
  bool inCorner = false;
  // manhattan distance from each corner via scaling transform
  if(clip.x * clip.y > 0) {//ul
    const vec2 rv = posInRect.xy;
    const vec2 dv = rv / clip.xy;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (clip.xy + borderThickness.xy);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(clip.z * clip.y > 0) {//ur
    const vec2 rv = posInRect.zy;
    const vec2 dv = rv / clip.zy;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (clip.zy + borderThickness.zy);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(clip.x * clip.w > 0) {//ll
    const vec2 rv = posInRect.xw;
    const vec2 dv = rv / clip.xw;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (clip.xw + borderThickness.xw);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(clip.w * clip.z > 0) {//ul
    const vec2 rv = posInRect.zw;
    const vec2 dv = rv / clip.zw;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (clip.zw + borderThickness.zw);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(inCorner) discard;
  else if(onBorder) outColor = vec4(rectStyle.borderColor.rgb, 1);
  else if(rectStyle.fillColor.a > 0) outColor = vec4(rectStyle.fillColor.rgb, 1);
  else discard;
}

