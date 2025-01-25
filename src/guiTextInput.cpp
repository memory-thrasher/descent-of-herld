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

#include "guiTextInput.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "math.hpp"

namespace doh {

  guiTextInput::guiTextInput(textInputStyle_t& style, glm::vec4 bbox, const char* initialContent) :
    rect(guiRect::create()),
    rectData({ bbox }),
    text(guiText::create()),
    textData({ bbox }),
    style(style)
  {
    WITE::strcpy(content, initialContent, guiText_maxCharsPerString-1);
    content[guiText_maxCharsPerString-1] = 0;
    guiTextFormat(textContent, "%s", content);
    rect.writeInstanceData(rectData);
    text.writeIndirectBuffer(textContent);
    text.writeInstanceData(textData);
    caretData.color = style.caretColor;
    update();
  };

  guiTextInput::~guiTextInput() {
    rect.destroy();
    text.destroy();
    caret.destroy();//destroy has embedded existance check
  };

  void guiTextInput::update() {
    WITE::winput::compositeInputData mouseLocation, lmbCid;
    bool isHovered = false;
    glm::vec2 mouseInSnorm;
    WITE::winput::getInput(WITE::winput::mouse, mouseLocation);
    {
      WITE::scopeLock lock(&targetPrimary::allInstances_mutex);
      for(const targetPrimary& camera : targetPrimary::allInstances) {
	const auto size = camera.getWindow().getVecSize();
	mouseInSnorm = { mouseLocation.axes[0].current / size.x * 2 - 1,
			 mouseLocation.axes[1].current / size.y * 2 - 1 };
	if(rectContainsPoint(rectData.extents, mouseInSnorm)) {
	  isHovered = true;
	  break;
	}
      }
    }
    WITE::winput::getInput(WITE::winput::lmb, lmbCid);
    auto& lmb = lmbCid.axes[0];
    if(lmb.isPressed()) [[unlikely]] {
      isFocused = isHovered;
      if(isFocused) {
	insertPnt = uint32_t((mouseInSnorm.x - textData.bbox.x - style.textNormal.charMetric.z - style.textNormal.charMetric.x/2) / style.textNormal.charMetric.x);
	caretData.bbox = { textData.bbox.x + style.textNormal.charMetric.z + style.textNormal.charMetric.x * insertPnt - style.textNormal.charMetric.x/2, textData.bbox.y + style.textNormal.charMetric.w, style.textNormal.charMetric.x, style.textNormal.charMetric.y };
	if(!caret.onionObj)
	  caret = guiInputCaret::create();
      } else {
	caret.destroy();
      }
    }
    rect.setStyle(isFocused ? style.rectFocusedBuf : style.rectNormalBuf);
    text.setStyle(isFocused ? style.textFocusedBuf : style.textNormalBuf);
    if(caret.onionObj)
      caret.writeInstanceData(caretData);
  };

}
