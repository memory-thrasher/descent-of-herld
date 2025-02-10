/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "uxPanel.hpp"
#include "uxLayout.hpp"
#include "math.hpp"

namespace doh {

  uxPanel::uxPanel() : uxBase(), scrollOffset() {
    auto scrl = uxSlider::updateAction_F::make(this, &uxPanel::onScroll);
    sliderH.addListener(scrl);
    sliderV.addListener(scrl);
  };

  void uxPanel::onScroll(uxSlider* s) {
    if(!s) [[unlikely]] return;//should never happen but not a problem if it does
    glm::vec4 ib = getInnerBounds();
    if(s->domain.x > 0) [[likely]] scrollOffset.x = -s->value.x * (ib.z - ib.x);
    if(s->domain.y > 0) [[likely]] scrollOffset.y = -s->value.y * (ib.w - ib.y);
    redraw();//also updates scroll bars via layout->finalize()
  };

  void uxPanel::setLayout(uxLayout* l) {
    layout = l;
    l->panel = this;
    if(visible) [[unlikely]] //just in case we're changing layouts live, which would be dumb
      redraw();
  };

  void uxPanel::redraw() {
    ASSERT_TRAP(layout, "layout not set");
    // bool wasVisible = isVisible();
    // updateVisible(false);
    layout->reset();
    for(uxBase* c : children)
      layout->handle(c);
    layout->finalize();
    // updateVisible(wasVisible);
  };

  void uxPanel::push(uxBase* b) {
    children.emplace_back(b);
  };

  void uxPanel::clear() {
    if(layout) [[likely]] layout->reset();
    children.clear();
  };

  void uxPanel::updateScrollBars(const glm::vec2& logicalSize) {
    this->logicalSize = logicalSize;
    updateScrollBars();
  };

  void uxPanel::updateScrollBars() {
    glm::vec4 ib = getInnerBounds();
    auto& scrollStyle = sliderStyle();
    glm::vec2 domain { (logicalSize.x + scrollStyle.pad) / (ib.z - ib.x) - 1,
		       (logicalSize.y + scrollStyle.pad) / (ib.w - ib.y) - 1 };
    if(domain.x <= 0) [[unlikely]] domain.x = -1;
    if(domain.y <= 0) [[unlikely]] domain.y = -1;
    // WARN("updateScrollBars: ", logicalSize, " domain: ", domain, " ib: ", ib, " bounds: ", bounds);
    sliderH.setBounds({ bounds.x, ib.w + scrollStyle.pad, ib.z, bounds.w });
    sliderV.setBounds({ ib.z + scrollStyle.pad, bounds.y, bounds.z, ib.w });
    sliderH.setDomain({ domain.x, -1 });
    sliderV.setDomain({ -1, domain.y });
    sliderH.setVisible(domain.x > 0);
    sliderV.setVisible(domain.y > 0);
    updateVisible(isVisible());
  };

  void uxPanel::update() {
    sliderH.update();
    sliderV.update();
    for(uxBase* c : children)
      c->update();
    //TODO scroll wheel
  };

  const glm::vec4& uxPanel::getBounds() const {
    return bounds;
  };

  void uxPanel::setBounds(const glm::vec4& v) {
    bounds = v;
    if(visible) [[unlikely]]
      redraw();
    // WARN("Panel at: ", bounds, " (inner: ", getInnerBounds(), ")");
    updateScrollBars();
  };

  glm::vec4 uxPanel::getInnerBounds() const {
    auto& scrollStyle = sliderStyle();
    float barSpace = WITE::max(scrollStyle.indicatorThickness, scrollStyle.barThickness) + scrollStyle.pad;
    //TODO space for 2d scroll area if both overflow
    return { bounds.x, bounds.y,
	     bounds.w - bounds.y > logicalSize.y ? bounds.z : bounds.z - barSpace,
	     bounds.z - bounds.x > logicalSize.x ? bounds.w : bounds.w - barSpace };
  };

  void uxPanel::updateVisible(bool parentVisible) {
    visible = parentVisible && wantsVisibility;
    sliderH.updateVisible(visible);
    sliderV.updateVisible(visible);
    glm::vec4 usableBounds = getInnerBounds();
    for(uxBase* c : children)
      c->updateVisible(visible && rectContainsRect(usableBounds, c->getBounds()));
  };
  
}
