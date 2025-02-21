/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "uxInteractable.hpp"
#include "input.hpp"

namespace doh {

  std::list<uxInteractable*> uxInteractable::allInteractables;
  WITE::syncLock uxInteractable::allInteractables_mutex;
  uxInteractable* uxInteractable::focused = NULL;
  WITE::syncLock uxInteractable::focused_mutex;
  uint32_t uxInteractable::focusedChangeTime;
  uxInteractable::focusMode_e uxInteractable::focusMode;

  uxInteractable::uxInteractable() {
    WITE::scopeLock lock(&allInteractables_mutex);
    allInteractables.push_back(this);
  };

  uxInteractable::~uxInteractable() {
    WITE::scopeLock lock(&allInteractables_mutex);
    if(focused == this) [[unlikely]] {
      removeFocus();//fire listeners
      focused = NULL;//but don't let them block it
    }
    allInteractables.remove(this);
  };

  bool uxInteractable::isFocused() {
    //not locked: if this happens concurrently with a change, both answers are equally valid
    return focused == this;
  };

  void uxInteractable::gainFocus() {
    WITE::scopeLock lock(&focused_mutex);
    if(focused == this) [[unlikely]] return;
    if(focused) [[likely]]
      if(!focused->onFocusChanged(false)) [[unlikely]]
	return;
    focusedChangeTime = WITE::winput::getFrameStart();
    if(!onFocusChanged(true)) [[unlikely]] {
      focused = NULL;//we rejected focus, old holder was already notified of loss, so now nothing is focused
      return;
    }
    focused = this;
  };

  void uxInteractable::globalUpdate() {//static
    WITE::winput::compositeInputData mouse;
    WITE::winput::getInput(WITE::winput::mouse, mouse);
    if(mouse.axes[0].justChanged() || mouse.axes[1].justChanged()) [[unlikely]]
      focusMode = focusMode_e::mouse;
    bool keyPressed = false;
    controlValue cv;
    getControlValue(globalAction::menuDown, cv);
    if(cv.justPressed()) [[unlikely]] {
      shiftFocus({0, 1});
      keyPressed = true;
    }
    getControlValue(globalAction::menuRight, cv);
    if(cv.justPressed()) [[unlikely]] {
      shiftFocus({1, 0});
      keyPressed = true;
    }
    getControlValue(globalAction::menuLeft, cv);
    if(cv.justPressed()) [[unlikely]] {
      shiftFocus({-1, 0});
      keyPressed = true;
    }
    getControlValue(globalAction::menuUp, cv);
    if(cv.justPressed()) [[unlikely]] {
      shiftFocus({0, -1});
      keyPressed = true;
    }
    getControlValue(globalAction::menuSelect, cv);
    if(focused && cv.justReleased()) [[unlikely]]
      focused->onActivate();
    getControlValue(globalAction::menuNext, cv);
    if(cv.justPressed()) [[unlikely]] {
      shiftFocus(true);
      keyPressed = true;
    }
    getControlValue(globalAction::menuLast, cv);
    if(cv.justPressed()) [[unlikely]] {
      shiftFocus(false);
      keyPressed = true;
    }
    if(keyPressed) [[unlikely]]
      focusMode = focusMode_e::key;
  };

  void uxInteractable::manageFocus() {
    if(focusedChangeTime == WITE::winput::getFrameStart()) [[likely]] return;
    //focusedChangeTime: prevent cascade if the focus receiver gets updated later on the same frame
    if(focusMode == focusMode_e::mouse) [[likely]] {
      if(isHovered()) [[unlikely]]
	gainFocus();
      else if(isFocused()) [[unlikely]]
	removeFocus();
    }
    if(isFocused() && !isVisible()) [[unlikely]]
      removeFocus();
  };

  void uxInteractable::onActivate() {};//default no-op

  void uxInteractable::addFocusListener(focusEvent fe) {
    WITE::scopeLock lock(&focusListeners_mutex);
    focusListeners.push_back(fe);
  };

  void uxInteractable::removeFocusListener(focusEvent fe) {
    WITE::scopeLock lock(&focusListeners_mutex);
    focusListeners.remove(fe);
  };

  bool uxInteractable::onFocusChanged(bool acquiringFocus) {
    bool ret = true;
    WITE::scopeLock lock(&focusListeners_mutex);
    for(focusEvent& fe : focusListeners)
      ret &= fe(acquiringFocus);
    return ret;
  };

  void uxInteractable::shiftFocus(glm::vec2 dir) {//static
    WITE::scopeLock lockF(&focused_mutex);
    WITE::scopeLock lock(&allInteractables_mutex);
    if(dir == glm::vec2{0, 0}) [[unlikely]] return;//no change requested
    if(!focused) [[unlikely]] {
      focused = allInteractables.front();
      focusedChangeTime = WITE::winput::getFrameStart();
      return;
    }
    glm::vec2 src = focused->getBounds();
    uxInteractable* best = NULL;
    float bestScore = std::numeric_limits<float>::infinity();
    for(uxInteractable* ui : allInteractables) {
      if(ui == focused || !ui->isVisible()) [[unlikely]] continue;
      glm::vec2 dst = ui->getBounds(),
	delta = dst - src,
	proj = { delta.x * dir.x + delta.y * dir.y, delta.x * dir.y - delta.y * dir.x };
      if(proj.x <= 0) [[unlikely]] continue;
      float score = 2*abs(proj.y) + proj.x;//lower = better
      if(score < bestScore) [[unlikely]] {
	bestScore = score;
	best = ui;
      }
    }
    if(best == NULL) [[unlikely]] return;
    if(!focused->onFocusChanged(false)) [[unlikely]] return;
    focusedChangeTime = WITE::winput::getFrameStart();
    if(!best->onFocusChanged(true)) [[unlikely]] {
      focused = NULL;
      return;
    }
    focused = best;
  };

  void uxInteractable::shiftFocus(bool next) {//static
    WITE::scopeLock lockF(&focused_mutex);
    WITE::scopeLock lock(&allInteractables_mutex);
    if(!focused) [[unlikely]] {
      focused = allInteractables.front();
      focusedChangeTime = WITE::winput::getFrameStart();
      return;
    }
    auto end = allInteractables.end(),
      begin = allInteractables.begin(),
      i = begin;
    while(i != end && *i != focused) ++i;
    ASSERT_TRAP(i != end, "focused on something not found in list");
    if(next) {
      ++i;
      if(i == end) [[unlikely]] i = begin;
    } else {
      if(i == begin) [[unlikely]]
	i = end;
      --i;
    }
    uxInteractable* nextUx = *i;
    if(!focused->onFocusChanged(false)) [[unlikely]] return;
    focusedChangeTime = WITE::winput::getFrameStart();
    if(!nextUx->onFocusChanged(true)) [[unlikely]] {
      focused = NULL;
      return;
    }
    focused = nextUx;
  };

  void uxInteractable::removeFocus() {//static
    WITE::scopeLock lock(&focused_mutex);
    if(!focused) [[unlikely]] return;
    if(!focused->onFocusChanged(false)) [[unlikely]] return;
    focusedChangeTime = WITE::winput::getFrameStart();
    focused = NULL;
  };

  bool uxInteractable::anythingIsFocused() {//static
    return focused;//not locked: if this happens concurrently with a change, both answers are equally valid
  };

}
