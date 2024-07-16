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

//!!include me
//!!onion all

#define FLID (FILE_ID + __LINE__)

//give each file a unique id, and the above macro will generate a unique id per-line (assuming the file ids are diverse enough)
#define FILE_ID 0

constexpr uint64_t gpuId = 0;

constexpr WITE::bufferRequirements BR_singleTransform = WITE::singleTransform<gpuId, FLID>::value,
	    BR_S_singleTransform = WITE::withId(WITE::stagingRequirementsFor(BR_singleTransform, 2), FLID);
//!!append BR_all BR_singleTransform
//!!append BR_all BR_S_singleTransform

struct cameraData_t {
  glm::vec4 geometry;//xy pixels
};

constexpr WITE::bufferRequirements BR_cameraData = WITE::simpleUB<gpuId, 3, sizeof(cameraData_t)>::value,
	    BR_S_cameraData = WITE::withId(WITE::stagingRequirementsFor(BR_cameraData, 2), FLID);
//!!append BR_all BR_cameraData
//!!append BR_all BR_S_cameraData

constexpr WITE::copyStep CP_transform = WITE::simpleCopy<FLID>::value,
	    CP_data = WITE::simpleCopy<FLID>::value;
//!!append CS_all CP_transform
//!!append CS_all CP_data

constexpr uint64_t RC_ID_RP_1_color = FLID,
	    RC_ID_RP_1_depth = FLID,
	    RC_ID_RP_2_color = FLID,
	    RC_ID_RP_2_depth = FLID,
	    RC_ID_RP_3_color = FLID,
	    RC_ID_RP_3_depth = FLID,
	    RC_ID_RP_4_color = FLID,
	    RC_ID_RP_4_depth = FLID,
	    RC_ID_RP_5_color = FLID,
	    RC_ID_RP_5_depth = FLID;

//this IR is the final target of reflection cube maps, getting resambled by future renders, and also the pre-bloom attachment for the screen target, which is resambled by the bloom shader.
constexpr WITE::imageRequirements IR_intermediateColor = WITE::intermediateColor<gpuId, FLID, WITE::Format::RGBA32float>::value;
//!!append IR_all IR_intermediateColor

constexpr WITE::imageRequirements IR_depth = WITE::simpleDepth<gpuId, FLID>::value;//depth are pretty much all the same
//!!append IR_all IR_depth

#undef FILE_ID
