/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <set>
#include <iostream>

#include "guiButton.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "math.hpp"
#include "uiStyle.hpp"

namespace doh {

  guiButton::guiButton(buttonStyle_t& style, glm::vec2 location, std::string labelStr, clickAction ca) :
    rect(guiRect::create()),
    rectData({ location, location.x + style.width, location.y + style.height }),
    label(guiText::create()),
    labelData({ location, location.x + style.width, location.y + style.height }),
    style(style),
    labelStr(labelStr),
    onClick(ca)
  {
    rect.setStyle(style.rectNormalBuf);
    rect.writeInstanceData(rectData);
    label.setTextMesh(fontMeshBuffer());
    label.setStyle(style.textNormalBuf);
    guiTextFormat(labelContent, "%s", labelStr.c_str());
    label.writeIndirectBuffer(labelContent);
    label.writeInstanceData(labelData);
  };

  guiButton::~guiButton() {
    rect.destroy();
    label.destroy();
  };

  void guiButton::update() {
    WITE::winput::compositeInputData cid;
    bool isHovered = false;
    WITE::winput::getInput(WITE::winput::mouse, cid);
    {
      WITE::scopeLock lock(&targetPrimary::allInstances_mutex);
      for(const targetPrimary& camera : targetPrimary::allInstances) {
	const auto size = camera.getWindow().getVecSize();
	glm::vec2 mouseInSnorm(cid.axes[0].current / size.x * 2 - 1,
			       cid.axes[1].current / size.y * 2 - 1);
	if(rectContainsPoint(rectData.extents, mouseInSnorm)) {
	  isHovered = true;
	  break;
	}
      }
    }
    if(!isHovered) [[likely]]
      isPressed = false; //dragging off a button = abort
    else if(WITE::winput::getButtonDown(WITE::winput::lmb)) [[unlikely]]
      isPressed = true;
    else if(!isPressed && WITE::winput::getButton(WITE::winput::lmb)) [[unlikely]]
      //dragging on does not proc indication of press sensitivity
      isHovered = false;
    else if(!WITE::winput::getButton(WITE::winput::lmb)) {
      if(isPressed && WITE::winput::getButtonUp(WITE::winput::lmb)) [[unlikely]]
	onClick(this);
      isPressed = false;
    }
    rect.setStyle(isPressed ? style.rectPressBuf : isHovered ? style.rectHovBuf : style.rectNormalBuf);
    label.setStyle(isPressed ? style.textPressBuf : isHovered ? style.textHovBuf : style.textNormalBuf);
  };

  void guiButton::resize(glm::vec4) {
    //
  };

  void guiButton::setLabel(std::string) {
    //
  };


}
