/*
  Copyright 2024-2025 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include <cstdio>
#include <cstdarg>

#include "gpuShared.hpp"
#include "../generated/font.hpp"

//!!include me
//!!onion all

#define FILE_ID 10001000

// This is for static text. There should eventually be a dynamic text renderable that has staging areas and a compute shader to render strings into direct buffers (like the WITE test).
namespace doh {

  constexpr ::WITE::meshWrapper<gpuId, WITE::UDM::R8uint, sizeof(font_lines), FLID> fontMesh = font_lines;
  constexpr size_t guiText_maxCharsPerString = 128;
  //!!append BR_all fontMesh.bufferRequirements_v

  constexpr WITE::objectLayout OL_guiText = { .id = FLID };
  //!!append OL_all OL_guiText
  //!!genObj OL_guiText

  struct guiTextInstance_t {
    glm::vec4 bbox;
  };

  struct guiTextStyle_t {
    glm::vec4 color, charMetric;
  };

  struct guiTextIndirectBuffer_t {
    glm::uvec4 size;//mostly pad
    vk::DrawIndirectCommand drawCommands[guiText_maxCharsPerString];
  };

  int guiTextFormat(guiTextIndirectBuffer_t& out, const char* format, ...);
  decltype(fontMesh)::buffer_t& fontMeshBuffer();

  constexpr WITE::bufferRequirements BR_guiTextInstance = WITE::swappableUB<gpuId, FLID, sizeof(guiTextInstance_t)>::value,
	      BR_guiTextStyle = WITE::swappableUB<gpuId, FLID, sizeof(guiTextStyle_t)>::value,
	      BR_guiTextIndirect = {
		.deviceId = gpuId,
		.id = FLID,
		.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndirectBuffer,
		.size = sizeof(guiTextIndirectBuffer_t),
		.frameswapCount = 2,
		.hostVisible = false,
	      };
  static_assert(guiText_maxCharsPerString % 16 == 0);
  //!!append BR_all BR_guiTextInstance
  //!!append BR_all BR_guiTextStyle
  //!!append BR_all BR_guiTextIndirect

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
  }, RS_guiTextMesh = {
    .id = FLID,
    .requirementId = fontMesh.bufferRequirements_v.id,
    .objectLayoutId = OL_guiText.id,
    .external = true,
  }, RS_guiText_all[] = {
    RS_guiTextInstance,
    RS_guiTextStyle,
    RS_guiTextIndirect,
    RS_guiTextMesh,
  };
  //!!append RS_all RS_guiText_all
  //!!genObjSlowWrite RS_guiTextInstance writeInstanceData guiTextInstance_t
  //!!genObjSlowWrite RS_guiTextIndirect writeIndirectBuffer guiTextIndirectBuffer_t
  //!!genObjSet RS_guiTextStyle setStyle WITE::buffer<BR_guiTextStyle>
  //!!genObjSingleton RS_guiTextMesh &fontMeshBuffer()

  constexpr WITE::resourceConsumer RC_S_guiText_instance = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eVertex>::value,
	      RC_S_guiText_style = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment>::value,
	      RC_S_guiText_indirect = WITE::indirectConsumer<FLID>::value,
	      RC_S_guiText_indirectCount = WITE::indirectCountConsumer<FLID>::value,
	      RC_S_guiText_sourceAll[] = {
		RC_S_guiText_style,
		RC_S_guiText_instance,
		RC_S_guiText_indirect,
		RC_S_guiText_indirectCount,
		fontMesh.resourceConsumer_v
	      };

#include "text.vert.spv.h"
#include "solidColor.frag.spv.h"

  constexpr WITE::shaderModule SM_L_guiText[] {
    { text_vert, sizeof(text_vert), vk::ShaderStageFlagBits::eVertex },
    { solidColor_frag, sizeof(solidColor_frag), vk::ShaderStageFlagBits::eFragment },
  };

  constexpr WITE::graphicsShaderRequirements S_guiText {
    .id = FLID,
    .modules = SM_L_guiText,
    .sourceProvidedResources = RC_S_guiText_sourceAll,
    .topology = vk::PrimitiveTopology::eLineList,
    .cullMode = vk::CullModeFlagBits::eNone,
  };
  //!!append S_RP_gui S_guiText

  constexpr WITE::resourceReference RR_L_guiText[] = {
    { .resourceConsumerId = RC_S_guiText_indirectCount.id, .resourceSlotId = RS_guiTextIndirect.id, .subresource = {0, 16} },
    { .resourceConsumerId = RC_S_guiText_indirect.id, .resourceSlotId = RS_guiTextIndirect.id, .subresource = {16} },
    { RC_S_guiText_style.id, RS_guiTextStyle.id },
    { RC_S_guiText_instance.id, RS_guiTextInstance.id },
    { fontMesh.resourceConsumer_v.id, RS_guiTextMesh.id },
  };

  constexpr WITE::sourceLayout SL_guiText = {
    .id = FLID,
    .objectLayoutId = OL_guiText.id,
    .resources = RR_L_guiText,
  };
  //!!append SL_all SL_guiText

}

#undef FILE_ID

