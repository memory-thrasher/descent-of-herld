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

#include <WITE/WITE.hpp>//syncLock

#include "uxBase.hpp"

namespace doh {

  struct uxInteractable : public uxBase {
  public:
    typedefCB(focusEvent, bool, bool);//notifies each and asks permission for focus to change
  private:
    static std::list<uxInteractable*> allInteractables;
    static WITE::syncLock allInteractables_mutex;
    static uxInteractable* focused = NULL;
    static WITE::syncLock focused_mutex;
    bool onFocusChanged(bool acquiringFocus);//dispatches focus events
    std::list<focusEvent> focusListeners;
    WITE::syncLock focusListeners_mutex;
  public:
    uxInteractable();
    uxInteractable(const uxInteractable&) = delete;//copy/move is a bad idea bc uxPanel keeps pointers to these
    virtual ~uxInteractable();
    bool isFocused();
    void gainFocus();
    bool manageFocus();//optionally called by child's update to test for menu input events, returns true if this object lost focus as a result of such an input event, false if not, including if this object was not focused in the first place
    void addFocusListener(focusEvent);
    void removeFocusListener(focusEvent);
    static void shiftFocus(glm::vec2 dir);//delta direction, need not be normalized. May be dead end.
    static void shiftFocus(bool next);//shift by order of creation (i.e. tab key)
    static void removeFocus();
    static bool anythingIsFocused();
  };

}
