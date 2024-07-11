/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <WITE/WITE.hpp>

//!!include me
//!!onion all

constexpr uint64_t gpuId = 0;

constexpr bufferRequirements BR_singleTransform = WITE::singleTransform<gpuId, 1>::value,
	    BR_S_singleTransform = WITE::withId(WITE::stagingRequirementsFor(BR_singleTransform, 2), 2);
//!!register BR BR_singleTransform
//!!register BR BR_S_singleTransform

struct cameraData_t {
  glm::vec4 geometry;//xy pixels
};

constexpr bufferRequirements BR_cameraData = WITE::simpleUB<gpuId, 3, sizeof(cameraData_t)>::value,
	    BR_S_cameraData = WITE::withId(WITE::stagingRequirementsFor(BR_cameraData, 4), 2);
//!!register BR BR_cameraData
//!!register BR BR_S_cameraData

constexpr copyStep CP_transform = WITE::simpleCopy<100>::value;
//!!register CP CP_transform

constexpr uint64_t RC_ID_RP_1_color = 200,
	    RC_ID_RP_2_color = 201,
	    RC_ID_RP_3_color = 202,
	    RC_ID_RP_4_color = 203,
	    RC_ID_RP_5_color = 204,
	    RC_ID_reusable_depth = 205;



