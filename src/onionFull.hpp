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

  expand_IDL_CP_L_warmup(uint64_t);
  constexpr WITE::layerRequirements L_warmup = {
    .copies = IDL_CP_L_warmup,
  };
  //!!append LR_all L_warmup

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

  constexpr WITE::layerRequirements L_gui = {
    .renders = RP_gui.id,
  };
  //!!append LR_all L_gui

  expand_S_RP_prenear(WITE::graphicsShaderRequirements);
  constexpr WITE::renderPassRequirements RP_prenear = {
    .id = FLID,
    .depth = RC_ID_RP_prenear_depth,
    .input = RC_ID_RP_prenear_depth_input,
    .shaders = S_RP_prenear,
  };
  //!!append RPR_all RP_prenear

  constexpr WITE::layerRequirements L_prenear = {
    .renders = RP_prenear.id,
  };
  //!!append LR_all L_prenear

  // expand_S_RP_near(WITE::graphicsShaderRequirements);
  constexpr WITE::renderPassRequirements RP_near = {
    .id = FLID,
    .depth = RC_ID_RP_near_depth,
    .color = RC_ID_RP_near_color,
    // .shaders = S_RP_near,
  };
  //!!append RPR_all RP_near

  constexpr WITE::layerRequirements L_near = {
    .renders = RP_near.id,
  };
  //!!append LR_all L_near

  expand_S_RP_premid(WITE::graphicsShaderRequirements);
  constexpr WITE::renderPassRequirements RP_premid = {
    .id = FLID,
    .depth = RC_ID_RP_premid_depth,
    .input = RC_ID_RP_premid_depth_input,
    .shaders = S_RP_premid,
  };
  //!!append RPR_all RP_premid

  constexpr WITE::layerRequirements L_premid = {
    .renders = RP_premid.id,
  };
  //!!append LR_all L_premid

  // expand_S_RP_mid(WITE::graphicsShaderRequirements);
  constexpr WITE::renderPassRequirements RP_mid = {
    .id = FLID,
    .depth = RC_ID_RP_mid_depth,
    .color = RC_ID_RP_mid_color,
    // .shaders = S_RP_mid,
  };
  //!!append RPR_all RP_mid

  constexpr WITE::layerRequirements L_mid = {
    .renders = RP_mid.id,
  };
  //!!append LR_all L_mid

  expand_S_RP_prefar(WITE::graphicsShaderRequirements);
  constexpr WITE::renderPassRequirements RP_prefar = {
    .id = FLID,
    .depth = RC_ID_RP_prefar_depth,
    .input = RC_ID_RP_prefar_depth_input,
    .shaders = S_RP_prefar,
  };
  //!!append RPR_all RP_prefar

  constexpr WITE::layerRequirements L_prefar = {
    .renders = RP_prefar.id,
  };
  //!!append LR_all L_prefar

  // expand_S_RP_far(WITE::graphicsShaderRequirements);
  constexpr WITE::renderPassRequirements RP_far = {
    .id = FLID,
    .depth = RC_ID_RP_far_depth,
    .color = RC_ID_RP_far_color,
    // .shaders = S_RP_far,
  };
  //!!append RPR_all RP_far

  constexpr WITE::layerRequirements L_far = {
    .renders = RP_far.id,
  };
  //!!append LR_all L_far

  expand_S_RP_preskybox(WITE::graphicsShaderRequirements);
  constexpr WITE::renderPassRequirements RP_preskybox = {
    .id = FLID,
    .depth = RC_ID_RP_preskybox_depth,
    .input = RC_ID_RP_preskybox_depth_input,
    .shaders = S_RP_preskybox,
  };
  //!!append RPR_all RP_preskybox

  constexpr WITE::layerRequirements L_preskybox = {
    .renders = RP_preskybox.id,
  };
  //!!append LR_all L_preskybox

  expand_S_RP_skybox(WITE::graphicsShaderRequirements);
  constexpr WITE::renderPassRequirements RP_skybox = {
    .id = FLID,
    .depth = RC_ID_RP_skybox_depth,
    .color = RC_ID_RP_skybox_color,
    .shaders = S_RP_skybox,
  };
  //!!append RPR_all RP_skybox

  constexpr WITE::layerRequirements L_skybox = {
    .renders = RP_skybox.id,
  };
  //!!append LR_all L_skybox

  // expand_S_RP_prepost(WITE::graphicsShaderRequirements);
  // constexpr WITE::renderPassRequirements RP_prepost = {
  //   .id = FLID,
  //   .depth = RC_ID_RP_prepost_depth,
  //   .color = RC_ID_RP_prepost_color,
  //   .shaders = S_RP_prepost,
  // };
  // //! !append RPR_all RP_prepost

  // constexpr WITE::layerRequirements L_prepost = {
  //   .renders = RP_prepost.id,
  // };
  // //! !append LR_all L_prepost

  // expand_S_RP_postH(WITE::graphicsShaderRequirements);
  // constexpr WITE::renderPassRequirements RP_postH = {
  //   .id = FLID,
  //   .depth = RC_ID_RP_postH_depth,
  //   .color = RC_ID_RP_postH_color,
  //   .shaders = S_RP_postH,
  // };
  // //! !append RPR_all RP_postH

  // constexpr WITE::layerRequirements L_postH = {
  //   .renders = RP_postH.id,
  // };
  // //! !append LR_all L_postH

  // expand_S_RP_postV(WITE::graphicsShaderRequirements);
  // constexpr WITE::renderPassRequirements RP_postV = {
  //   .id = FLID,
  //   .depth = RC_ID_RP_postV_depth,
  //   .color = RC_ID_RP_postV_color,
  //   .shaders = S_RP_postV,
  // };
  // //! !append RPR_all RP_postV

  // constexpr WITE::layerRequirements L_postV = {
  //   .renders = RP_postV.id,
  // };
  // //! !append LR_all L_postV

  // expand_S_RP_postFinal(WITE::graphicsShaderRequirements);
  // constexpr WITE::renderPassRequirements RP_postFinal = {
  //   .id = FLID,
  //   .depth = RC_ID_RP_postFinal_depth,
  //   .color = RC_ID_RP_postFinal_color,
  //   .shaders = S_RP_postFinal,
  // };
  // //! !append RPR_all RP_postFinal

  // constexpr WITE::layerRequirements L_postFinal = {
  //   .renders = RP_postFinal.id,
  // };
  // //! !append LR_all L_postFinal

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
