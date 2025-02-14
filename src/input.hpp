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

  struct controlAction {
    uint32_t id;
    const char*label;
  };

  struct control : public WITE::winput::inputIdentifier {
    uint8_t axisId;
    auto operator<=>(const control&) const = default;
  };

  //this is stored in a dbFile, keep it PoD
  struct controlConfiguration {
    control id;
    uint32_t action;//indexed
    float min, max, deadzone;
    char label[64];
  };

  struct controlValue : public WITE::winput::compositeInputData::axis {
    float normalizedValue;
  };

  typedef WITE::dbFile<controlConfiguration, 4096> inputConfigFile_t;
  typedef WITE::dbFile<controller, 4> controllersFile_t;

  std::string getSysName(const controllerId&);
  controller& getController(const controllerId&);
  controlConfiguration* getControl(const inputConfigFile_t::iterator_t&);
  controlConfiguration* getControl(const control&);
  controlConfiguration* getControl(uint32_t controlActionId);
  void deleteControl(const control& c);
  void getControlValue(const control&, controlValue& out);
  inputConfigFile_t::iterator_t getControlBegin();
  inputConfigFile_t::iterator_t getControlEnd();
  std::string to_string(const control&);

  //TODO list of actions

}

