/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#version 450

layout(std140, set = 0, binding = 0) uniform data_t {
  vec4 extents;//LTRB snorm screen
  vec4 clip;//LTRB in pixels, distance to truncate corners. One possible value would result in an octagon.
  vec4 borderColor;//a=thickness pxls
  vec4 fillColor;//a=0|1
} rectData;

layout(std140, set = 1, binding = 0) uniform cameraData_t {
  vec4 geometry;//xy pixels
} cameraData;

layout(location = 0) in vec2 screenPxlPos;
layout(location = 0) out vec4 outColor;

void main() {
  const vec4 extentsPxl = rectData.extents * cameraData.geometry.xyxy;
  const vec4 pxlWithinRect = vec4(screenPxlPos, extentsPxl.wz) - extentsPxl.xyxy;
  const float borderSize = rectData.borderColor.w;
  bool onBorder = borderSize > 0 && (pxlWithinRect.x < borderSize || pxlWithinRect.y < borderSize || pxlWithinRect.x > pxlWithinRect.w - borderSize || pxlWithinRect.y > pxlWithinRect.z - borderSize);
  bool inCorner = false;
  //manhattan distance from each corner via scaling transform
  if(rectData.clip.x * rectData.clip.y * borderSize > 0) {//ul
    const vec2 rv = pxlWithinRect.xy;
    const vec2 dv = rv / rectData.clip.xy;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (rectData.clip.xy + borderSize.xx);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(rectData.clip.w * rectData.clip.y * borderSize > 0) {//ur
    const vec2 rv = vec2(pxlWithinRect.w - pxlWithinRect.x, pxlWithinRect.y);
    const vec2 dv = rv / rectData.clip.wy;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (rectData.clip.wy + borderSize.xx);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(rectData.clip.x * rectData.clip.z * borderSize > 0) {//ll
    const vec2 rv = vec2(pxlWithinRect.x, pxlWithinRect.z - pxlWithinRect.y);
    const vec2 dv = rv / rectData.clip.xz;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (rectData.clip.xz + borderSize.xx);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(rectData.clip.w * rectData.clip.z * borderSize > 0) {//ul
    const vec2 rv = pxlWithinRect.wz - pxlWithinRect.xy;
    const vec2 dv = rv / rectData.clip.wz;
    inCorner = inCorner || dv.x + dv.y < 1;
    const vec2 bv = rv / (rectData.clip.wz + borderSize.xx);
    onBorder = onBorder || bv.x + bv.y < 1;
  }
  if(inCorner) discard;
  else if(onBorder) outColor = vec4(rectData.borderColor.rgb, 1);
  else if(rectData.fillColor.a > 0) outColor = vec4(rectData.fillColor.rgb, 1);
  else discard;
}

