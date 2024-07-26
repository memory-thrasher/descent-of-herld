/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <set>

#include "mainMenu.hpp"
#include "dbFull.hpp"
#include "cameraStuff.hpp"
#include "uiStyle.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "math.hpp"
#include "guiButton.hpp"

namespace doh {

  struct transients_t {
    targetPrimary camera = targetPrimary::create();
    guiButton buttons[1] {
      { btnHuge(), { -0.95f, -0.95f }, "Continue" },
    };
  };

  transients_t* getTransients(uint64_t oid, void* dbv) {
    auto db = reinterpret_cast<db_t*>(dbv);
    mainMenu mm;
    db->readCurrent<mainMenu>(oid, &mm);//current because old pointers don't help
    return reinterpret_cast<transients_t*>(mm.transients);
  };

  void mainMenu::allocated(uint64_t oid, void* dbv) {
  };

  void mainMenu::freed(uint64_t oid, void* dbv) {
  };

  static_assert(WITE::has_update<mainMenu>::value);
  void mainMenu::update(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    const auto size = transients->camera.getWindow().getVecSize();
    cameraData_t cameraData { { size, 0, 0 } };
    transients->camera.writeCameraData(cameraData);
    for(auto& btn : transients->buttons)
      btn.update();
  };

  void mainMenu::spunUp(uint64_t oid, void* dbv) {
    auto db = reinterpret_cast<db_t*>(dbv);
    mainMenu mm;
    // WITE::scopeLock lock(db->mutexFor<mainMenu>(oid));//note: not necessary in allocation
    db->read<mainMenu>(oid, 0, &mm);
    mm.transients = reinterpret_cast<void*>(new transients_t());
    db->write<mainMenu>(oid, &mm);
  };

  void mainMenu::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    transients->camera.destroy();
    delete transients;
  };

}

