/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "uiStyle.hpp"

namespace doh {

  void buttonStyle_t::pushToBuffers() {
    textHovBuf.slowOutOfBandSet(textHov);
    textNormalBuf.slowOutOfBandSet(textNormal);
    textPressBuf.slowOutOfBandSet(textPress);
    rectHovBuf.slowOutOfBandSet(rectHov);
    rectNormalBuf.slowOutOfBandSet(rectNormal);
    rectPressBuf.slowOutOfBandSet(rectPress);
  };

  buttonStyle_t& btnBig() {
    static buttonStyle_t ret;
    return ret;
  };

  buttonStyle_t& btnHuge() {
    static buttonStyle_t ret;
    return ret;
  };

  buttonStyle_t& btnNormal() {
    static buttonStyle_t ret;
    return ret;
  };

  buttonStyle_t& btnSmall() {
    static buttonStyle_t ret;
    return ret;
  };

  void sliderStyle_t::pushToBuffers() {
    rectBarNormalBuf.slowOutOfBandSet(rectBarNormal);
    rectBarHovBuf.slowOutOfBandSet(rectBarHov);
    rectBarPressBuf.slowOutOfBandSet(rectBarPress);
    rectIndNormalBuf.slowOutOfBandSet(rectIndNormal);
    rectIndHovBuf.slowOutOfBandSet(rectIndHov);
    rectIndPressBuf.slowOutOfBandSet(rectIndPress);
  };

  sliderStyle_t& sliderStyle() {
    static sliderStyle_t ret;
    return ret;
  };

  void textInputStyle_t::pushToBuffers() {
    textFocusedBuf.slowOutOfBandSet(textFocused);
    textNormalBuf.slowOutOfBandSet(textNormal);
    rectFocusedBuf.slowOutOfBandSet(rectFocused);
    rectNormalBuf.slowOutOfBandSet(rectNormal);
  };

  textInputStyle_t& textInputNormal() {
    static textInputStyle_t ret;
    return ret;
  };

  textInputStyle_t& textInputBig() {
    static textInputStyle_t ret;
    return ret;
  };

  void textOnlyStyle_t::pushToBuffers() {
    textBuf.slowOutOfBandSet(text);
    rectBuf.slowOutOfBandSet(rect);
  };

  textOnlyStyle_t& textOnlyHuge() {
    static textOnlyStyle_t ret;
    return ret;
  };

  textOnlyStyle_t& textOnlyBig() {
    static textOnlyStyle_t ret;
    return ret;
  };

  textOnlyStyle_t& textOnlyNormal() {
    static textOnlyStyle_t ret;
    return ret;
  };

  textOnlyStyle_t& textOnlySmall() {
    static textOnlyStyle_t ret;
    return ret;
  };

}

