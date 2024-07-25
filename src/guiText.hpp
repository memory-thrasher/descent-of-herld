/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include "gpuShared.hpp"
#include "../generated/font.hpp"

//!!include me
//!!onion all

#define FILE_ID 10001000

// This is for static text. There should eventually be a dynamic text renderable that has staging areas and a compute shader to render strings into direct buffers (like the WITE test).
namespace doh {

  constexpr ::WITE::meshWrapper<gpuId, WITE::UDM::RG8uint, sizeof(font_triangles)/2, FLID> fontMesh = font_triangles;
  constexpr size_t guiText_maxCharsPerString = 128;

  constexpr WITE::objectLayout OL_guiText = { .id = FLID };
  //!!append OL_all OL_guiText
  //!!genObj OL_guiText

  struct guiTextInstance_t {
    glm::vec4 bbox;
  };

  struct guiTextStyle_t {
    glm::vec4 color, charMetric;
  };

  constexpr WITE::bufferRequirements BR_guiTextInstance = WITE::simpleUB<gpuId, FLID, sizeof(guiTextInstance_t)>::value,
	      BR_guiTextStyle = WITE::swappableUB<gpuId, FLID, sizeof(guiTextStyle_t)>::value,
	      BR_textIndirect = {
		.deviceId = gpuId,
		.id = FLID,
		.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndirectBuffer,
		.size = 16 + guiText_maxCharsPerString * sizeof(vk::drawIndirectCommand),
		.frameswapCount = 1,
		.hostVisible = false,
	      };
  static_cast(guiText_maxCharsPerString % 16 == 0);
  //!!append BR_all BR_guiTextInstance
  //!!append BR_all BR_guiTextStyle
  //!!append BR_all BR_textIndirect

  constexpr WITE::resourceSlot RS_guiTextInstance = {
    .id = FLID,
    .requirementId = BR_guiTextInstance.id,
    .objectLayoutId = OL_guiText.id,
  }, RS_guiTextStyle = {
    .id = FLID,
    .requirementId = BR_guiTextStyle.id,
    .objectLayoutId = OL_guiText.id,
    .external = true,
  }, RS_guiTextIndirect = {
    .id = FLID,
    .requirementId = BR_guiTextIndirect.id,
    .objectLayoutId = OL_guiText.id,
  }, RS_guiText_all[] = {
    RS_guiTextInstance,
    RS_guiTextStyle,
    RS_guiTextIndirect,
  };
  //!!append RS_all RS_guiText_all
  //!!genObjWrite RS_S_guiTextInstance writeInstanceData guiTextInstance_t
  //!!genObjSet RS_guiTextStyle setStyle WITE::buffer<BR_guiTextStyle>
  //!!genObjWrite RS_S_guiTextIndirect writeIndirectBuffer vk::drawIndirectCommand[guiText_maxCharsPerString]

  constexpr WITE::resourceConsumer RC_S_guiText_instance = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment>::value,
	      RC_S_guiText_style = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eFragment>::value,
	      RC_S_guiText_indirect = WITE::indirectConsumer<FLID>::value,
	      RC_S_guiText_sourceAll[] = {
		RC_S_guiText_instance,
		RC_S_guiText_style,
	      };

}
