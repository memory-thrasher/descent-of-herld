/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "gpuShared.hpp"

namespace doh {

  void compoundTransform_t::pack(compoundTransform_packed_t* out) {
    out->transform = glm::mat4x3(glm::column(orientation, 0),
				 glm::column(orientation, 1),
				 glm::column(orientation, 2),
				 meters);
    out->chunk = glm::uvec4(chunk, 0);
    out->sector = glm::uvec4(sector, 0);
  };

  void compoundTransform_packed_t::unpack(compoundTransform_t* out) {
    out->orientation = glm::mat3(glm::column(transform, 0),
				 glm::column(transform, 1),
				 glm::column(transform, 2));
    out->meters = glm::column(transform, 3);
    out->chunk = glm::uvec3(chunk);
    out->sector = glm::uvec3(sector);
  };

  void compoundTransform_t::stabilize() {
    glm::vec3 forward = glm::normalize(glm::column(orientation, 0));
    glm::vec3 right = glm::normalize(glm::cross(glm::column(orientation, 1), forward));
    ASSERT_TRAP(glm::dot(right, glm::column(orientation, 2)) > 0.5f, "hand-reversal detected");
    orientation = glm::mat3(forward, glm::normalize(glm::cross(forward, right)), right);
  };

  void compoundTransform_t::move(const glm::vec3& deltaMeters, const glm::ivec3& deltaChunk, const glm::ivec3& deltaSector) {
    for(int i = 0;i < 3;i++) {
      int chunkCarry = glm::abs(meters[i] + deltaMeters[i]) <= chunkMeters ? 0 : meters[i] > 0 ? 1 : -1;
      meters[i] += deltaMeters[i] - chunkMeters * chunkCarry;
      int64_t newChunk = static_cast<int64_t>(chunk[i]) + deltaChunk[i] + chunkCarry;
      chunk[i] = (chunksPerSector + newChunk) % chunksPerSector;
      sector[i] += newChunk / chunksPerSector + deltaSector[i];
    }
  };

}
