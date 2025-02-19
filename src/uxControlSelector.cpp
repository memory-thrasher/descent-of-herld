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

#include "uxControlSelector.hpp"

namespace doh {

  uxControlSelector::uxControlSelector(buttonStyle_t& style, controlActionMapping& cam) :
    style(style),
    cam(cam)
  {
    updateLabel();
    wantsVisibility = true;
  };

  uxControlSelector::~uxControlSelector() {
    destroy();
  };

  void uxControlSelector::updateLabel() {
    if(awaitingInput) [[unlikely]] {
      guiTextFormat(labelContent, "Press a key or button, or move an axis...");
    } else if(cam.controlId.axisId != control::nullAxis) [[likely]] {
      controlConfiguration*cc = getControl(cam.controlId);
      ASSERT_TRAP(cc, "action contained control id but no matching configuration was found");
      controller& c = getController(cam.controlId);
      guiTextFormat(labelContent, "%s, %s", c.label, cc->label[0] ? cc->label : getSysName(cam.controlId).c_str());
    } else {
      guiTextFormat(labelContent, "unassigned");
    }
  };

  void uxControlSelector::update() {
    if(!isVisible()) [[unlikely]] return;
    bool isHovered = this->isHovered();
    if(awaitingInput) [[unlikely]] {
      WITE::winput::inputPair latest;
      WITE::winput::getLatest(latest);
      uint8_t axis = 0;
      for(uint8_t i = 1;i < 3;i++)
	if(abs(latest.data.axes[i].current) > abs(latest.data.axes[axis].current)) [[unlikely]]
	  axis = i;
      if(latest.data.axes[axis].justChanged() && latest.data.axes[axis].current > 0.5f) [[unlikely]] {
	if(latest.id != WITE::winput::key<SDLK_ESCAPE>::v &&
	   latest.id != WITE::winput::lmb) [[likely]]
	  cam.controlId = { latest.id, axis };
	awaitingInput = false;
      }
    } else {
      WITE::winput::compositeInputData lmbCid;
      WITE::winput::getInput(WITE::winput::lmb, lmbCid);
      auto& lmb = lmbCid.axes[0];
      if(!isHovered) [[likely]]
	isPressed = false; //dragging off a button = abort
      else if(lmb.justPressed()) [[unlikely]]
	isPressed = true;
      else if(!isPressed && lmb.isPressed()) [[unlikely]]
	//dragging on does not proc indication of press sensitivity
	isHovered = false;
      else if(!lmb.isPressed()) {
	if(isPressed && lmb.justReleased()) [[unlikely]]
	  awaitingInput = true;
	isPressed = false;
      }
    }
    updateLabel();
    if(rect) [[likely]] {
      rect.setStyle(awaitingInput ? style.rectPressBuf : isHovered ? style.rectHovBuf : style.rectNormalBuf);
      rect.writeInstanceData(rectData);
    }
    if(label) [[likely]] {
      label.setStyle(isPressed ? style.textPressBuf : isHovered ? style.textHovBuf : style.textNormalBuf);
      label.writeIndirectBuffer(labelContent);
      label.writeInstanceData(labelData);
    }
  };

  void uxControlSelector::destroy() {
    rect.destroy();
    label.destroy();
  };

  void uxControlSelector::create() {
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

  const glm::vec4& uxControlSelector::getBounds() const {
    return rectData.extents;
  };

  void uxControlSelector::setBounds(const glm::vec4& bbox) {
    rectData.extents = labelData.bbox = bbox;
  };

  void uxControlSelector::updateVisible(bool parentVisible) {
    bool shouldBeVisible = parentVisible && wantsVisibility;
    if(shouldBeVisible == visible) [[likely]] return;
    if(shouldBeVisible)
      create();
    else
      destroy();
    visible = shouldBeVisible;
  };

}
