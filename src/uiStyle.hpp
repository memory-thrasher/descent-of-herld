/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include <WITE/WITE.hpp>

#include "guiRect.hpp"
#include "guiText.hpp"

namespace doh {

  struct buttonStyle_t {
    WITE::buffer<BR_guiRectStyle> rectNormalBuf, rectHovBuf, rectPressBuf;
    WITE::buffer<BR_guiTextStyle> textNormalBuf, textHovBuf, textPressBuf;
    guiRectStyle_t rectNormal, rectHov, rectPress;
    guiTextStyle_t textNormal, textHov, textPress;
    float width, height;
    void pushToBuffers();
  };

  buttonStyle_t& btnBig();
  buttonStyle_t& btnHuge();
  buttonStyle_t& btnNormal();
  buttonStyle_t& btnSmall();

  struct sliderStyle_t {
    WITE::buffer<BR_guiRectStyle> rectBarNormalBuf, rectBarHovBuf, rectBarPressBuf,
      rectIndNormalBuf, rectIndHovBuf, rectIndPressBuf;
    guiRectStyle_t rectBarNormal, rectBarHov, rectBarPress,
      rectIndNormal, rectIndHov, rectIndPress;
    float barThickness, indicatorThickness, pad, indicatorMinLen;
    void pushToBuffers();
  };

  sliderStyle_t& sliderStyle();//only one style for now

  struct textInputStyle_t {
    WITE::buffer<BR_guiRectStyle> rectNormalBuf, rectFocusedBuf;
    WITE::buffer<BR_guiTextStyle> textNormalBuf, textFocusedBuf;
    guiRectStyle_t rectNormal, rectFocused;
    guiTextStyle_t textNormal, textFocused;
    glm::vec4 caretColor;
    void pushToBuffers();
  };

  textInputStyle_t& textInputNormal();
  textInputStyle_t& textInputBig();

  struct textOnlyStyle_t {
    WITE::buffer<BR_guiRectStyle> rectBuf;
    WITE::buffer<BR_guiTextStyle> textBuf;
    guiRectStyle_t rect;
    guiTextStyle_t text;
    void pushToBuffers();
  };

  textOnlyStyle_t& textOnlyHuge();
  textOnlyStyle_t& textOnlyBig();
  textOnlyStyle_t& textOnlyNormal();
  textOnlyStyle_t& textOnlySmall();

}

