/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include "uxBase.hpp"
#include "uiStyle.hpp"
#include "../generated/guiRect_stub.hpp"
#include "../generated/guiRectVolatile_stub.hpp"
#include "../generated/guiText_stub.hpp"

namespace doh {

  struct uxSlider : public uxBase {
    typedefCB(updateAction, void, uxSlider*);
    guiRect rectBar;
    guiRectInstance_t rectBarData;
    guiRectVolatile rectInd;
    guiRectInstance_t rectIndData;
    //note: style.pad is external (see uxPanel)
    glm::vec4 bounds;//since neither other rect is the complete bounds
    glm::vec2 value;//0-domain.xy
    glm::vec2 domain;//max value in each direction, negative to lock axis (generally, in pages)
    glm::vec2 dragOffset;
    sliderStyle_t& style;//keeping this ref just in case more styles exist later
    bool isPressed = false;
    uxSlider();
    uxSlider(sliderStyle_t& style, const glm::vec2& domain, const glm::vec4& bounds);
    uxSlider(sliderStyle_t& style);
    uxSlider(const uxSlider&) = delete;
    ~uxSlider();
    glm::vec2 getIndicatorSize();
    void redraw();
    void setDomain(const glm::vec2&);
    void destroy();
    void create();
    virtual void update() override;
    virtual const glm::vec4& getBounds() const override;
    virtual void setBounds(const glm::vec4&) override;
    virtual void updateVisible(bool parentVisible) override;
  };

}
