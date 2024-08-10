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
    for(int i = 0;i < 3;i++)
      out->transform[i] = glm::vec4(glm::row(orientation, i), meters[i]);
    out->chunk = glm::uvec4(chunk, 0);
    out->sector = glm::uvec4(sector, 0);
  };

  void compoundTransform_packed_t::unpack(compoundTransform_t* out) {
    for(int i = 0;i < 3;i++) {
      out->orientation = glm::row(out->orientation, i, glm::vec3(transform[i]));
      out->meters[i] = transform[i].w;
    }
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

  void compoundTransform_t::rotate(const glm::vec3& axis, float angle) {
    float const cosA = glm::cos(angle);
    float const sinA = glm::sin(angle);
    glm::vec norm(normalize(axis));
    glm::vec cosAxis((1.0f - cosA) * norm);
    orientation *= glm::mat3(cosA + cosAxis[0] * norm[0],
			     cosAxis[0] * norm[1] + sinA * norm[2],
			     cosAxis[0] * norm[2] - sinA * norm[1],
			     cosAxis[1] * norm[0] - sinA * norm[2],
			     cosA + cosAxis[1] * norm[1],
			     cosAxis[1] * norm[2] + sinA * norm[0],
			     cosAxis[2] * norm[0] + sinA * norm[1],
			     cosAxis[2] * norm[1] - sinA * norm[0],
			     cosA + cosAxis[2] * norm[2]);
  };

}
