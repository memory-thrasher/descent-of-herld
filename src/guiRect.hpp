/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

//!!include me
//!!onion all

struct guiRect_t {
  glm::vec4 extents;//LTRB snorm screen
  glm::vec4 clip;//LTRB in pixels, distance to truncate corners. One possible value would result in an octagon.
  glm::vec4 borderColor;//a=thickness pxls
  glm::vec4 fillColor;//a=0|1
};//no staging, set with out of band when creating. Rectangles should not move

constexpr bufferRequirements BR_guiRect = WITE::simpleUB<gpuId, 10000001, sizeof(guiRect_t)>::value;
//!!register BR BR_guiRect

constexpr resourceConsumer RC_guiRect_S_source = WITE::simpleUBConsumer<10000101, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment>::value;

constexpr resourceConsumer RC_guiRect_S_target = WITE::simpleUBConsumer<10000102, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment>::value;


