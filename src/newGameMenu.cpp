/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "newGameMenu.hpp"
#include "mainMenu.hpp"
#include "dbType.hpp"
#include "uiStyle.hpp"
#include "guiButton.hpp"
#include "guiLabel.hpp"

namespace doh {

  namespace newGameMenu_internals {

    constexpr size_t saveSlots = 10;

    struct transients_t {
      dbWrapper owner;
      std::vector<guiButton> buttons;
      std::vector<guiButton> tempButtons;
      std::vector<guiLabel> tempText;
      bool deleteMe = false;
      transients_t(dbWrapper owner) :
	owner(owner) {
	gameSlotInfo_t info;
	for(size_t i = 0;i < saveSlots;i++) {
	  getSlotInfo(i, info);
	  //TODO handle usable = false (replace button with label?)
	  buttons.emplace_back(btnNormal(), glm::vec2(-0.95f, -0.95f + btnNormal().height * 1.25f * i), info.label, guiButton::clickAction_F::make<transients_t, size_t, gameSlotInfo_t>(this, i, info, &transients_t::slotClicked));
	}
	buttons.emplace_back(btnNormal(), glm::vec2(-0.95f, -0.95f + btnNormal().height * 1.25f * (saveSlots + 1)), "Back",
			     guiButton::clickAction_F::make([this](uxButton*){
			       this->deleteMe = true;
			       dbTypeFactory<mainMenu>(this->owner).construct();
			     }));
      };

      void slotClicked(size_t slotId, gameSlotInfo_t info, uxButton*) {
	if(info.exists) {
	  tempButtons.clear();
	  tempText.clear();
	  float top = -0.95f, bottom = top + textOnlyBig().text.charMetric.y * 2;
	  tempText.emplace_back(textOnlyBig(), glm::vec4(0, top, 0.95f, bottom), "File Exists");
	  top = bottom += 0.005;
	  bottom += textOnlyNormal().text.charMetric.y * 4;
	  tempText.emplace_back(textOnlyNormal(), glm::vec4(0, top, 0.95f, bottom), "Using this slot will erase the previous game data.");
	  top = bottom += 0.005;
	  tempButtons.emplace_back(btnNormal(), glm::vec2(0, top), "Erase and Replace", guiButton::clickAction_F::make([slotId, this](uxButton*){
	    this->deleteMe = true;
	    createNewGame(slotId);
	  }));
	  tempButtons.emplace_back(btnNormal(), glm::vec2(0.5f, top), "Cancel", guiButton::clickAction_F::make([this](uxButton*){
	    this->tempButtons.clear();
	    this->tempText.clear();
	  }));
	} else {
	  this->deleteMe = true;
	  createNewGame(slotId);
	}
      };
    };

    transients_t* getTransients(uint64_t oid, void* dbv) {
      dbType<newGameMenu> dbmm(oid, dbv);
      newGameMenu mm;
      dbmm.readCurrent(&mm);//current because old pointers don't help
      return reinterpret_cast<transients_t*>(mm.transients);
    };

  }

  using namespace newGameMenu_internals;//I know that's kinda lazy

  // void newGameMenu::allocated(uint64_t oid, void* dbv) {
  // };

  // void newGameMenu::freed(uint64_t oid, void* dbv) {
  // };

  static_assert(WITE::has_update<newGameMenu>::value);
  void newGameMenu::update(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    for(guiButton& btn : transients->buttons)
      btn.update();
    for(guiButton& btn : transients->tempButtons)
      btn.update();
    if(transients->deleteMe) [[unlikely]]
      dbType<newGameMenu>(oid, dbv).destroy();
  };

  void newGameMenu::spunUp(uint64_t oid, void* dbv) {
    dbType<newGameMenu> dbmm(oid, dbv);
    newGameMenu mm;
    dbmm.readCurrent(&mm);
    auto* transients = new transients_t(dbv);
    mm.transients = reinterpret_cast<void*>(transients);
    dbmm.write(&mm);
  };

  void newGameMenu::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    delete transients;
  };

}

