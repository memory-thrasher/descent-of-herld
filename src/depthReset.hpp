/*
  Copyright 2024-2025 Wafflecat Games, LLC

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

#define FILE_ID 10004000

/*
  There are five rendering stages that correlate with five rendering distances (including gui). These can correlate to LoD, as many objects use very different paradigms at different distances. Many objects can only exist or are only visible at close range. In each case, the entire bit range of the depth map is used. Shaders that are visible at close range are rendered first. Between stages, the below shader resets the depth map so that only unoccupied (depth 1) pixels will be rendered in future stages (still at depth 1) by setting the depth value of all other pixels to 0.

  Each rendering stage gets its own depth map. Between stages, a whole-screen triangle is drawn, and the prior stage's depth map is an input attachment, feeding a simple ternary fragment shader that populates the initial state of the new stage's depth map.
 */

namespace doh {

#include "wholeViewport.vert.spv.h"
#include "depthReset.frag.spv.h"

  constexpr WITE::shaderModule SM_L_depthReset[] = {
    { wholeViewport_vert, sizeof(wholeViewport_vert), vk::ShaderStageFlagBits::eVertex },
    { depthReset_frag, sizeof(depthReset_frag), vk::ShaderStageFlagBits::eFragment },
  };

  constexpr auto RC_RP_prenear_depth_input = WITE::consumerForInputAttachment(RC_ID_RP_prenear_depth_input_DS),
	      RC_RP_premid_depth_input = WITE::consumerForInputAttachment(RC_ID_RP_premid_depth_input_DS),
	      RC_RP_prefar_depth_input = WITE::consumerForInputAttachment(RC_ID_RP_prefar_depth_input_DS),
	      RC_RP_preskybox_depth_input = WITE::consumerForInputAttachment(RC_ID_RP_preskybox_depth_input_DS);


  constexpr WITE::graphicsShaderRequirements S_depthReset_prenear {
    .id = FLID,
    .modules = SM_L_depthReset,
    .targetProvidedResources = RC_RP_prenear_depth_input,
    .cullMode = vk::CullModeFlagBits::eNone,
    .vertexCountOverride = 3,
    .depthCompareMode = vk::CompareOp::eAlways,
  };
  //!!append S_RP_prenear S_depthReset_prenear


  constexpr WITE::graphicsShaderRequirements S_depthReset_premid {
    .id = FLID,
    .modules = SM_L_depthReset,
    .targetProvidedResources = RC_RP_premid_depth_input,
    .cullMode = vk::CullModeFlagBits::eNone,
    .vertexCountOverride = 3,
    .depthCompareMode = vk::CompareOp::eAlways,
  };
  //!!append S_RP_premid S_depthReset_premid


  constexpr WITE::graphicsShaderRequirements S_depthReset_prefar {
    .id = FLID,
    .modules = SM_L_depthReset,
    .targetProvidedResources = RC_RP_prefar_depth_input,
    .cullMode = vk::CullModeFlagBits::eNone,
    .vertexCountOverride = 3,
    .depthCompareMode = vk::CompareOp::eAlways,
  };
  //!!append S_RP_prefar S_depthReset_prefar

  constexpr WITE::graphicsShaderRequirements S_depthReset_preskybox {
    .id = FLID,
    .modules = SM_L_depthReset,
    .targetProvidedResources = RC_RP_preskybox_depth_input,
    .cullMode = vk::CullModeFlagBits::eNone,
    .vertexCountOverride = 3,
    .depthCompareMode = vk::CompareOp::eAlways,
  };
  //!!append S_RP_preskybox S_depthReset_preskybox

}

#undef FILE_ID
