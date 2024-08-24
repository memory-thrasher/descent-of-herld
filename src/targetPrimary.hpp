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

//!!include me
//!!onion all

#define FILE_ID 1000000

namespace doh {

  constexpr uint64_t RC_ID_primaryCamera_present = FLID;

  constexpr WITE::objectLayout OL_primaryCamera = {
    .id = FLID,
    .windowConsumerId = RC_ID_primaryCamera_present,//this implicitly creates a window that presents the image here by RR
  };
  //!!append OL_all OL_primaryCamera
  //!!genObj OL_primaryCamera
  //!!genObjGlobalCollection
  //!!genObjGetWindow

  constexpr WITE::imageRequirements IR_color = WITE::simpleColor<gpuId, FLID>::value;
  //!!append IR_all IR_color

  constexpr WITE::resourceSlot RS_primaryCamera_cameraData = {
    .id = FLID,
    .requirementId = BR_cameraData.id,
    .objectLayoutId = OL_primaryCamera.id,
  }, RS_S_primaryCamera_cameraData = {
    .id = FLID,
    .requirementId = BR_S_cameraData.id,
    .objectLayoutId = OL_primaryCamera.id,
  }, RS_primaryCamera_transform = {
    .id = FLID,
    .requirementId = BR_compoundTransform.id,
    .objectLayoutId = OL_primaryCamera.id,
  }, RS_S_primaryCamera_transform = {
    .id = FLID,
    .requirementId = BR_S_compoundTransform.id,
    .objectLayoutId = OL_primaryCamera.id,
  }, RS_primaryCamera_color = {
    .id = FLID,
    .requirementId = IR_color.id,
    .objectLayoutId = OL_primaryCamera.id,
    .resizeBehavior = WITE::resize_trackWindow_discard,
  }, RS_primaryCamera_depth = {
    .id = FLID,
    .requirementId = IR_depth.id,
    .objectLayoutId = OL_primaryCamera.id,
    .resizeBehavior = WITE::resize_trackWindow_discard,
  }, RS_primaryCamera_all[] = {
    RS_primaryCamera_cameraData,
    RS_S_primaryCamera_cameraData,
    RS_primaryCamera_transform,
    RS_S_primaryCamera_transform,
    RS_primaryCamera_color,
    RS_primaryCamera_depth,
  };
  //!!append RS_all RS_primaryCamera_all
  //!!genObjWrite RS_S_primaryCamera_cameraData writeCameraData cameraData_t
  //!!genObjWrite RS_S_primaryCamera_transform writeCameraTransform compoundTransform_packed_t

  constexpr WITE::resourceReference RR_L_primaryCamera_invariant[] = {
    { CP_warmup_transform.src, RS_S_primaryCamera_transform.id },
    { CP_warmup_transform.dst, RS_primaryCamera_transform.id },
    { CP_warmup_data.src, RS_S_primaryCamera_cameraData.id },
    { CP_warmup_data.dst, RS_primaryCamera_cameraData.id },
    { RC_ID_RP_gui_depth, RS_primaryCamera_depth.id },
    { RC_ID_RP_gui_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_prenear_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_prenear_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_near_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_near_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_premid_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_premid_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_mid_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_mid_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_prefar_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_prefar_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_far_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_far_color, RS_primaryCamera_color.id },
    { RC_ID_RP_preskybox_depth, RS_primaryCamera_depth.id },
    { RC_ID_RP_preskybox_depth_input, RS_primaryCamera_depth.id },
    { RC_ID_RP_preskybox_depth_input_DS, RS_primaryCamera_depth.id },
    { RC_ID_RP_preskybox_color, RS_primaryCamera_color.id },
    { RC_ID_RP_skybox_depth, RS_primaryCamera_depth.id },
    { RC_ID_RP_skybox_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_prepost_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_prepost_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_postH_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_postH_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_postV_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_postV_color, RS_primaryCamera_color.id },
    // { RC_ID_RP_postFinal_depth, RS_primaryCamera_depth.id },
    // { RC_ID_RP_postFinal_color, RS_primaryCamera_color.id },
    { RC_ID_primaryCamera_present, RS_primaryCamera_color.id },
  };
  //!!append RR_L_primaryCamera RR_L_primaryCamera_invariant

}

#undef FILE_ID

