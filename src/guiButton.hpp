/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include "uiStyle.hpp"
#include "../generated/guiRect_stub.hpp"
#include "../generated/guiText_stub.hpp"

namespace doh {

  struct guiButton {
    typedefCB(clickAction, void, guiButton*);
    guiRect rect;
    guiRectInstance_t rectData;
    guiText label;
    guiTextInstance_t labelData;
    guiTextIndirectBuffer_t labelContent;
    buttonStyle_t& style;
    std::string labelStr;
    clickAction onClick;
    bool isPressed = false;
    guiButton() = delete;
    guiButton(const guiButton&);
    guiButton(buttonStyle_t& style, glm::vec2 location, std::string label, clickAction ca);
    ~guiButton();
    void update();
  };

}
