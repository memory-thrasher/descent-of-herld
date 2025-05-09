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

#include "uxButtonVolatile.hpp"
#include "math.hpp"
#include "uiStyle.hpp"

namespace doh {

  uxButtonVolatile::uxButtonVolatile(buttonStyle_t& style, std::string labelStr, clickAction ca) :
    style(style),
    labelStr(labelStr),
    onClick(ca)
  {
    guiTextFormat(labelContent, "%s", labelStr.c_str());
    wantsVisibility = true;
  };

  uxButtonVolatile::~uxButtonVolatile() {
    rect.destroy();
    label.destroy();
  };

  void uxButtonVolatile::update() {
    if(!isVisible()) [[unlikely]] return;
    manageFocus();
    WITE::winput::compositeInputData lmbCid;
    bool isFocused = this->isFocused(), isClicked = false;
    WITE::winput::getInput(WITE::winput::lmb, lmbCid);
    auto& lmb = lmbCid.axes[0];
    if(!isFocused) [[likely]]
      isPressed = false; //dragging off a button = abort
    else if(lmb.justPressed()) [[unlikely]]
      isPressed = true;
    else if(!isPressed && lmb.isPressed()) [[unlikely]]
      //dragging on does not proc indication of press sensitivity
      isFocused = false;
    else if(!lmb.isPressed()) {
      if(isPressed && lmb.justReleased()) [[unlikely]]
	isClicked = true;
      isPressed = false;
    }
    guiTextFormat(labelContent, "%s", labelStr.c_str());
    if(rect) [[likely]] {
      rect.setStyle(isPressed ? style.rectPressBuf : isFocused ? style.rectHovBuf : style.rectNormalBuf);
      rect.writeInstanceData(rectData);
    }
    if(label) [[likely]] {
      label.setStyle(isPressed ? style.textPressBuf : isFocused ? style.textHovBuf : style.textNormalBuf);
      label.writeIndirectBuffer(labelContent);
      label.writeInstanceData(labelData);
    }
    if(isClicked) [[unlikely]] onClick(this);
  };

  void uxButtonVolatile::onActivate() {
    onClick(this);
  };

  void uxButtonVolatile::destroy() {
    rect.destroy();
    label.destroy();
  };

  void uxButtonVolatile::create() {
    if(!rect) [[likely]] {
      rect = guiRectVolatile::create();
      rect.writeInstanceData(rectData);
      rect.setStyle(style.rectNormalBuf);
    }
    if(!label) [[likely]] {
      label = guiTextVolatile::create();
      label.writeIndirectBuffer(labelContent);
      label.writeInstanceData(labelData);
      label.setStyle(style.textNormalBuf);
    }
  };

  const glm::vec4& uxButtonVolatile::getBounds() const {
    return rectData.extents;
  };

  void uxButtonVolatile::setBounds(const glm::vec4& bbox) {
    rectData.extents = labelData.bbox = bbox;
  };

  void uxButtonVolatile::updateVisible(bool parentVisible) {
    bool shouldBeVisible = parentVisible && wantsVisibility;
    if(shouldBeVisible == visible) [[likely]] return;
    if(shouldBeVisible)
      create();
    else
      destroy();
    visible = shouldBeVisible;
  };

}
