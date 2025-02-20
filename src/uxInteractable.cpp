/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "uxInteractable.hpp"

namespace doh {

  uxInteractable::uxInteractable() {
    WITE::scopeLock lock(&allInteractables_mutex);
    allInteractables.push_back(this);
  };

  uxInteractable::~uxInteractable() {
    WITE::scopeLock lock(&allInteractables_mutex);
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
    if(!onFocusChanged(true)) [[unlikely]] {
      focused = NULL;//we rejected focus, old holder was already notified of loss, so now nothing is focused
      return;
    }
    focused = this;
  };

  bool uxInteractable::manageFocus() {
    #error TODO
  };

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

  void uxInteractable::shiftFocus(glm::vec2 dir) {
    WITE::scopeLock lockF(&focused_mutex);
    WITE::scopeLock lock(&allInteractables_mutex);
    if(dir == glm::vec2{0, 0} || !focused) [[unlikely]] return;//no change requested
    glm::vec2 src = focused->getBounds();
    uxInteractable* best = NULL;
    float bestScore = 0;//std::numeric_limits<float>::infinity();
    for(uxInteractable& ui : allInteractables) {
      if(&ui == focused) [[unlikely]] continue;
      glm::vec2 dst = ui.getBounds(),
	delta = dst - src,
	proj = { delta.x * dir.x + delta.y * dir.y, delta.x * dir.y - delta.y * dir.x };
      if(proj.x < 0) [[unlikely]] continue;
      float score = 2*abs(proj.y) - proj.x;//lower = better
      if(score < bestScore) [[unlikely]] {
	bestScore = score;
	best = &ui;
      }
    }
    if(best == NULL) [[unlikely]] return;
    if(!focused->onFocusChanged(false)) [[unlikely]] return;
    if(!best->onFocusChanged(true)) [[unlikely]] {
      focused = NULL;
      return;
    }
    focused = best;
  };//static

  void uxInteractable::shiftFocus(bool next) {
    WITE::scopeLock lockF(&focused_mutex);
    WITE::scopeLock lock(&allInteractables_mutex);
    if(dir == glm::vec2{0, 0} || !focused) [[unlikely]] return;//no change requested
    auto end = allInteractables.end(),
      begin = allInteractables.begin(),
      i = begin;
    while(i != end && *i != focused) ++i;
    ASSERT(i != end, "focused on something not found in list");
    if(next) {
      ++i;
      if(i == end) [[unlikely]] i = begin;
    } else {
      if(i == begin) [[unlikely]]
	i = end;
      --i;
    }
    uxInteractable* next = *i;
    if(!focused->onFocusChanged(false)) [[unlikely]] return;
    if(!next->onFocusChanged(true)) [[unlikely]] {
      focused = NULL;
      return;
    }
    focused = best;
  };//static

  void uxInteractable::removeFocus() {//static
    WITE::scopeLock lock(&focused_mutex);
    if(!focused) [[unlikely]] return;
    if(!focused->onFocusChange(false)) [[unlikely]] return;
    focused = NULL;
  };

  bool uxInteractable::anythingIsFocused() {//static
    return focused;//not locked: if this happens concurrently with a change, both answers are equally valid
  };

}
