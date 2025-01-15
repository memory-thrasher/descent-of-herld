/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "gpuShared.hpp"

//ostream overloads belong in global namespace
std::ostream& operator<<(std::ostream& s, const doh::compoundTransform_t& ct) {
  return s << "{ [" << ct.orientation << "] (" << ct.meters << ") (" << ct.chunk << ") (" << ct.sector << ") }";
};

glm::uvec3 operator/(const glm::uvec3& l, uint32_t r) {
  return { l.x / r, l.y / r, l.z / r };
};

namespace doh {

  compoundTransform_t::compoundTransform_t() : orientation(1), meters(), chunk(), sector() {};

  compoundTransform_t::compoundTransform_t(const glm::mat3& orientation) :
    orientation(orientation), meters(), chunk(), sector() {};

  compoundTransform_t::compoundTransform_t(const glm::vec3& meters, const glm::ivec3& chunk, const glm::ivec3& sector) :
    orientation(1), meters(meters), chunk(chunk), sector(sector) {};

  compoundTransform_t::compoundTransform_t(const glm::mat3& orientation, const glm::vec3& meters, const glm::ivec3& chunk, const glm::ivec3& sector) : orientation(orientation), meters(meters), chunk(chunk), sector(sector) {};

  void compoundTransform_t::moveSectors(const glm::ivec3& delta) {
    move({}, {}, delta);
  };

  void compoundTransform_t::moveSectors(const glm::vec3& delta) {
    glm::vec3 whole,
      fract = glm::modf(delta, whole);
    for(int i = 0;i < 3;i++)
      if(glm::abs(fract[i]) >= 0.5f) {
	float sign = glm::sign(fract[i]);
	fract[i] -= sign;
	whole[i] += sign;
      }
    move({}, glm::ivec3(fract * 65536.0f) * 65536, glm::ivec3(whole));
  };

  glm::vec3 compoundTransform_t::approxSector() {//note: for debugging only, will cause precision issues in most of the universe
    return glm::vec3(sector) + glm::vec3(chunk / 65536) / 65536.0f;
  };

  void compoundTransform_t::moveChunks(const glm::ivec3& delta) {
    move({}, delta);
  };

  void compoundTransform_t::moveChunks(const glm::vec3& delta) {
    glm::vec3 whole,
      fract = glm::modf(delta, whole);
    for(int i = 0;i < 3;i++)
      if(glm::abs(fract[i]) >= 0.5f) {
	float sign = glm::sign(fract[i]);
	fract[i] -= sign;
	whole[i] += sign;
      }
    move(fract * chunkMeters, glm::ivec3(whole + glm::vec3(0.5f, 0.5f, 0.5f)));
  };

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
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::column(orientation, 1)));
    orientation = glm::mat3(forward, glm::normalize(glm::cross(right, forward)), right);
  };

  void compoundTransform_t::move(const glm::vec3& deltaMeters, const glm::ivec3& deltaChunk, const glm::ivec3& deltaSector) {
    for(int i = 0;i < 3;i++) {
      int chunkCarry = glm::abs(meters[i] + deltaMeters[i]) <= chunkMeters/2 ? 0 : meters[i] + deltaMeters[i] > 0 ? 1 : -1;
      meters[i] += deltaMeters[i] - chunkMeters * chunkCarry;
      int64_t newChunk = static_cast<int64_t>(chunk[i]) + deltaChunk[i] + chunkCarry;
      chunk[i] = static_cast<uint32_t>((static_cast<int64_t>(chunksPerSector*10) + newChunk) % chunksPerSector);
      sector[i] += newChunk / static_cast<int64_t>(chunksPerSector) + (newChunk < 0 ? -1 : 0) + deltaSector[i];
    }
  };

  void compoundTransform_t::rotate(const glm::vec3& axis, float angle) {
    rotateLocal(axis * orientation, angle);
  };

  void compoundTransform_t::rotateLocal(const glm::vec3& axis, float angle) {
    float const cosA = glm::cos(angle);
    float const sinA = glm::sin(angle);
    glm::vec cosAxis((1.0f - cosA) * axis);
    orientation *= glm::mat3(cosA + cosAxis[0] * axis[0],
			     cosAxis[0] * axis[1] + sinA * axis[2],
			     cosAxis[0] * axis[2] - sinA * axis[1],
			     cosAxis[1] * axis[0] - sinA * axis[2],
			     cosA + cosAxis[1] * axis[1],
			     cosAxis[1] * axis[2] + sinA * axis[0],
			     cosAxis[2] * axis[0] + sinA * axis[1],
			     cosAxis[2] * axis[1] - sinA * axis[0],
			     cosA + cosAxis[2] * axis[2]);
  };

}
