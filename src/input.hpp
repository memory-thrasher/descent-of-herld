/*
  Copyright 2024-2025 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include <vector>
#include <WITE/WITE.hpp>

namespace doh {

  struct controllerId {
    WITE::winput::type_e type;
    uint64_t id;
    controllerId(const WITE::winput::inputIdentifier& i) : type(i.type), id(i.controllerId) {};
    auto operator<=>(const controllerId& o) const = default;
  };

  struct controller {
    controllerId id;
    char label[33];
  };

  struct control : public WITE::winput::inputIdentifier {
    static constexpr uint8_t nullAxis = 255;
    uint8_t axisId;
    auto operator<=>(const control&) const = default;
  };

  //this is stored in a dbFile, keep it PoD
  struct controlConfiguration {
    control id;
    float min, max, deadzone;
    char label[64];
  };

  //this is stored in a dbFile, keep it PoD
  struct controlActionMapping {
    control controlId;
    uint32_t actionId;//indexed
  };

  struct controlValue : public WITE::winput::compositeInputData::axis {
    float normalizedValue;
  };

  typedef WITE::dbFile<controlConfiguration, 4096> inputConfigFile_t;

  //category and action details are mostly for the control binding menu
  enum class controlActionCategory : uint32_t {
    menuNavigation, schematic, flightDefaults
  };

  int32_t getSdlJoyIdxFromControllerId(uint64_t);
  std::string getSysName(const controllerId&);
  std::string getSysName(const control&);//does not include controller name
  controller& getController(const controllerId&);
  controlConfiguration* getControl(const inputConfigFile_t::iterator_t&);
  controlConfiguration* getControl(const control&);
  controlConfiguration* getControl(uint32_t actionId);
  void deleteControl(const control& c);
  void getControlValue(const control&, controlValue& out);
  inputConfigFile_t::iterator_t getControlBegin();
  inputConfigFile_t::iterator_t getControlEnd();
  controlActionMapping& getControlActionMapping(uint32_t actionId);
  std::string actionCategoryToString(controlActionCategory);

  enum class globalAction : uint32_t {
    //note: never change an id, only append
    menuDown, menuUp, menuLeft, menuRight, menuSelect, menuBack, menuNext, menuLast
  };

  struct controlActionDetails {
    globalAction actionId;
    controlActionCategory categoryId;
    const char*label;
  };

  //keep in same order as globalAction
  constexpr controlActionDetails globalActionDetails[] {//more actions are stored within game files per-ship
    { globalAction::menuDown, controlActionCategory::menuNavigation, "Menu Down" },
    { globalAction::menuUp, controlActionCategory::menuNavigation, "Menu Up" },
    { globalAction::menuLeft, controlActionCategory::menuNavigation, "Menu Left" },
    { globalAction::menuRight, controlActionCategory::menuNavigation, "Menu Right" },
    { globalAction::menuSelect, controlActionCategory::menuNavigation, "Menu Select" },
    { globalAction::menuBack, controlActionCategory::menuNavigation, "Menu Back" },
    { globalAction::menuNext, controlActionCategory::menuNavigation, "Menu Next" },
    { globalAction::menuLast, controlActionCategory::menuNavigation, "Menu Last" },
  };

}

