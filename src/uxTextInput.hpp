/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include "uxBase.hpp"
#include "../generated/guiRectVolatile_stub.hpp"
#include "../generated/guiTextVolatile_stub.hpp"
#include "../generated/guiInputCaret_stub.hpp"
#include "uiStyle.hpp"

namespace doh {

  struct uxTextInput : public uxBase {
    guiRectVolatile rect;
    guiRectInstance_t rectData;
    guiTextVolatile text;
    guiTextInstance_t textData;
    guiTextIndirectBuffer_t textContent;
    guiInputCaret caret;
    guiWireframeInstance_t caretData;
    textInputStyle_t& style;
    char content[guiText_maxCharsPerString];
    size_t maxLen = guiText_maxCharsPerString - 1;
    uint32_t insertPnt = 0;
    bool isFocused = false;
    uxTextInput() = delete;
    uxTextInput(const uxTextInput&) = delete;
    uxTextInput(textInputStyle_t& style, const char* initialContent);
    ~uxTextInput();
    void destroy();
    void create();
    void updateCaretData();
    virtual void update() override;
    virtual const glm::vec4& getBounds() const override;
    virtual void setBounds(const glm::vec4&) override;
    virtual void updateVisible(bool parentVisible) override;
  };

}
