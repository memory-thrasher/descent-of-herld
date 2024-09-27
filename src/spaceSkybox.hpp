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


#include <algorithm>

#include "gpuShared.hpp"
#include "targetPrimary.hpp"

#define FILE_ID 10002000

namespace doh {

  constexpr uint32_t pack(unsigned char x, unsigned char y, unsigned char z, unsigned char w) {
    return (x << 24) | (y << 16) | (z << 8) | w;
  };

  constexpr uint8_t unormToUint(float unorm) {
    return std::min<uint8_t>(static_cast<uint8_t>(unorm * 256), 255);
  };

  struct alignas(4) starType_t {//note: packed
    uint8_t r, g, b, p;//rgb = uint color; p = power (NYI)
    constexpr starType_t(const starType_t&) = default;
    constexpr starType_t(uint8_t r, uint8_t g, uint8_t b, uint8_t p) : r(r), g(g), b(b), p(p) {};
    constexpr starType_t(float r, float g, float b, uint8_t p) : r(unormToUint(r)), g(unormToUint(g)), b(unormToUint(b)), p(p) {};
  };

  constexpr starType_t starTypes[] = {
    { 0.6f, 0.69f, 1.0f, 10 * 2 },
    { 0.57f, 0.71f, 1.0f, 8 * 2 },
    { 0.667f, 0.749f, 1.0f, 6 * 2 },
    { 0.635f, 0.753f, 1.0f, 4 * 2 },
    { 0.792f, 0.843f, 1.0f, 3 * 2 },
    { 0.835f, 0.878f, 1.0f, 2 * 2 },
    { 0.973f, 0.969f, 1.0f, 2 * 2 },
    { 1.0f, 0.957f, 0.918f, 2 },
    { 1.0f, 0.929f, 0.89f, 2 },
    { 1.0f, 0.824f, 0.631f, 2 },
    { 1.0f, 0.855f, 0.71f, 2 },
    { 1.0f, 0.8f, 0.435f, 2 },
    { 1.0f, 0.71f, 0.424f, 2 },
    { 1.0f, 0.947f, 0.908f, 2 },
    { 1.0f, 0.859f, 0.76f, 2 },
    { 1.0f, 0.834f, 0.671f, 2 },
    { 1.0f, 0.825f, 0.661f, 2 },
    { 1.0f, 0.76f, 0.43f, 2 },
    { 1.0f, 0.61f, 0.344f, 2 },
    { 0.792f, 0.843f, 1.0f, 1 },
    { 0.835f, 0.878f, 1.0f, 1 },
    { 0.973f, 0.969f, 1.0f, 1 },
    { 1.0f, 0.957f, 0.918f, 1 },
    { 1.0f, 0.929f, 0.89f, 1 },
    { 1.0f, 0.824f, 0.631f, 1 },
    { 1.0f, 0.855f, 0.71f, 1 },
    { 1.0f, 0.8f, 0.435f, 1 },
    { 1.0f, 0.71f, 0.424f, 1 },
    { 1.0f, 0.947f, 0.908f, 1 },
    { 1.0f, 0.859f, 0.76f, 1 },
    { 1.0f, 0.834f, 0.671f, 1 },
  };

  static_assert(sizeof(starType_t) == 4);
  static_assert(sizeof(starTypes) == 4*4*5);//sync with spaceSkybox.partial.glsl

  constexpr uint32_t dot(glm::uvec3 a, glm::uvec3 b) {//TODO move to math file?
    return a.x*b.x + a.y*b.y + a.z*b.z;
  }

  constexpr starType_t* starTypeAt(glm::uvec3 worldPnt) {//sync with mesh shader
    constexpr int modulus = 0x1FFF;
    constexpr glm::uvec3 seedA(37, 139, 199),
		seedB(103, 53, 157);
    uint32_t idx = dot(worldPnt, worldPnt * seedA) + dot(worldPnt, seedB) & modulus;
    return idx < 20 ? &starTypes[idx] : NULL;
  };

  constexpr WITE::bufferRequirements BR_starTypes = WITE::simpleUB<gpuId, FLID, sizeof(starTypes)>::value;
  //!!append BR_all BR_starTypes

  WITE::buffer<BR_starTypes>& starTypesBuffer();

  constexpr WITE::objectLayout OL_spaceSkybox { FLID };
  //!!append OL_all OL_spaceSkybox
  //!!genObj OL_spaceSkybox

  constexpr WITE::resourceSlot RS_spaceSkybox_starTypes = {
    .id = FLID,
    .requirementId = BR_starTypes.id,
    .objectLayoutId = OL_spaceSkybox.id,
    .external = true,
  }, RS_L_spaceSkybox[] = {
    RS_spaceSkybox_starTypes,
  };
  //!!append RS_all RS_L_spaceSkybox
  //!!genObjSingleton RS_spaceSkybox_starTypes &starTypesBuffer()

  constexpr WITE::resourceConsumer RC_S_spaceSkybox_cameraTransform = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eMeshEXT | vk::ShaderStageFlagBits::eTaskEXT>::value,
	      RC_S_spaceSkybox_cameraData = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eMeshEXT | vk::ShaderStageFlagBits::eTaskEXT>::value,
	      RC_S_spaceSkybox_cameraAll[] = {
		RC_S_spaceSkybox_cameraTransform,
		RC_S_spaceSkybox_cameraData,
	      },
	      RC_S_spaceSkybox_starTypes = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eMeshEXT | vk::ShaderStageFlagBits::eFragment>::value,
	      RC_S_spaceSkybox_sourceAll[] = {
		RC_S_spaceSkybox_starTypes,
	      };

#include "spaceSkybox.task.spv.h"
#include "spaceSkybox.mesh.spv.h"
#include "spaceSkybox.frag.spv.h"

  constexpr WITE::shaderModule SM_L_spaceSkybox[] = {
    { spaceSkybox_task, sizeof(spaceSkybox_task), vk::ShaderStageFlagBits::eTaskEXT },
    { spaceSkybox_mesh, sizeof(spaceSkybox_mesh), vk::ShaderStageFlagBits::eMeshEXT },
    { spaceSkybox_frag, sizeof(spaceSkybox_frag), vk::ShaderStageFlagBits::eFragment },
  };

  constexpr WITE::graphicsShaderRequirements S_spaceSkybox {
    .id = FLID,
    .modules = SM_L_spaceSkybox,
    .targetProvidedResources = RC_S_spaceSkybox_cameraAll,
    .sourceProvidedResources = RC_S_spaceSkybox_sourceAll,
    .topology = vk::PrimitiveTopology::ePointList,
    .cullMode = vk::CullModeFlagBits::eNone,
    .meshGroupCountX = 700,
    .blend = additiveBlend,
    .depthWrite = false,//keep depth of skybox visible pixels to 1 for additive shaders
  };
  //!!append S_RP_skybox S_spaceSkybox

  constexpr WITE::resourceReference RR_L_spaceSkybox_source[] = {
    { RC_S_spaceSkybox_starTypes.id, RS_spaceSkybox_starTypes.id },
  }, RR_L_spaceSkybox_target[] = {
    { RC_S_spaceSkybox_cameraData.id, RS_primaryCamera_cameraData.id },
    { RC_S_spaceSkybox_cameraTransform.id, RS_primaryCamera_transform.id },
  };
  //!!append RR_L_primaryCamera RR_L_spaceSkybox_target

  constexpr WITE::sourceLayout SL_spaceSkybox = {
    .id = FLID,
    .objectLayoutId = OL_spaceSkybox.id,
    .resources = RR_L_spaceSkybox_source,
  };
  //!!append SL_all SL_spaceSkybox

}

#undef FILE_ID
