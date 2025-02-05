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
#include "uxBase.hpp"
#include "../generated/guiRect_stub.hpp"
#include "../generated/guiText_stub.hpp"

namespace doh {

  struct uxLabel : public uxBase {
    guiRect rect;
    guiRectInstance_t rectData;
    guiText label;
    guiTextInstance_t labelData;
    guiTextIndirectBuffer_t labelContent;
    textOnlyStyle_t& style;
    std::string labelStr;
    uxLabel() = delete;
    uxLabel(const uxLabel&) = delete;
    uxLabel(textOnlyStyle_t& style, std::string label);
    virtual ~uxLabel();
    void destroy();
    void create();
    void setLabel(std::string);
    virtual const glm::vec4& getBounds() const override;
    virtual void setBounds(const glm::vec4&) override;
    virtual void updateVisible(bool parentVisible) override;
  };

}
