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

//NOTE: cotangent of one pixel's FOV is ~688 for 45 degree total fov on a 1080p screen
//intensity saturation distance = 9k sectors (360ly) beyond which stars are unlikely to shade a pixel at all
//sector saturation distance = 687 sectors beyond which a sector might fall entirely between pixels
//size saturation distance (1080p) = 4 sectors (7000 au) where a star found will only be one pixel or smaller
//a renderer (type TBD) will render the star in the camera's current sector
//a mesh or geometry renderer that outputs points will shader stars in other sectors within 700 sectors
//a whole-screen analytical fragment shader will query stars behind each pixel beyond 700 sectors away

/* near-ish stars: geometry shader
   each input plane is a vertex (data in vertex buffer), override the instance count to max plane count (render depth)
   vertex shader projects the plane into camera space and passes along as much as we can pre-compute
   geometry shader, one invocation per plane, "points" in, actual points out.
 */

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

  //global constants because we might need them in other shaders
  constexpr uint32_t starGridCount = 1;//for testing, later will have many more
  //note: due to max output vertex limitations and to take advantage of parallelism, favor more planes of lower density

  constexpr ::WITE::meshWrapper<gpuId, WITE::UDM::RG32uint, starGridCount, FLID> starGridMesh = {
    { pack(19, 11, 29, 1), pack(12, 42, 18, 19) },
    //y.xy*y.w and (y.y, y.-x)*y.w define the axes of a grid along the z=0 plane where ray origins are found. Each ray spawns a star every x.xyz*x.w sectors.
  };
  //!!append BR_all starGridMesh.bufferRequirements_v

  constexpr ::WITE::udmObject<WITE::UDM::RGBA8uint> starTypes[] = {
    { unormToUint(0.6f), unormToUint(0.69f), 255, 10 * 2 },
    { unormToUint(0.57f), unormToUint(0.71f), 255, 8 * 2 },
    { unormToUint(0.667f), unormToUint(0.749f), 255, 6 * 2 },
    { unormToUint(0.635f), unormToUint(0.753f), 255, 4 * 2 },
    { unormToUint(0.792f), unormToUint(0.843f), 255, 3 * 2 },
    { unormToUint(0.835f), unormToUint(0.878f), 255, 2 * 2 },
    { unormToUint(0.973f), unormToUint(0.969f), 255, 2 * 2 },
    { 255, unormToUint(0.957f), unormToUint(0.918f), 2 },
    { 255, unormToUint(0.929f), unormToUint(0.89f), 2 },
    { 255, unormToUint(0.824f), unormToUint(0.631f), 2 },
    { 255, unormToUint(0.855f), unormToUint(0.71f), 2 },
    { 255, unormToUint(0.8f), unormToUint(0.435f), 2 },
    { 255, unormToUint(0.71f), unormToUint(0.424f), 2 },
    { 255, unormToUint(0.947f), unormToUint(0.908f), 2 },
    { 255, unormToUint(0.859f), unormToUint(0.76f), 2 },
    { 255, unormToUint(0.834f), unormToUint(0.671f), 2 },
    { 255, unormToUint(0.825f), unormToUint(0.661f), 2 },
    { 255, unormToUint(0.76f), unormToUint(0.43f), 2 },
    { 255, unormToUint(0.61f), unormToUint(0.344f), 2 },
    { unormToUint(0.792f), unormToUint(0.843f), 255, 1 },
    { unormToUint(0.835f), unormToUint(0.878f), 255, 1 },
    { unormToUint(0.973f), unormToUint(0.969f), 255, 1 },
    { 255, unormToUint(0.957f), unormToUint(0.918f), 1 },
    { 255, unormToUint(0.929f), unormToUint(0.89f), 1 },
    { 255, unormToUint(0.824f), unormToUint(0.631f), 1 },
    { 255, unormToUint(0.855f), unormToUint(0.71f), 1 },
    { 255, unormToUint(0.8f), unormToUint(0.435f), 1 },
    { 255, unormToUint(0.71f), unormToUint(0.424f), 1 },
    { 255, unormToUint(0.947f), unormToUint(0.908f), 1 },
    { 255, unormToUint(0.859f), unormToUint(0.76f), 1 },
    { 255, unormToUint(0.834f), unormToUint(0.671f), 1 },
  };

  constexpr WITE::bufferRequirements BR_starTypes = WITE::simpleUB<gpuId, FLID, sizeof(starTypes)>::value;
  //!!append BR_all BR_starTypes

  decltype(starGridMesh)::buffer_t& starGridMeshBuffer();
  WITE::buffer<BR_starTypes>& starTypesBuffer();

  constexpr uint32_t starGridInstancesPerGrid = 512;//sync with shaders (instances in spaceSkybox.geom.glsl)

  constexpr WITE::objectLayout OL_spaceSkybox { FLID };
  //!!append OL_all OL_spaceSkybox
  //!!genObj OL_spaceSkybox

  constexpr WITE::resourceSlot RS_spaceSkybox_starGridMesh = {
    .id = FLID,
    .requirementId = starGridMesh.bufferRequirements_v.id,
    .objectLayoutId = OL_spaceSkybox.id,
    .external = true,
  }, RS_spaceSkybox_starTypes = {
    .id = FLID,
    .requirementId = BR_starTypes.id,
    .objectLayoutId = OL_spaceSkybox.id,
    .external = true,
  }, RS_L_spaceSkybox[] = {
    RS_spaceSkybox_starTypes,
    RS_spaceSkybox_starGridMesh,
  };
  //!!append RS_all RS_L_spaceSkybox
  //!!genObjSet RS_spaceSkybox_starGridMesh setStarGridMesh WITE::buffer<starGridMesh.bufferRequirements_v>
  //!!genObjSet RS_spaceSkybox_starTypes setStarTypes WITE::buffer<BR_starTypes>

  constexpr WITE::resourceConsumer RC_S_spaceSkybox_cameraTransform = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eGeometry>::value,
	      RC_S_spaceSkybox_cameraData = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eGeometry>::value,
	      RC_S_spaceSkybox_cameraAll[] = {
		RC_S_spaceSkybox_cameraTransform,
		RC_S_spaceSkybox_cameraData,
	      },
	      RC_S_spaceSkybox_starTypes = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eFragment>::value,
	      RC_S_spaceSkybox_sourceAll[] = {
		starGridMesh.resourceConsumer_v,
		RC_S_spaceSkybox_starTypes,
	      };

#include "spaceSkybox.vert.spv.h"
#include "spaceSkybox.geom.spv.h"
#include "spaceSkybox.frag.spv.h"

  constexpr WITE::shaderModule SM_L_spaceSkybox[] {
    { spaceSkybox_vert, sizeof(spaceSkybox_vert), vk::ShaderStageFlagBits::eVertex },
    { spaceSkybox_geom, sizeof(spaceSkybox_geom), vk::ShaderStageFlagBits::eGeometry },
    { spaceSkybox_frag, sizeof(spaceSkybox_frag), vk::ShaderStageFlagBits::eFragment },
  };

  constexpr WITE::graphicsShaderRequirements S_spaceSkybox {
    .id = FLID,
    .modules = SM_L_spaceSkybox,
    .targetProvidedResources = RC_S_spaceSkybox_cameraAll,
    .sourceProvidedResources = RC_S_spaceSkybox_sourceAll,
    .topology = vk::PrimitiveTopology::ePointList,
    .cullMode = vk::CullModeFlagBits::eNone,
    .instanceCountOverride = starGridInstancesPerGrid,
  };
  //!!append S_RP_gui S_spaceSkybox

  constexpr WITE::resourceReference RR_L_spaceSkybox_source[] = {
    { starGridMesh.resourceConsumer_v.id, RS_spaceSkybox_starGridMesh.id },
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
