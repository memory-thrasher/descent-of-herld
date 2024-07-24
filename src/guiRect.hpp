/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once
//!!include me
//!!onion all

#include "targetPrimary.hpp"

#define FILE_ID 10000000

namespace doh {

  constexpr WITE::objectLayout OL_guiRect = { .id = FLID };
  //!!append OL_all OL_guiRect
  //!!genObj OL_guiRect

  struct guiRectInstance_t {
    glm::vec4 extents;//LTRB snorm screen
  };

  struct guiRectStyle_t {
    glm::vec4 clip;//LTRB in pixels, distance to truncate corners. One possible value would result in an octagon.
    glm::vec4 borderColor;
    glm::vec4 borderThickness;//LTRB
    glm::vec4 fillColor;//a=0|1
  };

  constexpr WITE::bufferRequirements BR_guiRectInstance = WITE::simpleUB<gpuId, FLID, sizeof(guiRectInstance_t)>::value,
	      BR_S_guiRectInstance = WITE::withId(WITE::stagingRequirementsFor(BR_guiRectInstance, 2), FLID);
  //!!append BR_all BR_guiRectInstance
  //!!append BR_all BR_S_guiRectInstance

  constexpr WITE::bufferRequirements BR_guiRectStyle = WITE::swappableUB<gpuId, FLID, sizeof(guiRectStyle_t)>::value;
  //!!append BR_all BR_guiRectStyle

  constexpr WITE::resourceSlot RS_guiRectInstance = {
    .id = FLID,
    .requirementId = BR_guiRectInstance.id,
    .objectLayoutId = OL_guiRect.id,
  }, RS_S_guiRectInstance = {
    .id = FLID,
    .requirementId = BR_S_guiRectInstance.id,
    .objectLayoutId = OL_guiRect.id,
  }, RS_guiRectStyle = {
    .id = FLID,
    .requirementId = BR_guiRectStyle.id,
    .objectLayoutId = OL_guiRect.id,
    .external = true,
  }, RS_guiRect_all[] = {
    RS_guiRectInstance,
    RS_S_guiRectInstance,
    RS_guiRectStyle,
  };
  //!!append RS_all RS_guiRect_all
  //!!genObjWrite RS_S_guiRectInstance writeInstanceData guiRectInstance_t
  //!!genObjSet RS_guiRectStyle setStyle WITE::buffer<BR_guiRectStyle>

  constexpr WITE::resourceConsumer RC_S_guiRect_instance = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment>::value,
	      RC_S_guiRect_style = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eFragment>::value,
	      RC_S_guiRect_sourceAll[] = {
		RC_S_guiRect_instance,
		RC_S_guiRect_style,
	      };

  constexpr WITE::resourceConsumer RC_S_guiRect_target = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment>::value;

#include "guiRect.frag.spv.h"
#include "rectangle.vert.spv.h"

  constexpr WITE::shaderModule SM_L_guiRect[] = {
    { rectangle_vert, sizeof(rectangle_vert), vk::ShaderStageFlagBits::eVertex },
    { guiRect_frag, sizeof(guiRect_frag), vk::ShaderStageFlagBits::eFragment }
  };

  constexpr WITE::graphicsShaderRequirements S_guiRect {
    .id = FLID,
    .modules = SM_L_guiRect,
    .targetProvidedResources = RC_S_guiRect_target,
    .sourceProvidedResources = RC_S_guiRect_sourceAll,
    .cullMode = vk::CullModeFlagBits::eNone,
    .vertexCountOverride = 6
  };
  //!!append S_RP_gui S_guiRect

  constexpr WITE::resourceReference RR_L_guiRect[] = {
    { CP_data.src, RS_S_guiRectInstance.id },
    { CP_data.dst, RS_guiRectInstance.id },
    { RC_S_guiRect_instance.id, RS_guiRectInstance.id },
    { RC_S_guiRect_style.id, RS_guiRectStyle.id },
  };

  constexpr WITE::sourceLayout SL_guiRect = {
    .id = FLID,
    .objectLayoutId = OL_guiRect.id,
    .resources = RR_L_guiRect,
  };
  //!!append SL_all SL_guiRect

  constexpr WITE::resourceReference RR_L_primaryCamera_guiRect[] = {
    { RC_S_guiRect_target.id, RS_primaryCamera_cameraData.id },
  };
  //!!append RR_L_primaryCamera RR_L_primaryCamera_guiRect

}

#undef FILE_ID

