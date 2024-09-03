/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include <WITE/WITE.hpp>

#include "cameraStuff.hpp"

//!!include me
//!!onion all

#define FLID (FILE_ID + __LINE__)

  //give each file a unique id, and the above macro will generate a unique id per-line (assuming the file ids are diverse enough)
#define FILE_ID 0

namespace doh {

  constexpr uint64_t gpuId = 0;

  /*
    The playable area is larger than the representation of a float, yet doubles are slow, so the playable space is divded into grid cubes of integer coordinates, and floats represent any position within the cube. The orientation part of the matrix applies equally to all layers of the grid.

    The least significant component of each position identifier is a 3d vector of 32-bit floats. The unit of this component is one meter. The second-least significant component is a 3d vector of unsigned 32-bit integers. Each grid cube in this vector is called a chunk. A chunk measures 2^16 meters on each axis. A single body, including sizable asteroids and even the smallest of planets, may well span multiple chunks. meter=[0,0,0] is centered in the chunk, whereas the chunk origin in the sector and the sector origin in the universe are each an extreme corner of their respective spaces, being indexed by an unisgned int.

    A grid of (2^32)^3 chunks is called a sector. A sector is therefore a cube of size 2^48 meters or about 0.03ly or 1882au, about 5 times the heliopause diameter of Sol. If a sector contains one or more stars, they will be located near the center of the sector (for rendering convenience at long range).

    The universe is divded into (2^32)^3 sectors. This gives the universe a hard boundary of 2^80 meters, about 8.4*10^12 ly, about 1000 times larger than the real-life observable universe.

    Sectors are not arranged in galaxies. They have approximately even distribution throughout the universe. Most celestial objects are stationary, and will not collapse or orbit naturally, though they may spawn arranged in a way that implies they do orbit.
  */

  constexpr float chunkMeters = 1 << 16;
  constexpr int64_t chunksPerSector = std::numeric_limits<uint32_t>::max();

  struct compoundTransform_packed_t;

  struct compoundTransform_t {
    glm::mat3 orientation; //stable rotation-only transform, and therefore trivially reversible
    glm::vec3 meters;
    glm::uvec3 chunk, sector;
    compoundTransform_t();
    explicit compoundTransform_t(const glm::mat3& orientation);
    compoundTransform_t(const glm::vec3& meters, const glm::ivec3& chunk = {}, const glm::ivec3& sector = {});
    compoundTransform_t(const glm::mat3& orientation, const glm::vec3& meters, const glm::ivec3& chunk = {}, const glm::ivec3& sector = {});
    void pack(compoundTransform_packed_t* out);
    void stabilize();//ensure axes are perpendicular and vector columns and rows are normal
    void move(const glm::vec3& deltaMeters, const glm::ivec3& deltaChunk = {}, const glm::ivec3& deltaSector = {});
    void rotate(const glm::vec3& axis, float angle);//axis must be normal
    void rotateLocal(const glm::vec3& axis, float angle);//axis must be normal
    void moveSectors(const glm::ivec3& delta);
    void moveSectors(const glm::vec3& delta);
    void moveChunks(const glm::ivec3& delta);
    void moveChunks(const glm::vec3& delta);
    glm::vec3 approxSector();//note: for debugging only, will cause precision issues in most of the universe
  };

  //because std140 doesn't like non-256-bit-multiple objects.
  struct compoundTransform_packed_t {
    glm::vec4 transform[3];//orientation and meter-part of the location combined, row-major
    glm::uvec4 chunk, sector;//w is pad
    void unpack(compoundTransform_t* out);
  };

  constexpr WITE::bufferRequirements BR_compoundTransform = WITE::simpleUB<gpuId, FLID, sizeof(compoundTransform_packed_t)>::value,
	      BR_S_compoundTransform = WITE::withId(WITE::stagingRequirementsFor(BR_compoundTransform, 2), FLID);
  //!!append BR_all BR_compoundTransform
  //!!append BR_all BR_S_compoundTransform

  constexpr WITE::bufferRequirements BR_gvec4 = WITE::simpleUB<gpuId, FLID, sizeof(glm::vec4)>::value,
	      BR_S_gvec4 = WITE::withId(WITE::stagingRequirementsFor(BR_gvec4, 2), FLID);
  //!!append BR_all BR_gvec4
  //!!append BR_all BR_S_gvec4

  constexpr WITE::bufferRequirements BR_cameraData = WITE::simpleUB<gpuId, FLID, sizeof(cameraData_t)>::value,
	      BR_S_cameraData = WITE::withId(WITE::stagingRequirementsFor(BR_cameraData, 2), FLID);
  //!!append BR_all BR_cameraData
  //!!append BR_all BR_S_cameraData

  constexpr WITE::copyStep CP_warmup_transform = {
    .id = FLID,
    .src = FLID,
    .dst = FLID,
  }, CP_warmup_data {
    .id = FLID,
    .src = FLID,
    .dst = FLID,
  };
  //!!append CS_all CP_warmup_transform
  //!!append CS_all CP_warmup_data
  //!!append IDL_CP_L_warmup CP_warmup_transform.id
  //!!append IDL_CP_L_warmup CP_warmup_data.id

  constexpr uint64_t RC_ID_RP_gui_depth = FLID,
	      RC_ID_RP_gui_color = FLID,
	      RC_ID_RP_prenear_depth = FLID,
	      RC_ID_RP_prenear_depth_input_DS = FLID,
	      RC_ID_RP_prenear_depth_input = FLID,
	      RC_ID_RP_prenear_color = FLID,
	      RC_ID_RP_near_depth = FLID,
	      RC_ID_RP_near_color = FLID,
	      RC_ID_RP_premid_depth = FLID,
	      RC_ID_RP_premid_depth_input_DS = FLID,
	      RC_ID_RP_premid_depth_input = FLID,
	      RC_ID_RP_premid_color = FLID,
	      RC_ID_RP_mid_depth = FLID,
	      RC_ID_RP_mid_color = FLID,
	      RC_ID_RP_prefar_depth = FLID,
	      RC_ID_RP_prefar_depth_input_DS = FLID,
	      RC_ID_RP_prefar_depth_input = FLID,
	      RC_ID_RP_prefar_color = FLID,
	      RC_ID_RP_far_depth = FLID,
	      RC_ID_RP_far_color = FLID,
	      RC_ID_RP_preskybox_depth = FLID,
	      RC_ID_RP_preskybox_depth_input_DS = FLID,
	      RC_ID_RP_preskybox_depth_input = FLID,
	      RC_ID_RP_preskybox_color = FLID,
	      RC_ID_RP_skybox_depth = FLID,
	      RC_ID_RP_skybox_color = FLID,
	      RC_ID_RP_prepost_depth = FLID,
	      RC_ID_RP_prepost_color = FLID,
	      RC_ID_RP_postH_depth = FLID,
	      RC_ID_RP_postH_color = FLID,
	      RC_ID_RP_postV_depth = FLID,
	      RC_ID_RP_postV_color = FLID,
	      RC_ID_RP_postFinal_depth = FLID,
	      RC_ID_RP_postFinal_color = FLID;

  //this IR is the final target of reflection cube maps, getting resambled by future renders, and also the pre-bloom attachment for the screen target, which is resambled by the bloom shader.
  constexpr WITE::imageRequirements IR_intermediateColor = WITE::intermediateColor<gpuId, FLID, WITE::Format::RGBA32float>::value;
  //!!append IR_all IR_intermediateColor

  constexpr WITE::imageRequirements IR_depth = {
    .deviceId = gpuId,
    .id = FLID,
    .format = WITE::Format::D16unorm,
    .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eInputAttachment,
  };
  //!!append IR_all IR_depth

  constexpr vk::PipelineColorBlendAttachmentState additiveBlend = { true, vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

#undef FILE_ID

}

//std::ostream overloads belong in the global namespace
std::ostream& operator<<(std::ostream&, const doh::compoundTransform_t& ct);

glm::uvec3 operator/(const glm::uvec3& l, uint32_t r);
