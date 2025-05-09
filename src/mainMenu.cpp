/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "mainMenu.hpp"
#include "newGameMenu.hpp"
#include "loadGameMenu.hpp"
#include "settingsMenu.hpp"
#include "dbType.hpp"
#include "uiStyle.hpp"
#include "guiLabel.hpp"
#include "guiButton.hpp"

namespace doh {

  namespace mainMenu_internals {

    struct transients_t {
      guiLabel title = { textOnlyHuge(), glm::vec4(-0.25f, -0.95f, 0.25f, -1), "Descent of Herld" };
      dbWrapper owner;
      guiButton continueBtn, loadBtn, newBtn, settingsBtn, exitBtn;
      uxButton*buttons[5];
      bool deleteMe = false;
      transients_t(dbWrapper owner) :
	owner(owner),
	continueBtn(btnHuge(), { -0.95f, -0.95f + btnHuge().height * 1.25f * 0 }, "Continue",
		    guiButton::clickAction_F::make([](uxButton*){ /*TODO*/ })),
	loadBtn(btnHuge(), { -0.95f, -0.95f + btnHuge().height * 1.25f * 1 }, "Load",
		guiButton::clickAction_F::make([this](uxButton*){
		 this->deleteMe = true;
		 dbTypeFactory<loadGameMenu>(this->owner).construct();
	       })),
	newBtn(btnHuge(), { -0.95f, -0.95f + btnHuge().height * 1.25f * 2 }, "New",
	       guiButton::clickAction_F::make([this](uxButton*){
		 this->deleteMe = true;
		 dbTypeFactory<newGameMenu>(this->owner).construct();
	       })),
	settingsBtn(btnHuge(), { -0.95f, -0.95f + btnHuge().height * 1.25f * 3 }, "Settings",
		    guiButton::clickAction_F::make([this](uxButton*){
		      this->deleteMe = true;
		      dbTypeFactory<settingsMenu>(this->owner).construct();
		    })),
	exitBtn(btnHuge(), { -0.95f, -0.95f + btnHuge().height * 1.25f * 4 }, "Exit",
		guiButton::clickAction_F::make([](uxButton*){ WITE::requestShutdown(); })),
	buttons(&continueBtn, &loadBtn, &newBtn, &settingsBtn, &exitBtn) {};
    };

    static_assert(WITE::dbAllocationBatchSizeOf<mainMenu>::value == 1);
    static_assert(WITE::dbLogAllocationBatchSizeOf<mainMenu>::value == 1);

    transients_t* getTransients(uint64_t oid, void* dbv) {
      dbType<mainMenu> dbmm(oid, dbv);
      mainMenu mm;
      dbmm.readCurrent(&mm);//current because old pointers don't help
      return reinterpret_cast<transients_t*>(mm.transients);
    };

  }

  using namespace mainMenu_internals;//I know that's kinda lazy

  // void mainMenu::allocated(uint64_t oid, void* dbv) {
  // };

  // void mainMenu::freed(uint64_t oid, void* dbv) {
  // };

  static_assert(WITE::has_update<mainMenu>::value);
  void mainMenu::update(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    for(auto* btn : transients->buttons)
      btn->update();
    if(transients->deleteMe) [[unlikely]]
      dbType<mainMenu>(oid, dbv).destroy();
  };

  void mainMenu::spunUp(uint64_t oid, void* dbv) {
    dbType<mainMenu> dbmm(oid, dbv);
    mainMenu mm;
    dbmm.readCurrent(&mm);
    auto* transients = new transients_t(dbv);
    mm.transients = reinterpret_cast<void*>(transients);
    dbmm.write(&mm);
  };

  void mainMenu::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    delete transients;
  };

}

