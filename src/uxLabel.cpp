/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <set>
#include <iostream>

#include "uxLabel.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "math.hpp"
#include "uiStyle.hpp"

namespace doh {

  uxLabel::uxLabel(textOnlyStyle_t& style, std::string labelStr) : style(style), labelStr(labelStr) {
    guiTextFormat(labelContent, "%s", labelStr.c_str());
    setVisible(true);
  };

  uxLabel::~uxLabel() {
    destroy();
  };

  void uxLabel::destroy() {
    rect.destroy();
    label.destroy();
  };

  void uxLabel::create() {
    if(!rect) [[likely]] {
      rect = guiRect::create();
      rect.writeInstanceData(rectData);
      rect.setStyle(style.rectBuf);
    }
    if(!label) [[likely]] {
      label = guiText::create();
      label.writeIndirectBuffer(labelContent);
      label.writeInstanceData(labelData);
      label.setStyle(style.textBuf);
    }
  };

  void uxLabel::setLabel(std::string str) {
    labelStr = str;
    guiTextFormat(labelContent, "%s", labelStr.c_str());
    if(isVisible()) [[likely]] {
      destroy();
      create();
    }
  };

  const glm::vec4& uxLabel::getBounds() const {
    return rectData.extents;
  };

  void uxLabel::setBounds(const glm::vec4& bbox) {
    rectData.extents = labelData.bbox = bbox;
    if(isVisible()) [[likely]] {
      destroy();
      create();
    }
  };

  void uxLabel::updateVisible(bool parentVisible) {
    bool shouldBeVisible = parentVisible && wantsVisibility;
    if(shouldBeVisible == visible) [[likely]] return;
    if(shouldBeVisible)
      create();
    else
      destroy();
    visible = shouldBeVisible;
  };

}
