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

#include <WITE/WITE.hpp>

#include "uxSlider.hpp"
#include "math.hpp"
#include "uiStyle.hpp"

namespace doh {

  uxSlider::uxSlider(sliderStyle_t& style) : style(style) {
    wantsVisibility = true;
  };

  uxSlider::uxSlider(sliderStyle_t& style, const glm::vec2& domain, const glm::vec4& bounds) :
    bounds(bounds), domain(domain), style(style) {
    wantsVisibility = true;
  };

  uxSlider::uxSlider() : uxSlider(sliderStyle()) {};

  uxSlider::~uxSlider() {
    destroy();
  };

  void uxSlider::update() {
    if(!isVisible()) [[unlikely]] return;
    WITE::winput::compositeInputData lmbCid;
    glm::vec2 mouseInSnorm;
    bool isHovered = this->isHovered(&mouseInSnorm);
    // glm::vec2 mouseInBounds = mouseInSnorm - glm::vec2(bounds);
    WITE::winput::getInput(WITE::winput::lmb, lmbCid);
    auto& lmb = lmbCid.axes[0];
    if(!lmb.isPressed()) [[likely]]
      isPressed = false;
    else if(isHovered && lmb.justPressed()) [[unlikely]] {
      isPressed = true;
      if(rectContainsPoint(rectIndData.extents, mouseInSnorm)) [[likely]]
	dragOffset = mouseInSnorm - glm::vec2(rectIndData.extents);
      else
	//began pressing when over rect but not over indicator, so jump to that spot
	dragOffset = { (rectIndData.extents.z - rectIndData.extents.x) / 2, (rectIndData.extents.w - rectIndData.extents.y) / 2 };
    } else if(!isPressed) [[likely]]
      //dragging on does not proc hover
      isHovered = false;
    glm::vec2 indicatorSize = getIndicatorSize();
    glm::vec2 valueOld = value;
    if(isPressed) [[unlikely]] {
      value.x = domain.x <= 0 ? 0 : domain.x * (mouseInSnorm.x - dragOffset.x - bounds.x) / (bounds.z - indicatorSize.x - bounds.x);
      value.y = domain.y <= 0 ? 0 : domain.y * (mouseInSnorm.y - dragOffset.y - bounds.y) / (bounds.w - indicatorSize.y - bounds.y);
    }
    if(value.x > domain.x) [[unlikely]] value.x = domain.x;
    if(value.x < 0) [[unlikely]] value.x = 0;
    if(value.y > domain.y) [[unlikely]] value.y = domain.y;
    if(value.y < 0) [[unlikely]] value.y = 0;
    if(valueOld != value) [[unlikely]]
      for(auto& ua : updateListeners)
	ua(this);
    //notify listeners before updating the UI just in case a listener changes the value
    rectIndData.extents.x = domain.x <= 0 ? (bounds.x + bounds.z - indicatorSize.x)/2 :
      bounds.x + value.x * (bounds.z - indicatorSize.x - bounds.x) / domain.x;
    rectIndData.extents.y = domain.y <= 0 ? (bounds.y + bounds.w - indicatorSize.y)/2 :
      bounds.y + value.y * (bounds.w - indicatorSize.y - bounds.y) / domain.y;
    rectIndData.extents.z = rectIndData.extents.x + indicatorSize.x;
    rectIndData.extents.w = rectIndData.extents.y + indicatorSize.y;
    ASSERT_TRAP(rectBar, "isVisible() returned true earlier (", isVisible(), " now) but no rectBar");
    ASSERT_TRAP(rectInd, "isVisible() returned true earlier (", isVisible(), " now) but no rectInd");
    rectInd.writeInstanceData(rectIndData);
    rectBar.writeInstanceData(rectBarData);
    rectBar.setStyle(isPressed ? style.rectBarPressBuf : isHovered ? style.rectBarHovBuf : style.rectBarNormalBuf);
    rectInd.setStyle(isPressed ? style.rectIndPressBuf : isHovered ? style.rectIndHovBuf : style.rectIndNormalBuf);
// #ifdef DEBUG
//     static int hits = 0;
//     if((hits++) > 10 && hits < 15) [[unlikely]]
//       WARN("slider ", this, " at: ", bounds, " with bar: ", rectBarData.extents, " with indicator: ", rectIndData.extents, " from value: ", value, " in domain: ", domain);
// #endif
  };

  void uxSlider::addListener(const updateAction& ua) {
    updateListeners.emplace_back(ua);
  };

  glm::vec2 uxSlider::getIndicatorSize() {
    //the size of the indicator compared to the bar (bounds) should be proportional to the size of the viewport (1 "page" = 1 in domain space) compared to the scrollable content (domain)
    //ret.x/bounds.width = 1/domain.x
    return { domain.x <= 0 ? style.indicatorThickness : WITE::max(style.indicatorMinLen, (bounds.z - bounds.x) / (domain.x + 1)), domain.y <= 0 ? style.indicatorThickness : WITE::max(style.indicatorMinLen, (bounds.w - bounds.y) / (domain.y + 1)) };
  };

  void uxSlider::destroy() {
    rectInd.destroy();
    rectBar.destroy();
  };

  void uxSlider::create() {
    if(!rectInd) [[likely]] {
      rectInd = guiRectVolatile::create();
      rectInd.writeInstanceData(rectIndData);
      rectInd.setStyle(style.rectIndNormalBuf);
      // WARN("slider ", this, " ind created");
    }
    if(!rectBar) [[likely]] {
      rectBar = guiRectVolatile::create();
      rectBar.writeInstanceData(rectBarData);
      rectBar.setStyle(style.rectBarNormalBuf);
      // WARN("slider ", this, " bar created");
    }
  };

  const glm::vec4& uxSlider::getBounds() const {
    return bounds;
  };

  void uxSlider::setBounds(const glm::vec4& bbox) {
    bounds = bbox;
    redraw();
  };

  void uxSlider::setDomain(const glm::vec2& d) {
    domain = d;
    redraw();
  };

  void uxSlider::redraw() {
    //only update the bar, since the indicator gets updated every frame anyway
    glm::vec2 center { (bounds.z - bounds.x)/2, (bounds.w - bounds.y)/2 };
    if(domain.x <= 0) [[likely]] {
      rectBarData.extents.x = (bounds.z + bounds.x - style.barThickness)/2;
      rectBarData.extents.z = rectBarData.extents.x + style.barThickness;
    } else {
      rectBarData.extents.x = bounds.x;
      rectBarData.extents.z = bounds.z;
    }
    if(domain.y <= 0) [[likely]] {
      rectBarData.extents.y = (bounds.w + bounds.y - style.barThickness)/2;
      rectBarData.extents.w = rectBarData.extents.y + style.barThickness;
    } else {
      rectBarData.extents.y = bounds.y;
      rectBarData.extents.w = bounds.w;
    }
    // WARN("slider ", this, " at: ", bounds, " with bar: ", rectBarData.extents, " with indicator: ", rectIndData.extents);
  };

  void uxSlider::updateVisible(bool parentVisible) {
    bool shouldBeVisible = parentVisible && wantsVisibility;
    // WARN("slider ", this, " updateVisible(", parentVisible, ") is setting visible: ", shouldBeVisible, " was: ", visible);
    if(shouldBeVisible == visible) [[likely]] return;
    if(shouldBeVisible)
      create();
    else
      destroy();
    visible = shouldBeVisible;
  };

}
