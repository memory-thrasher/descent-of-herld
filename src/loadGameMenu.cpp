/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <set>

#include "loadGameMenu.hpp"
#include "mainMenu.hpp"
#include "dbType.hpp"
#include "cameraStuff.hpp"
#include "uiStyle.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "../generated/spaceSkybox_stub.hpp"
#include "math.hpp"
#include "guiButton.hpp"

namespace doh {

  namespace loadGameMenu_internals {

    constexpr size_t saveSlots = 10;

    struct transients_t {
      dbWrapper owner;
      std::vector<guiButton> buttons;
      bool deleteMe = false;
      transients_t(dbWrapper owner) :
	owner(owner) {
	for(size_t i = 0;i < saveSlots;i++) {
	  //TODO harvest date and other data to add to button label
	  buttons.emplace_back(btnHuge(), glm::vec2(-0.95f, -0.95f + btnHuge().height * 1.25f * i),
			       "Slot " + std::to_string(i),
			       guiButton::clickAction_F::make([this, i](guiButton*) {
				 this->deleteMe = true;
				 WARN("TODO load game ", i);
				 dbTypeFactory<mainMenu>(this->owner).construct();
			       }));
	}
	buttons.emplace_back(btnHuge(), glm::vec2(-0.95f, -0.95f + btnHuge().height * 1.25f * (saveSlots + 1)), "Back",
			     guiButton::clickAction_F::make([this](guiButton*){
			       this->deleteMe = true;
			       dbTypeFactory<mainMenu>(this->owner).construct();
			     }));
      };
    };

    static_assert(WITE::dbAllocationBatchSizeOf<loadGameMenu>::value == 1);
    static_assert(WITE::dbLogAllocationBatchSizeOf<loadGameMenu>::value == 1);

    transients_t* getTransients(uint64_t oid, void* dbv) {
      dbType<loadGameMenu> dbmm(oid, dbv);
      loadGameMenu mm;
      dbmm.readCurrent(&mm);//current because old pointers don't help
      return reinterpret_cast<transients_t*>(mm.transients);
    };

  }

  using namespace loadGameMenu_internals;//I know that's kinda lazy

  // void loadGameMenu::allocated(uint64_t oid, void* dbv) {
  // };

  // void loadGameMenu::freed(uint64_t oid, void* dbv) {
  // };

  static_assert(WITE::has_update<loadGameMenu>::value);
  void loadGameMenu::update(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    for(guiButton& btn : transients->buttons)
      btn.update();
    if(transients->deleteMe) [[unlikely]]
      dbType<loadGameMenu>(oid, dbv).destroy();
  };

  void loadGameMenu::spunUp(uint64_t oid, void* dbv) {
    dbType<loadGameMenu> dbmm(oid, dbv);
    loadGameMenu mm;
    dbmm.readCurrent(&mm);
    auto* transients = new transients_t(dbv);
    mm.transients = reinterpret_cast<void*>(transients);
    dbmm.write(&mm);
  };

  void loadGameMenu::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    delete transients;
  };

}

