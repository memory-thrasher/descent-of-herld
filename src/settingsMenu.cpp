/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "settingsMenu.hpp"
#include "mainMenu.hpp"
#include "dbType.hpp"
#include "uiStyle.hpp"
#include "guiLabel.hpp"
#include "guiButton.hpp"
#include "guiTextInput.hpp"

namespace doh {

  namespace settingsMenu_internals {

    struct transients_t {
      dbWrapper owner;
      guiButton exitBtn;
      guiTextInput input;
      bool deleteMe = false;
      transients_t(dbWrapper owner) :
	owner(owner),
	exitBtn(btnHuge(), { -0.95f, -0.95f + btnHuge().height * 1.25f * 0 }, "Back",
		guiButton::clickAction_F::make([this](guiButton*){
		  deleteMe = true;
		  dbTypeFactory<mainMenu>(this->owner).construct();
		})),
	input(textInputNormal(), { -0.95f, 0, 0, btnNormal().height }, "Text Here.") {};
    };

    static_assert(WITE::dbAllocationBatchSizeOf<settingsMenu>::value == 1);
    static_assert(WITE::dbLogAllocationBatchSizeOf<settingsMenu>::value == 1);

    transients_t* getTransients(uint64_t oid, void* dbv) {
      dbType<settingsMenu> dbmm(oid, dbv);
      settingsMenu mm;
      dbmm.readCurrent(&mm);//current because old pointers don't help
      return reinterpret_cast<transients_t*>(mm.transients);
    };

  }

  using namespace settingsMenu_internals;//I know that's kinda lazy

  // void settingsMenu::allocated(uint64_t oid, void* dbv) {
  // };

  // void settingsMenu::freed(uint64_t oid, void* dbv) {
  // };

  static_assert(WITE::has_update<settingsMenu>::value);
  void settingsMenu::update(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    transients->exitBtn.update();
    transients->input.update();
    if(transients->deleteMe) [[unlikely]]
      dbType<settingsMenu>(oid, dbv).destroy();
  };

  void settingsMenu::spunUp(uint64_t oid, void* dbv) {
    dbType<settingsMenu> dbmm(oid, dbv);
    settingsMenu mm;
    dbmm.readCurrent(&mm);
    auto* transients = new transients_t(dbv);
    mm.transients = reinterpret_cast<void*>(transients);
    dbmm.write(&mm);
  };

  void settingsMenu::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    delete transients;
  };

}

