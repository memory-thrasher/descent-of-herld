/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once
//!!include me
//!!onion all

#include "guiRect.hpp"

#define FILE_ID 10007000

namespace doh {

  constexpr WITE::objectLayout OL_guiRectVolatile = { .id = FLID };
  //!!append OL_all OL_guiRectVolatile
  //!!genObj OL_guiRectVolatile

  constexpr WITE::bufferRequirements BR_guiRectVolatileInstance = WITE::simpleUB<gpuId, FLID, sizeof(guiRectInstance_t)>::value;
  //!!append BR_all BR_guiRectVolatileInstance

  constexpr WITE::bufferRequirements BR_S_guiRectVolatileInstance = withId(WITE::stagingRequirementsFor(BR_guiRectVolatileInstance, 2), FLID);
  //!!append BR_all BR_S_guiRectVolatileInstance

  constexpr WITE::resourceSlot RS_guiRectVolatileInstance = {
    .id = FLID,
    .requirementId = BR_guiRectVolatileInstance.id,
    .objectLayoutId = OL_guiRectVolatile.id,
  }, RS_S_guiRectVolatileInstance = {
    .id = FLID,
    .requirementId = BR_S_guiRectVolatileInstance.id,
    .objectLayoutId = OL_guiRectVolatile.id,
  }, RS_guiRectVolatileStyle = {
    .id = FLID,
    .requirementId = BR_guiRectStyle.id,
    .objectLayoutId = OL_guiRectVolatile.id,
    .external = true,
  }, RS_guiRectVolatile_all[] = {
    RS_guiRectVolatileInstance,
    RS_S_guiRectVolatileInstance,
    RS_guiRectVolatileStyle,
  };
  //!!append RS_all RS_guiRectVolatile_all
  //!!genObjWrite RS_S_guiRectVolatileInstance writeInstanceData guiRectInstance_t
  //!!genObjSet RS_guiRectVolatileStyle setStyle WITE::buffer<BR_guiRectStyle>

  constexpr WITE::resourceReference RR_L_guiRectVolatile[] = {
    { RC_S_guiRect_instance.id, RS_guiRectVolatileInstance.id },
    { RC_S_guiRect_style.id, RS_guiRectVolatileStyle.id },
    { CP_warmup_data.src, RS_S_guiRectVolatileInstance.id },
    { CP_warmup_data.dst, RS_guiRectVolatileInstance.id },
  };

  constexpr WITE::sourceLayout SL_guiRectVolatile = {
    .id = FLID,
    .objectLayoutId = OL_guiRectVolatile.id,
    .resources = RR_L_guiRectVolatile,
  };
  //!!append SL_all SL_guiRectVolatile

}

#undef FILE_ID

