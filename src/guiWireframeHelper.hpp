/*
  Copyright 2024-2025 Wafflecat Games, LLC

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

  struct guiWireframeInstance_t {
    glm::vec4 color;
    glm::vec4 bbox;
  };

  typedef WITE::udmObject<WITE::UDM::RG8unorm> guiWireframeVert_t;

  constexpr WITE::bufferRequirements BR_guiWireframeInstance = WITE::simpleUB<gpuId, FLID, sizeof(guiWireframeInstance_t)>::value,
	      BR_S_guiWireframeInstance = withId(WITE::stagingRequirementsFor(BR_guiWireframeInstance, 2), FLID);
  //!!append BR_all BR_guiWireframeInstance
  //!!append BR_all BR_S_guiWireframeInstance

#include "guiWireframe.vert.spv.h"
#include "solidColor.frag.spv.h"

  constexpr WITE::shaderModule SM_L_guiWireframe[] {
    { guiWireframe_vert, sizeof(guiWireframe_vert), vk::ShaderStageFlagBits::eVertex },
    { solidColor_frag, sizeof(solidColor_frag), vk::ShaderStageFlagBits::eFragment },
  };

  constexpr WITE::resourceConsumer RC_S_guiWireframeInstance = WITE::simpleUBConsumer<FLID, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment>::value;

  //consumes IDs [ID, ID + 15]
#define defineGuiWireframe(nom, G, mesh, ID)				\
  constexpr WITE::meshWrapper<G, WITE::UDM::RG8unorm, sizeof(mesh)/sizeof(mesh[0]), ID> mesh_ ##nom = mesh; \
  constexpr WITE::objectLayout OL_ ##nom = { .id = ID + 3 };		\
  constexpr WITE::resourceSlot RS_guiWireframeInstance_ ##nom = {	\
    .id = ID + 4,							\
    .requirementId = BR_guiWireframeInstance.id,			\
    .objectLayoutId = OL_ ##nom .id,					\
  }, RS_S_guiWireframeInstance_ ##nom = {				\
    .id = ID + 5,							\
    .requirementId = BR_S_guiWireframeInstance.id,			\
    .objectLayoutId = OL_ ##nom .id,					\
  }, RS_guiWireframeMesh_ ##nom = {					\
    .id = ID + 6,							\
    .requirementId = mesh_ ##nom .bufferRequirements_v.id,		\
    .objectLayoutId = OL_ ##nom .id,					\
    .external = true,							\
  }, RS_guiWireframe_ ##nom## _all[] = {				\
    RS_guiWireframeInstance_ ##nom,					\
    RS_S_guiWireframeInstance_ ##nom,					\
    RS_guiWireframeMesh_ ##nom,						\
  };									\
  constexpr WITE::resourceConsumer RC_S_guiWireframe_sourceAll_ ##nom [] = { \
    RC_S_guiWireframeInstance,						\
    mesh_ ##nom .resourceConsumer_v					\
  };									\
  constexpr WITE::graphicsShaderRequirements S_guiWireframe_ ##nom {	\
    .id = ID + 7,							\
    .modules = SM_L_guiWireframe,					\
    .sourceProvidedResources = RC_S_guiWireframe_sourceAll_ ##nom,	\
    .topology = vk::PrimitiveTopology::eLineList,			\
    .cullMode = vk::CullModeFlagBits::eNone,				\
  };									\
  constexpr WITE::resourceReference RR_L_guiWireframe_ ##nom [] = {	\
    { RC_S_guiWireframeInstance.id, RS_guiWireframeInstance_ ##nom .id }, \
    { mesh_ ##nom .resourceConsumer_v.id, RS_guiWireframeMesh_ ##nom .id }, \
    { CP_warmup_transform.src, RS_S_guiWireframeInstance_ ##nom .id },	\
    { CP_warmup_transform.dst, RS_guiWireframeInstance_ ##nom .id },	\
  };									\
  constexpr WITE::sourceLayout SL_guiWireframe_ ##nom = {		\
    .id = ID + 8,							\
    .objectLayoutId = OL_ ##nom .id,					\
    .resources = RR_L_guiWireframe_ ##nom ,				\
  }

  //template helper: (replace @@ with two !, #nom with nom passed to above define)
  //decltype(mesh_#nom)::buffer_t& #nom_meshBuffer(); //define this and make it load the data, see fontMesh
  //@@append SL_all SL_guiWireframe_#nom
  //@@append S_RP_gui S_guiWireframe_#nom
  //@@append RS_all RS_guiWireframe_#nom_all
  //@@append BR_all mesh_#nom.bufferRequirements_v
  //@@append OL_all OL_#nom
  //@@genObj OL_#nom
  //@@genObjWrite RS_S_guiWireframeInstance_#nom writeInstanceData guiWireframeInstance_t
  //@@genObjSingleton RS_guiWireframeMesh_#nom #nom_meshBuffer()

}

#undef FILE_ID

