/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include <list>

#include "uxPanel.hpp"
#include "uxButtonVolatile.hpp"
#include "uxGridLayout.hpp"

namespace doh {

  struct uxTabbedView;

  struct uxTab {
    uxButtonVolatile btn;
    uxPanel panel;
    uxTab(uxTabbedView* owner, std::string label);
    uxTab(const uxTab&) = delete;
  };

  struct uxTabbedView : public uxBase {
    std::list<uxTab> tabs;
    uxPanel btnPanel;
    uxGridLayout btnPanelLayout;
    uxTab* currentTab;
    buttonStyle_t& btnStyle;
    glm::vec2 padding;
    glm::vec4 bounds;
    uxTabbedView(buttonStyle_t& btnStyle, const glm::vec2& padding = { 0.005f, 0.1f });
    uxTabbedView(float btnWidth, buttonStyle_t& btnStyle, const glm::vec2& padding = { 0.005f, 0.1f });
    uxTabbedView(buttonStyle_t& btnStyle, uint32_t btnChars, const glm::vec2& padding = { 0.005f, 0.1f });
    uxTabbedView(const uxTabbedView&) = delete;
    virtual ~uxTabbedView() = default;
    void redraw();
    void setCurrentTab(uxTab*, uxButtonVolatile* = NULL);
    uxTab& emplaceTab(std::string btnLabel);//call redraw() after (batch of) emplace
    glm::vec4 getContentBounds();
    glm::vec4 getBtnBounds();
    virtual void update() override;
    virtual const glm::vec4& getBounds() const override;
    virtual void setBounds(const glm::vec4&) override;
    virtual void updateVisible(bool parentVisible) override;
  };

}
