/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

namespace doh {

  /*
    construct 1→n constructModule 1→n constructComponent
      n:1 ↓ copy          n:1 ↓ copy              n:1 ↓ copy
    constructDesign 1→n constructDesignModule 1→n constructDesignComponent
    modules are of a configurable size bbox in whole meters, laid out in a grid
    each module contains a specific number of component slots depending on size and type of module
    component slots have component type restrictions
   */

  struct constructDesign {
    static constexpr size_t labelMaxChars = 128;
    char label[labelMaxChars + 1];//null term
    uint16_t version;
    uint8_t pad[4096-129-2];
  };
  static_assert(sizeof(constructDesign) == 4096);

}
