/*
  Copyright 2024-2025 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "guiText.hpp"

namespace doh {

  int guiTextFormat(guiTextIndirectBuffer_t& out, const char* format, ...) {//move to cpp file?
    char buffer[guiText_maxCharsPerString+1];//null terminated
    std::va_list args;
    va_start(args, format);
    int ret = std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    for(out.size.x = 0;out.size.x < guiText_maxCharsPerString && buffer[out.size.x];++out.size.x) {
      unsigned ch = (unsigned)buffer[out.size.x];
      const auto& fce = font_character_extents[ch <= 127 ? ch : 0].data;//offset-length pair
      out.drawCommands[out.size.x] = vk::DrawIndirectCommand { fce[1], 1, fce[0], out.size.x };
    }
    return ret;
  };

  decltype(fontMesh)::buffer_t& fontMeshBuffer() {
    static decltype(fontMesh)::buffer_t ret;
    static bool loaded = false;
    if(!loaded) {
      loaded = true;
      fontMesh.load(&ret);
    }
    return ret;
  };

  float guiTextStyle_t::widthToFitChars(uint32_t chars) {
    return charMetric.x * (chars + 0.01f) + charMetric.z * 2;
  };

}

