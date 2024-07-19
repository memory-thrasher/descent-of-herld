/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "gpuShared.hpp"
#include "../generated/onionHelper.hpp"

#define FILE_ID 1000000000

namespace doh {

  expand_RR_L_primaryCamera(WITE::resourceReference);

  constexpr WITE::targetLayout TL_primaryCamera = {
    .id = FLID,
    .objectLayoutId = OL_primaryCamera.id,
    .resources = RR_L_primaryCamera,
  };
  //!!append TL_all TL_primaryCamera

  expand_S_RP_gui(WITE::graphicsShaderRequirements);

  constexpr WITE::renderPassRequirements RP_gui = {
    .id = FLID,
    .depth = RC_ID_RP_gui_depth,
    .color = RC_ID_RP_gui_color,
    .clearDepth = true,
    .clearColor = true,
    .shaders = S_RP_gui,
  };
  //!!append RPR_all RP_gui

  expand_IDL_CP_L_gui(uint64_t);

  constexpr WITE::layerRequirements L_gui = {
    .copies = IDL_CP_L_gui,
    .renders = RP_gui.id,
  };
  //!!append LR_all L_gui

  expand_IR_all(WITE::imageRequirements);
  expand_BR_all(WITE::bufferRequirements);
  expand_RS_all(WITE::resourceSlot);
  //expand_CSR_all(WITE::computeShaderRequirements);
  expand_RPR_all(WITE::renderPassRequirements);
  //expand_CL_all(WITE::clearStep);
  expand_CS_all(WITE::copyStep);
  expand_LR_all(WITE::layerRequirements);
  expand_OL_all(WITE::objectLayout);
  expand_TL_all(WITE::targetLayout);
  expand_SL_all(WITE::sourceLayout);

  constexpr WITE::onionDescriptor onionFull_data = {
    .IRS = IR_all,
    .BRS = BR_all,
    .RSS = RS_all,
    // .CSRS = CSR_all,
    .RPRS = RPR_all,
    // .CLS = CL_all,
    .CSS = CS_all,
    .LRS = LR_all,
    .OLS = OL_all,
    .TLS = TL_all,
    .SLS = SL_all,
    .GPUID = gpuId,
  };

  typedef WITE::onion<onionFull_data> onionFull_t;

  onionFull_t* getOnionFull();

}

#undef FILE_ID
