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

#define FILE_ID 10005000

namespace doh {

  //max nebula size = 128 sectors (half max render distance)
  //max image size then is: 128^3 = 2mb per byte-channel (3d support >= 256 required)
  //note: mipmap required support is bound to the VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT flag
  //VK_FORMAT_B10G11R11_UFLOAT_PACK32 supports sampling but must be host-generated (no attach or storage)
  //VK_FORMAT_R16G16B16A16_SFLOAT is twice as big but supports more ops and higher precision
  //mipmap required support levels: 16 (2^4) so smallest mip is 8^3
  //support of mip for 3D is not obviously confirmed or denied anywhere, needs an experiment

  constexpr size_t nebulaSize = 128,
    nebulaVolume = nebulaSize*nebulaSize*nebulaSize;

  typedef WITE::copyableArray<uint32_t, nebulaVolume> nebulaMap_t;

  void generateNebula(const glm::uvec3& location, nebulaMap_t& out);

}

#undef FILE_ID
