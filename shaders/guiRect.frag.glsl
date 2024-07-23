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
  vec4 clip;//LTRB in pixels, distance to truncate corners. One possible value would result in an octagon.
  vec4 borderColor;
  vec4 borderThickness;
  vec4 fillColor;//a=0|1
} rectStyle;

layout(std140, set = 1, binding = 0) uniform cameraData_t {
  vec4 geometry;//xy = surface size in pixels
} cameraData;

layout(location = 0) in vec2 screenPxlPos;
layout(location = 0) out vec4 outColor;

void main() {
  const vec4 extentsPxl = (rectInstance.extents + (1).xxxx) * 0.5f * cameraData.geometry.xyxy;
  const vec4 pxlWithinRect = vec4(screenPxlPos, extentsPxl.zw) - extentsPxl.xyxy;
  bool onBorder = (pxlWithinRect.x < rectStyle.borderThickness.x || pxlWithinRect.y < rectStyle.borderThickness.y || pxlWithinRect.x > pxlWithinRect.z - rectStyle.borderThickness.z || pxlWithinRect.y > pxlWithinRect.w - rectStyle.borderThickness.w);
  bool inCorner = false;
  // manhattan distance from each corner via scaling transform
  if(rectStyle.clip.x * rectStyle.clip.y > 0) {//ul
    const vec2 rv = pxlWithinRect.xy;
    const vec2 dv = rv / rectStyle.clip.xy;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (rectStyle.clip.xy + rectStyle.borderThickness.xy);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(rectStyle.clip.z * rectStyle.clip.y > 0) {//ur
    const vec2 rv = vec2(pxlWithinRect.z - pxlWithinRect.x, pxlWithinRect.y);
    const vec2 dv = rv / rectStyle.clip.zy;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (rectStyle.clip.zy + rectStyle.borderThickness.zy);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(rectStyle.clip.x * rectStyle.clip.w > 0) {//ll
    const vec2 rv = vec2(pxlWithinRect.x, pxlWithinRect.w - pxlWithinRect.y);
    const vec2 dv = rv / rectStyle.clip.xw;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (rectStyle.clip.xw + rectStyle.borderThickness.xw);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(rectStyle.clip.w * rectStyle.clip.z > 0) {//ul
    const vec2 rv = pxlWithinRect.zw - pxlWithinRect.xy;
    const vec2 dv = rv / rectStyle.clip.zw;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (rectStyle.clip.zw + rectStyle.borderThickness.zw);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(inCorner) discard;
  else if(onBorder) outColor = vec4(rectStyle.borderColor.rgb, 1);
  else if(rectStyle.fillColor.a > 0) outColor = vec4(rectStyle.fillColor.rgb, 1);
  else discard;
}

