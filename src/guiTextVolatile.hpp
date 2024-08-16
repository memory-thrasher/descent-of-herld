/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include "guiText.hpp"

//!!include me
//!!onion all

#define FILE_ID 10003000

// This is for static text. There should eventually be a dynamic text renderable that has staging areas and a compute shader to render strings into direct buffers (like the WITE test).
namespace doh {

  constexpr WITE::objectLayout OL_guiTextVolatile = { .id = FLID };
  //!!append OL_all OL_guiTextVolatile
  //!!genObj OL_guiTextVolatile

  constexpr WITE::bufferRequirements BR_guiTextVolatileInstance = WITE::simpleUB<gpuId, FLID, sizeof(guiTextInstance_t)>::value,
	      BR_S_guiTextVolatileInstance = withId(WITE::stagingRequirementsFor(BR_guiTextVolatileInstance, 2), FLID),
	      BR_guiTextVolatileIndirect = {
		.deviceId = gpuId,
		.id = FLID,
		.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndirectBuffer,
		.size = 16 + guiText_maxCharsPerString * sizeof(vk::DrawIndirectCommand),
		.frameswapCount = 1,
		.hostVisible = false,
	      },
	      BR_S_guiTextVolatileIndirect = withId(WITE::stagingRequirementsFor(BR_guiTextVolatileIndirect, 2), FLID);
  static_assert(guiText_maxCharsPerString % 16 == 0);
  //!!append BR_all BR_guiTextVolatileInstance
  //!!append BR_all BR_guiTextVolatileIndirect
  //!!append BR_all BR_S_guiTextVolatileInstance
  //!!append BR_all BR_S_guiTextVolatileIndirect

  constexpr WITE::resourceSlot RS_guiTextVolatileInstance = {
    .id = FLID,
    .requirementId = BR_guiTextVolatileInstance.id,
    .objectLayoutId = OL_guiTextVolatile.id,
  }, RS_S_guiTextVolatileInstance = {
    .id = FLID,
    .requirementId = BR_S_guiTextVolatileInstance.id,
    .objectLayoutId = OL_guiTextVolatile.id,
  }, RS_guiTextVolatileStyle = {
    .id = FLID,
    .requirementId = BR_guiTextStyle.id,
    .objectLayoutId = OL_guiTextVolatile.id,
    .external = true,
  }, RS_guiTextVolatileIndirect = {
    .id = FLID,
    .requirementId = BR_guiTextVolatileIndirect.id,
    .objectLayoutId = OL_guiTextVolatile.id,
  }, RS_S_guiTextVolatileIndirect = {
    .id = FLID,
    .requirementId = BR_S_guiTextVolatileIndirect.id,
    .objectLayoutId = OL_guiTextVolatile.id,
  }, RS_guiTextVolatileMesh = {
    .id = FLID,
    .requirementId = fontMesh.bufferRequirements_v.id,
    .objectLayoutId = OL_guiTextVolatile.id,
    .external = true,
  }, RS_guiTextVolatile_all[] = {
    RS_guiTextVolatileInstance,
    RS_S_guiTextVolatileInstance,
    RS_guiTextVolatileStyle,
    RS_guiTextVolatileIndirect,
    RS_S_guiTextVolatileIndirect,
    RS_guiTextVolatileMesh,
  };
  //!!append RS_all RS_guiTextVolatile_all
  //!!genObjSet RS_guiTextVolatileStyle setStyle WITE::buffer<BR_guiTextStyle>
  //!!genObjSingleton RS_guiTextVolatileMesh &fontMeshBuffer()
  //!!genObjWrite RS_S_guiTextVolatileIndirect writeIndirectBuffer guiTextIndirectBuffer_t
  //!!genObjWrite RS_S_guiTextVolatileInstance writeInstanceData guiTextInstance_t

  constexpr WITE::resourceReference RR_L_guiTextVolatile[] = {
    { .resourceConsumerId = RC_S_guiText_indirectCount.id, .resourceSlotId = RS_guiTextVolatileIndirect.id, .subresource = {0, 16} },
    { .resourceConsumerId = RC_S_guiText_indirect.id, .resourceSlotId = RS_guiTextVolatileIndirect.id, .subresource = {16} },
    { RC_S_guiText_style.id, RS_guiTextVolatileStyle.id },
    { RC_S_guiText_instance.id, RS_guiTextVolatileInstance.id },
    { fontMesh.resourceConsumer_v.id, RS_guiTextVolatileMesh.id },
    { CP_transform.src, RS_S_guiTextVolatileInstance.id },
    { CP_transform.dst, RS_guiTextVolatileInstance.id },
    { CP_data.src, RS_S_guiTextVolatileIndirect.id },
    { CP_data.dst, RS_guiTextVolatileIndirect.id },
  };

  constexpr WITE::sourceLayout SL_guiTextVolatile = {
    .id = FLID,
    .objectLayoutId = OL_guiTextVolatile.id,
    .resources = RR_L_guiTextVolatile,
  };
  //!!append SL_all SL_guiTextVolatile

}

#undef FILE_ID

