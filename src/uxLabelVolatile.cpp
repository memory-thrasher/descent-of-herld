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

#include "uxLabelVolatile.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "math.hpp"
#include "uiStyle.hpp"

namespace doh {

  uxLabelVolatile::uxLabelVolatile(textOnlyStyle_t& style, std::string labelStr) : style(style), labelStr(labelStr) {
    guiTextFormat(labelContent, "%s", labelStr.c_str());
    setVisible(true);
  };

  uxLabelVolatile::~uxLabelVolatile() {
    destroy();
  };

  void uxLabelVolatile::update() {
    if(rect) [[likely]] {
      rect.writeInstanceData(rectData);
      rect.setStyle(style.rectBuf);
    }
    if(label) [[likely]] {
      guiTextFormat(labelContent, "%s", labelStr.c_str());
      label.writeIndirectBuffer(labelContent);
      label.writeInstanceData(labelData);
      label.setStyle(style.textBuf);
    }
  };

  void uxLabelVolatile::destroy() {
    rect.destroy();
    label.destroy();
  };

  void uxLabelVolatile::create() {
    if(!rect) [[likely]] {
      rect = guiRectVolatile::create();
      rect.writeInstanceData(rectData);
      rect.setStyle(style.rectBuf);
    }
    if(!label) [[likely]] {
      label = guiTextVolatile::create();
      label.writeIndirectBuffer(labelContent);
      label.writeInstanceData(labelData);
      label.setStyle(style.textBuf);
    }
  };

  void uxLabelVolatile::setLabel(std::string str) {
    labelStr = str;
  };

  const glm::vec4& uxLabelVolatile::getBounds() const {
    return rectData.extents;
  };

  void uxLabelVolatile::setBounds(const glm::vec4& bbox) {
    rectData.extents = labelData.bbox = bbox;
  };

  void uxLabelVolatile::updateVisible(bool parentVisible) {
    bool shouldBeVisible = parentVisible && wantsVisibility;
    if(shouldBeVisible == visible) [[likely]] return;
    if(shouldBeVisible)
      create();
    else
      destroy();
    visible = shouldBeVisible;
  };

}
