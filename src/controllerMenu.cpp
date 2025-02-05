/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "controllerMenu.hpp"
#include "settingsMenu.hpp"
#include "dbType.hpp"
#include "uiStyle.hpp"
#include "guiButton.hpp"
#include "uxButton.hpp"
#include "uxLabel.hpp"
#include "uxTextInput.hpp"
#include "uxPanel.hpp"
#include "uxGridLayout.hpp"
#include "input.hpp"

namespace doh {

  namespace controllerMenu_internals {

    struct transients_t {
      dbWrapper owner;
      guiButton exitBtn;
      uxGridLayout layout;
      uxPanel panel;
      std::vector<std::unique_ptr<uxLabel>> testButtons;
      bool deleteMe = false;
      transients_t(dbWrapper owner) :
	owner(owner),
	exitBtn(btnNormal(), { -0.95f, -0.95f }, "Back",
		guiButton::clickAction_F::make([this](uxButton*){
		  deleteMe = true;
		  dbTypeFactory<settingsMenu>(this->owner).construct();
		})),
	layout(btnNormal().height, { btnNormal().width, btnNormal().width * 2, btnNormal().width }, { 0.005f, 0.03f }),
	panel()
      {
	panel.setBounds({ -0.95f, -0.95f + btnNormal().height * 3, 0.95f, 0.95f });
	panel.setLayout(&layout);
	for(size_t i = 0;i < 100;i++)
	  panel.push(testButtons.emplace_back(std::make_unique<uxLabel>(textOnlyNormal(), "test button")).get());
	panel.redraw();
	panel.setVisible(true);
	panel.updateVisible(true);
      };
    };

    transients_t* getTransients(uint64_t oid, void* dbv) {
      dbType<controllerMenu> dbmm(oid, dbv);
      controllerMenu mm;
      dbmm.readCurrent(&mm);//current because old pointers don't help
      return reinterpret_cast<transients_t*>(mm.transients);
    };

  }

  using namespace controllerMenu_internals;

  // void controllerMenu::allocated(uint64_t oid, void* dbv) {
  // };

  // void controllerMenu::freed(uint64_t oid, void* dbv) {
  // };

  void controllerMenu::update(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    transients->exitBtn.update();
    transients->panel.update();
    if(transients->deleteMe) [[unlikely]]
      dbType<controllerMenu>(oid, dbv).destroy();
  };

  void controllerMenu::spunUp(uint64_t oid, void* dbv) {
    dbType<controllerMenu> dbmm(oid, dbv);
    controllerMenu mm;
    dbmm.readCurrent(&mm);
    auto* transients = new transients_t(dbv);
    mm.transients = reinterpret_cast<void*>(transients);
    dbmm.write(&mm);
  };

  void controllerMenu::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    delete transients;
  };

}
