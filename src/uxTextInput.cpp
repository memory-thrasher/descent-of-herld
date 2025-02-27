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

#include "uxTextInput.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "math.hpp"

namespace doh {

  uxTextInput::uxTextInput(textInputStyle_t& style, const char* initialContent) : style(style) {
    WITE::strcpy(content, initialContent, guiText_maxCharsPerString-1);
    content[guiText_maxCharsPerString-1] = 0;
    guiTextFormat(textContent, "%s", content);
    caretData.color = style.caretColor;
  };

  uxTextInput::~uxTextInput() {
    rect.destroy();
    text.destroy();
    caret.destroy();//destroy has embedded existance check
  };

  void uxTextInput::update() {
    if(!isVisible()) [[unlikely]] return;
    WITE::winput::compositeInputData lmbCid;
    glm::vec2 mouseInSnorm;
    bool updated = false, hover = isHovered(&mouseInSnorm);
    WITE::winput::getInput(WITE::winput::lmb, lmbCid);
    auto& lmb = lmbCid.axes[0];
    bool isFocused = this->isFocused();
    if(lmb.isPressed()) [[unlikely]] {
      updated = true;
      if(hover) [[unlikely]] {
	insertPnt = WITE::min(static_cast<uint32_t>((mouseInSnorm.x - textData.bbox.x - style.textNormal.charMetric.z + style.textNormal.charMetric.x/2) / style.textNormal.charMetric.x), static_cast<uint32_t>(std::strlen(content)));
      }
    }
    if(isFocused) [[unlikely]] {
      bool shiftDown = WITE::winput::keyPressed<SDLK_LSHIFT>() || WITE::winput::keyPressed<SDLK_RSHIFT>();
      for(const uint32_t&key : WITE::winput::frameKeyboardBuffer) {
	if(key == 0) [[unlikely]] break;
	char insert = 0;
	int insertPntDelta = 0;
	switch(key) {
	case SDLK_KP_ENTER:
	case SDLK_RETURN:
	case SDLK_ESCAPE:
	  removeFocus();
	  break;
	case SDLK_TAB:
	  shiftFocus(!shiftDown);
	  break;
	  //TODO non-qwerty layouts (mostly what symbols are shifted of what base keys)
	case SDLK_UP:
	  //TODO shift focus?
	  break;
	case SDLK_DOWN:
	  //TODO shift focus?
	  break;
	case SDLK_LEFT:
	  insertPntDelta = -1;
	  break;
	case SDLK_RIGHT:
	  insertPntDelta = 1;
	  break;
	case SDLK_END:
	  insertPnt = std::strlen(content);
	  updated = true;
	  break;
	case SDLK_HOME:
	  insertPnt = 0;
	  updated = true;
	  break;
	case SDLK_BACKSPACE:
	  if(insertPnt == 0) [[unlikely]] break;
	  insertPnt--;
	case SDLK_DELETE:
	  {
	    size_t len = std::strlen(content);
	    if(insertPnt >= len) [[unlikely]] break;
	    std::memmove(content + insertPnt, content + insertPnt + 1, len - insertPnt);//copies null term too
	  }
	  updated = true;
	  break;
	case SDLK_KP_0: insert = '0'; break;
	case SDLK_KP_1: insert = '1'; break;
	case SDLK_KP_2: insert = '2'; break;
	case SDLK_KP_3: insert = '3'; break;
	case SDLK_KP_4: insert = '4'; break;
	case SDLK_KP_5: insert = '5'; break;
	case SDLK_KP_6: insert = '6'; break;
	case SDLK_KP_7: insert = '7'; break;
	case SDLK_KP_8: insert = '8'; break;
	case SDLK_KP_9: insert = '9'; break;
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
	case SDLK_KP_MULTIPLY:
	case '*': insert = shiftDown ? '*' : '*'; break;
	case SDLK_KP_PLUS:
	case '+': insert = shiftDown ? '+' : '+'; break;
	case ',': insert = shiftDown ? '<' : ','; break;
	case SDLK_KP_MINUS:
	case '-': insert = shiftDown ? '_' : '-'; break;
	case SDLK_KP_PERIOD:
	case '.': insert = shiftDown ? '>' : '.'; break;
	case SDLK_KP_DIVIDE:
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
	if(insert) [[likely]] {
	  size_t newLen = WITE::min(std::strlen(content) + 1, sizeof(content) - 1, maxLen, (textData.bbox.z - textData.bbox.x - style.textNormal.charMetric.z) / style.textNormal.charMetric.x - 1);
	  std::memmove(content + insertPnt + 1, content + insertPnt, newLen - insertPnt);
	  ASSERT_TRAP(insertPnt < sizeof(content), "overflow");
	  ASSERT_TRAP(newLen < sizeof(content), "overflow");
	  content[insertPnt] = insert;
	  insertPntDelta = 1;
	  content[newLen] = 0;
	  updated = true;
	}
	if(insertPntDelta) [[likely]] {
	  updated = true;
	  if(-insertPntDelta >= static_cast<int>(insertPnt))
	    insertPnt = 0;
	  else
	    insertPnt = WITE::min(insertPnt + insertPntDelta, std::strlen(content));
	}
      }
    }
    if(updated) [[unlikely]] {
      guiTextFormat(textContent, "%s", content);
      if(isFocused) [[likely]]
	updateCaretData();
    }
    if(isFocused) [[unlikely]] {
      if(!caret.onionObj) [[unlikely]]
	caret = guiInputCaret::create();
    } else if(caret.onionObj) {
      caret.destroy();
    }
    if(rect) [[likely]] {
      rect.writeInstanceData(rectData);
      rect.setStyle(isFocused ? style.rectFocusedBuf : style.rectNormalBuf);
    }
    if(text) [[likely]] {
      text.writeInstanceData(textData);
      text.setStyle(isFocused ? style.textFocusedBuf : style.textNormalBuf);
      text.writeIndirectBuffer(textContent);
    }
    if(caret) [[unlikely]]
      caret.writeInstanceData(caretData);
  };

  void uxTextInput::updateCaretData() {
    caretData.bbox.x = textData.bbox.x + style.textNormal.charMetric.z + style.textNormal.charMetric.x * insertPnt - style.textNormal.charMetric.x/4;
    caretData.bbox.y = textData.bbox.y + style.textNormal.charMetric.w;
    caretData.bbox.z = caretData.bbox.x + style.textNormal.charMetric.x/2;
    caretData.bbox.w = caretData.bbox.y + style.textNormal.charMetric.y;
  };

  void uxTextInput::destroy() {
    rect.destroy();
    text.destroy();
    caret.destroy();
  };

  void uxTextInput::create() {
    if(!rect) [[likely]] {
      rect = guiRectVolatile::create();
      rect.writeInstanceData(rectData);
      rect.setStyle(style.rectNormalBuf);
    }
    if(!text) [[likely]] {
      text = guiTextVolatile::create();
      text.writeIndirectBuffer(textContent);
      text.writeInstanceData(textData);
      text.setStyle(style.textNormalBuf);
    }
    if(isFocused() && !caret) [[likely]] {
      caret = guiInputCaret::create();
      updateCaretData();
      caret.writeInstanceData(caretData);
    }
  };

  const glm::vec4& uxTextInput::getBounds() const {
    return rectData.extents;
  };

  void uxTextInput::setBounds(const glm::vec4& bbox) {
    rectData.extents = textData.bbox = bbox;
    updateCaretData();
  };

  void uxTextInput::updateVisible(bool parentVisible) {
    bool shouldBeVisible = parentVisible && wantsVisibility;
    if(shouldBeVisible == visible) [[likely]] return;
    if(shouldBeVisible)
      create();
    else
      destroy();
    visible = shouldBeVisible;
  };

}
