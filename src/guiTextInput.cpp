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
    bool updated = false;
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
      updated = true;
      isFocused = isHovered;
      if(isHovered) {
	insertPnt = WITE::min(static_cast<uint32_t>((mouseInSnorm.x - textData.bbox.x - style.textNormal.charMetric.z + style.textNormal.charMetric.x/2) / style.textNormal.charMetric.x), static_cast<uint32_t>(std::strlen(content)));
      }
    }
    if(isFocused) {
      bool shiftDown = WITE::winput::keyPressed<SDLK_LSHIFT>() || WITE::winput::keyPressed<SDLK_RSHIFT>();
      for(const uint32_t&key : WITE::winput::frameKeyboardBuffer) {
	if(key == 0) break;
	char insert = 0;
	switch(key) {
	case SDLK_RETURN:
	case SDLK_ESCAPE:
	case SDLK_TAB://TODO (shift) tab sends focus somewhere else
	  //loose focus
	  isFocused = false;
	  updated = true;
	  break;
	case ' ': insert = shiftDown ? ' ' : ' '; break;
	case '!': insert = shiftDown ? '!' : '!'; break;
	case '"': insert = shiftDown ? '"' : '"'; break;
	case '#': insert = shiftDown ? '#' : '#'; break;
	case '%': insert = shiftDown ? '%' : '%'; break;
	case '$': insert = shiftDown ? '$' : '$'; break;
	case '&': insert = shiftDown ? '&' : '&'; break;
	case '\'': insert = shiftDown ? '"' : '\''; break;
	case '(': insert = shiftDown ? '(' : '('; break;
	case ')': insert = shiftDown ? ')' : ')'; break;
	case '*': insert = shiftDown ? '*' : '*'; break;
	case '+': insert = shiftDown ? '+' : '+'; break;
	case ',': insert = shiftDown ? '<' : ','; break;
	case '-': insert = shiftDown ? '_' : '-'; break;
	case '.': insert = shiftDown ? '>' : '.'; break;
	case '/': insert = shiftDown ? '?' : '/'; break;
	case '0': insert = shiftDown ? ')' : '0'; break;
	case '1': insert = shiftDown ? '!' : '1'; break;
	case '2': insert = shiftDown ? '@' : '2'; break;
	case '3': insert = shiftDown ? '#' : '3'; break;
	case '4': insert = shiftDown ? '$' : '4'; break;
	case '5': insert = shiftDown ? '%' : '5'; break;
	case '6': insert = shiftDown ? '^' : '6'; break;
	case '7': insert = shiftDown ? '&' : '7'; break;
	case '8': insert = shiftDown ? '*' : '8'; break;
	case '9': insert = shiftDown ? '(' : '9'; break;
	case ':': insert = shiftDown ? ':' : ':'; break;
	case ';': insert = shiftDown ? ':' : ';'; break;
	case '<': insert = shiftDown ? '<' : '<'; break;
	case '=': insert = shiftDown ? '+' : '='; break;
	case '>': insert = shiftDown ? '>' : '>'; break;
	case '?': insert = shiftDown ? '?' : '?'; break;
	case '@': insert = shiftDown ? '@' : '@'; break;
	case '[': insert = shiftDown ? '{' : '['; break;
	case '\\': insert = shiftDown ? '|' : '\\'; break;
	case ']': insert = shiftDown ? '}' : ']'; break;
	case '^': insert = shiftDown ? '^' : '^'; break;
	case '_': insert = shiftDown ? '_' : '_'; break;
	case '`': insert = shiftDown ? '`' : '`'; break;
	case 'a': insert = shiftDown ? 'A' : 'a'; break;
	case 'b': insert = shiftDown ? 'B' : 'b'; break;
	case 'c': insert = shiftDown ? 'C' : 'c'; break;
	case 'd': insert = shiftDown ? 'D' : 'd'; break;
	case 'e': insert = shiftDown ? 'E' : 'e'; break;
	case 'f': insert = shiftDown ? 'F' : 'f'; break;
	case 'g': insert = shiftDown ? 'G' : 'g'; break;
	case 'h': insert = shiftDown ? 'H' : 'h'; break;
	case 'i': insert = shiftDown ? 'I' : 'i'; break;
	case 'j': insert = shiftDown ? 'J' : 'j'; break;
	case 'k': insert = shiftDown ? 'K' : 'k'; break;
	case 'l': insert = shiftDown ? 'L' : 'l'; break;
	case 'm': insert = shiftDown ? 'M' : 'm'; break;
	case 'n': insert = shiftDown ? 'N' : 'n'; break;
	case 'o': insert = shiftDown ? 'O' : 'o'; break;
	case 'p': insert = shiftDown ? 'P' : 'p'; break;
	case 'q': insert = shiftDown ? 'Q' : 'q'; break;
	case 'r': insert = shiftDown ? 'R' : 'r'; break;
	case 's': insert = shiftDown ? 'S' : 's'; break;
	case 't': insert = shiftDown ? 'T' : 't'; break;
	case 'u': insert = shiftDown ? 'U' : 'u'; break;
	case 'v': insert = shiftDown ? 'V' : 'v'; break;
	case 'w': insert = shiftDown ? 'W' : 'w'; break;
	case 'x': insert = shiftDown ? 'X' : 'x'; break;
	case 'y': insert = shiftDown ? 'Y' : 'y'; break;
	case 'z': insert = shiftDown ? 'Z' : 'z'; break;
	}
	if(insert) {
	  size_t newLen = WITE::min(std::strlen(content) + 1, sizeof(content) - 1, (textData.bbox.z - textData.bbox.x - style.textNormal.charMetric.z) / style.textNormal.charMetric.x - 1);
	  std::memmove(content + insertPnt + 1, content + insertPnt, newLen - insertPnt);
	  ASSERT_TRAP(insertPnt < sizeof(content), "overflow");
	  ASSERT_TRAP(newLen < sizeof(content), "overflow");
	  content[insertPnt] = insert;
	  content[newLen] = 0;
	  insertPnt = WITE::min(insertPnt + 1, newLen - 1);
	  updated = true;
	}
      }
    }
    if(updated) {
      guiTextFormat(textContent, "%s", content);
      if(isFocused) {
	caretData.bbox.x = textData.bbox.x + style.textNormal.charMetric.z + style.textNormal.charMetric.x * insertPnt - style.textNormal.charMetric.x/2;
	caretData.bbox.y = textData.bbox.y + style.textNormal.charMetric.w;
	caretData.bbox.z = caretData.bbox.x + style.textNormal.charMetric.x;
	caretData.bbox.w = caretData.bbox.y + style.textNormal.charMetric.y;
	if(!caret.onionObj) {
	  caret = guiInputCaret::create();
	}
      } else if(caret.onionObj) {
	caret.destroy();
      }
    }
    rect.setStyle(isFocused ? style.rectFocusedBuf : style.rectNormalBuf);
    text.setStyle(isFocused ? style.textFocusedBuf : style.textNormalBuf);
    text.writeIndirectBuffer(textContent);
    if(caret.onionObj)
      caret.writeInstanceData(caretData);
  };

}
