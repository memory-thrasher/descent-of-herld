/*
  Copyright 2024-2025 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include "guiWireframeHelper.hpp"

//!!include me
//!!onion all

#define FILE_ID 10006000

namespace doh {

  constexpr guiWireframeVert_t inputCaret_lines[] { {0, 0}, {255, 0}, {128, 0}, {128, 255}, {0, 255}, {255, 255} };
  defineGuiWireframe(inputCaret, gpuId, inputCaret_lines, FLID);
  decltype(mesh_inputCaret)::buffer_t& inputCaret_meshBuffer();
  //!!append SL_all SL_guiWireframe_inputCaret
  //!!append S_RP_gui S_guiWireframe_inputCaret
  //!!append RS_all RS_guiWireframe_inputCaret_all
  //!!append BR_all mesh_inputCaret.bufferRequirements_v
  //!!append OL_all OL_inputCaret
  //!!genObj OL_inputCaret
  //!!genObjWrite RS_S_guiWireframeInstance_inputCaret writeInstanceData guiWireframeInstance_t
  //!!genObjSingleton RS_guiWireframeMesh_inputCaret &inputCaret_meshBuffer()

}

#undef FILE_ID
