/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "uxTabbedView.hpp"

namespace doh {

  uxTab::uxTab(uxTabbedView* owner, std::string label) :
    btn(owner->btnStyle, label,
	uxButtonVolatile::clickAction_F::make<uxTabbedView, uxTab*>(owner, this, &uxTabbedView::setCurrentTab))
  {};

  uxTabbedView::uxTabbedView(buttonStyle_t& btnStyle, const glm::vec2& padding) :
    btnPanelLayout(btnStyle.height, { btnStyle.width }, padding),
    currentTab(NULL),
    btnStyle(btnStyle),
    padding(padding)
  {
    btnPanel.setLayout(&btnPanelLayout);
  };

  void uxTabbedView::redraw() {
    bool wasVisible = isVisible();
    updateVisible(false);
    btnPanel.clear();
    btnPanel.setBounds(getBtnBounds());
    glm::vec4 content = getContentBounds();
    for(uxTab& t : tabs) {
      btnPanel.push(&t.btn);
      t.btn.setBounds(content);
    }
    btnPanel.redraw();
    updateVisible(wasVisible);
  };

  void uxTabbedView::setCurrentTab(uxTab* t, uxButtonVolatile*) {
    if(currentTab == t || t == NULL) [[unlikely]] {// click current tab = hide current tab
      if(currentTab) [[likely]]
	currentTab->panel.updateVisible(false);
      currentTab = NULL;
    } else {
      currentTab->panel.updateVisible(false);
      currentTab = t;
      currentTab->panel.updateVisible(true);
    }
  };

  uxPanel& uxTabbedView::emplaceTab(std::string btnLabel) {
    uxTab& t = tabs.emplace_back(this, btnLabel);
    return t.panel;
  };

  glm::vec4 uxTabbedView::getContentBounds() {
    return { bounds.x + btnStyle.width + padding.x, bounds.y, bounds.z, bounds.w };
  };

  glm::vec4 uxTabbedView::getBtnBounds() {
    return { bounds.x, bounds.y, bounds.x + btnStyle.width, bounds.w };
  };

  void uxTabbedView::update() {
    for(uxTab& ut : tabs)
      ut.panel.update();
    btnPanel.update();
  };

  const glm::vec4& uxTabbedView::getBounds() const {
    return bounds;
  };

  void uxTabbedView::setBounds(const glm::vec4& v) {
    bounds = v;
    redraw();
  };

  void uxTabbedView::updateVisible(bool parentVisible) {
    bool v = isVisible() && parentVisible;
    for(uxTab& ut : tabs)
      ut.panel.updateVisible(v && &ut == currentTab);
    btnPanel.updateVisible(v);
  };

}
