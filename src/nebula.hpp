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
#include "targetPrimary.hpp"

//!!include me
//!!onion all

#define FILE_ID 10005000

namespace doh {

  //max nebula size = 128 sectors (half max render distance)
  //max image size then is: 128^3 = 2mb per byte-channel (3d support >= 256 required)
  //note: mipmap required support is bound to the VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT flag
  //mipmap required support levels: 16 (2^4) so smallest mip is 8^3

  constexpr size_t nebulaSize = 128,//sync with nebula.vert.glsl
    nebulaVolume = nebulaSize*nebulaSize*nebulaSize;

  /* nebula/cloud data:
     cpu data:
     db of elements, each element has:
       diffusive color (mat3)
       refractive scatter rate
     per sector of cloud
       encoded composition (list of compounds? Per sector? db model needed)
         for now, make this just a vec3 for conentration of H O and S
       calculated: (w = ir)
         light color transparency (vec4)
	 light color diffusion (mat4)
	 light color exposure, propogated (vec4)
     gpu data: (nebulaMap_t)
     per sector:
       apparent light emission (f16vec4 (w unused))
   */

  //note: total mass of reference nebula = 4.8M, with a volume of 700 cu ly so avg density = 0.006889275M/culy
  struct nebula_sectordata_t {//cpu only, generated
    compound_p composition;
    glm::vec4 transparency, exposure;
    glm::mat4 diffusion;
  };

  struct nebula_data_t {
    nebula_sectordata_t sectors[nebulaVolume];
  };

  typedef std::array<uint64_t, nebulaVolume> nebulaMap_t;

  void updateNebulaExposure(nebula_data_t& nebulaData);
  void updateNebula(const nebula_data_t& nebulaData, nebulaMap_t& out);
  void generateNebula(const glm::uvec3& location, nebula_data_t& out);

  constexpr WITE::objectLayout OL_nebula = { .id = FLID };
  //!!append OL_all OL_nebula
  //!!genObj OL_nebula

  constexpr WITE::imageRequirements IR_nebula_map = {
    .deviceId = gpuId,
    .id = FLID,
    .format = vk::Format::eR16Sfloat,
    .usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
    .dimensions = 3,
    .frameswapCount = 1,
    .mipLevels = 4,
    .defaultW = nebulaSize,
    .defaultH = nebulaSize,
    .defaultD = nebulaSize,
  };
  //!!append IR_all IR_nebula_map

  constexpr WITE::bufferRequirements BR_S_nebula_map = WITE::withId(WITE::stagingRequirementsFor<IR_nebula_map>(1), FLID);
  //!!append BR_all BR_S_nebula_map

  constexpr WITE::resourceSlot RS_nebula_instance = {//just the sector for now, w = unused
    .id = FLID,
    .requirementId = BR_gvec4.id,
    .objectLayoutId = OL_nebula.id,
  }, RS_nebula_map = {
    .id = FLID,
    .requirementId = IR_nebula_map.id,
    .objectLayoutId = OL_nebula.id,
  }, RS_S_nebula_map = {
    .id = FLID,
    .requirementId = BR_S_nebula_map.id,
    .objectLayoutId = OL_nebula.id,
  }, RS_nebula_all[] = {
    RS_nebula_instance,
    RS_nebula_map,
    RS_S_nebula_map,
  };
  //!!append RS_all RS_nebula_all
  //!!genObjSlowWrite RS_nebula_instance writeInstanceData glm::uvec4
  //!!genObjWrite RS_S_nebula_map writeMap nebulaMap_t
  //!!genObjStepControl CP_warmup_data.id mapCopySetEnabled

  constexpr WITE::resourceConsumer RC_S_nebula_instance = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eVertex>::value,
	      RC_S_nebula_map = {
		.id = FLID,
		.stages = vk::ShaderStageFlagBits::eFragment,
		.access = vk::AccessFlagBits2::eShaderSampledRead,
		.usage = { vk::DescriptorType::eCombinedImageSampler, { {}, vk::Filter::eLinear, vk::Filter::eNearest, vk::SamplerMipmapMode::eNearest } }
	      }, RC_S_nebula_sourceAll[] = {
    RC_S_nebula_instance,
    RC_S_nebula_map,
  };

  constexpr WITE::resourceConsumer RC_S_nebula_targetTransform = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex>::value,
	      RC_S_nebula_targetData = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex>::value,
	      RC_S_nebula_targetAll[] = {
		RC_S_nebula_targetTransform,
		RC_S_nebula_targetData,
	      };

#include "nebula.vert.spv.h"
#include "nebula.frag.spv.h"

  constexpr WITE::shaderModule SM_L_nebula[] = {
    { nebula_vert, sizeof(nebula_vert), vk::ShaderStageFlagBits::eVertex },
    { nebula_frag, sizeof(nebula_frag), vk::ShaderStageFlagBits::eFragment }
  };

  constexpr WITE::graphicsShaderRequirements S_nebula {
    .id = FLID,
    .modules = SM_L_nebula,
    .targetProvidedResources = RC_S_nebula_targetAll,
    .sourceProvidedResources = RC_S_nebula_sourceAll,
    .cullMode = vk::CullModeFlagBits::eNone,
    .vertexCountOverride = nebulaSize * 3 * 6,//one square per layer per axis
    .blend = additiveBlend,
    .depthWrite = false,//keep depth of skybox visible pixels to 1 for additive shaders
  };
  //!!append S_RP_skybox S_nebula

  constexpr WITE::resourceReference RR_L_nebula[] = {
    { CP_warmup_data.src, RS_S_nebula_map.id },
    { CP_warmup_data.dst, RS_nebula_map.id },
    { RC_S_nebula_instance.id, RS_nebula_instance.id },
    {
      .resourceConsumerId = RC_S_nebula_map.id,
      .resourceSlotId = RS_nebula_map.id,
      .subresource = { WITE::getAllInclusiveSubresource(IR_nebula_map), vk::ImageViewType::e3D },
    },
  };

  constexpr WITE::sourceLayout SL_nebula = {
    .id = FLID,
    .objectLayoutId = OL_nebula.id,
    .resources = RR_L_nebula,
  };
  //!!append SL_all SL_nebula

  constexpr WITE::resourceReference RR_L_primaryCamera_nebula[] = {
    { RC_S_nebula_targetData.id, RS_primaryCamera_cameraData.id },
    { RC_S_nebula_targetTransform.id, RS_primaryCamera_transform.id },
  };
  //!!append RR_L_primaryCamera RR_L_primaryCamera_nebula

}

#undef FILE_ID
