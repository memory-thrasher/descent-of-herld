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

  uxPanel::uxPanel() : uxBase(), scrollOffset(), scrollbarThickness(0) {};

  void uxPanel::setLayout(uxLayout* l) {
    layout = l;
    l->panel = this;
    if(visible) [[unlikely]] //just in case we're changing layouts live, which would be dumb
      redraw();
  };

  void uxPanel::redraw() {
    ASSERT_TRAP(layout, "layout not set");
    bool wasVisible = isVisible();
    setVisible(false);
    layout->reset();
    for(uxBase* c : children)
      layout->handle(c);
    setVisible(wasVisible);
  };

  void uxPanel::push(uxBase* b) {
    children.emplace_back(b);
  };

  void uxPanel::clear() {
    if(layout) [[likely]] layout->reset();
    children.clear();
  };

  void uxPanel::updateScrollBars(const glm::vec2& logicalSize) {
    WARN("TODO scroll bars");
  };

  void uxPanel::update() {
    for(uxBase* c : children)
      c->update();
  };

  const glm::vec4& uxPanel::getBounds() const {
    return bounds;
  };

  void uxPanel::setBounds(const glm::vec4& v) {
    bounds = v;
    if(visible) [[unlikely]]
      redraw();
  };

  void uxPanel::updateVisible(bool parentVisible) {
    visible = parentVisible && wantsVisibility;
    glm::vec4 usableBounds { bounds.x, bounds.y, bounds.z - scrollbarThickness, bounds.w - scrollbarThickness };
    for(uxBase* c : children)
      c->updateVisible(visible && rectContainsRect(usableBounds, c->getBounds()));
  };
  
}
