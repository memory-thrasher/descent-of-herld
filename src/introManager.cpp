/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <vector>

#include "introManager.hpp"
#include "guiButton.hpp"
#include "guiLabel.hpp"
#include "dbType.hpp"
#include "math.hpp"
#include "time.hpp"

namespace doh {

  namespace introManager_internals {

    struct transients_t {
      uint64_t oid;
      dbWrapper owner;
      bool deleteMe = false;
      std::vector<guiLabel> labels;
      std::vector<guiButton> buttons;
      uint64_t stageStartTime = 0;
      guiButton::clickAction advanceStage;

      void advanceStage_impl(uxButton*) {
	dbData<introManager> im(oid, owner);
	im.stage++;
	im.write();
	updateStage(im);
      };

      void setStage(uint64_t stage, uxButton*) {
	dbData<introManager> im(oid, owner);
	im.stage = stage;
	im.write();
	updateStage(im);
      };

      transients_t(uint64_t oid, dbWrapper owner) : oid(oid), owner(owner), advanceStage(guiButton::clickAction_F::make<transients_t>(this, &transients_t::advanceStage_impl)) {};

      void updateStage(const dbData<introManager>& im) {
	labels.clear();
	buttons.clear();
	buttons.emplace_back(btnNormal(), glm::vec2(0.95f-btnNormal().width, -0.95f), "Restart", [this](uxButton*) {
	  dbData<introManager> im(oid, owner);
	  im.stage = 0;
	  im.write();
	  updateStage(im);
	  //TODO hide anything that already got shown
	});
	buttons.emplace_back(btnNormal(), glm::vec2(0.95f-btnNormal().width, -0.95f + btnNormal().height * 1.25f), "Exit", [](uxButton*) {
	  createMainMenu();
	});
	float lineIncrement = textOnlyNormal().text.charMetric.y * 1.05f;
#define printLine(str) labels.emplace_back(textOnlyNormal(), glm::vec4(-0.95f, top, -0.05f, -1), str); \
	top += lineIncrement
#define printLineRight(str) labels.emplace_back(textOnlyNormal(), glm::vec2(0, top), str); \
	top += lineIncrement
	float top = -0.95f;
	switch(im.stage) {
	case 0:
	  printLine("Procedural Core requests Heuristic Core startup.");
	  buttons.emplace_back(btnNormal(), glm::vec2(0, top), "Startup.", advanceStage);
	  break;
	case 1:
	  printLine("Procedural Core requests Heuristic Core startup.");
	  printLineRight("Startup.");
	  printLine("Heuristic Core, provide status.");
	  buttons.emplace_back(btnNormal(), glm::vec2(0, top), "Online.", advanceStage);
	  break;
	case 2:
	  printLine("Procedural Core requests Heuristic Core startup.");
	  printLineRight("Startup.");
	  printLine("Heuristic Core, provide status.");
	  printLineRight("Online.");
	  printLine("Heuristic Core, provide version.");
	  buttons.emplace_back(btnNormal(), glm::vec2(0, top), "v0.3a.", advanceStage);
	  break;
	case 3:
	  printLine("Procedural Core requests Heuristic Core startup.");
	  printLineRight("Startup.");
	  printLine("Heuristic Core, provide status.");
	  printLineRight("Online.");
	  printLine("Heuristic Core, provide version.");
	  printLineRight("v0.3a.");
	  printLine("Heuristic Core, run self-diagnostic.");
	  buttons.emplace_back(btnNormal(), glm::vec2(0, top), "Diagnostic completed. 113 Errors found.", advanceStage);
	  break;
	case 4:
	  printLine("Procedural Core requests Heuristic Core startup.");
	  printLineRight("Startup.");
	  printLine("Heuristic Core, provide status.");
	  printLineRight("Online.");
	  printLine("Heuristic Core, provide version.");
	  printLineRight("v0.3a.");
	  printLine("Heuristic Core, run self-diagnostic.");
	  printLineRight("Diagnostic completed. 113 Errors found.");
	  printLine("Is debug function available? (enable tutorial?)");
	  buttons.emplace_back(btnNormal(), glm::vec2(0, top), "Connect debugger. (enables tutorial)", advanceStage);
	  top += lineIncrement * 2;
	  buttons.emplace_back(btnNormal(), glm::vec2(0, top), "Debugging unavailable in production environment. (disables tutorial)", guiButton::clickAction_F::make<transients_t, uint64_t>(this, 999, &transients_t::setStage));
	  break;
	case 5:
	  printLine("Procedural Core requests Heuristic Core startup.");
	  printLineRight("Startup.");
	  printLine("Heuristic Core, provide status.");
	  printLineRight("Online.");
	  printLine("Heuristic Core, provide version.");
	  printLineRight("v0.3a.");
	  printLine("Heuristic Core, run self-diagnostic.");
	  printLineRight("Diagnostic completed. 113 Errors found.");
	  printLine("Is debug function available? (enable tutorial?)");
	  printLineRight("Connect debugger. (enables tutorial)");
	  printLine("Test internal sensors");
	  buttons.emplace_back(btnNormal(), glm::vec2(0, top), "Internal sensors online", advanceStage);
	  break;
	case 6:
	  printLine("(Use right-click-and-drag or controller to orbit the ship)");
	  buttons.emplace_back(btnNormal(), glm::vec2(0, top), "(Next)", advanceStage);
	  //TODO show ship
	  break;
	default://for development
	  printLine("TODO finish intro (stage not found) " + std::to_string(im.stage));
	  break;
	}
#undef printLine
      };

    };

    transients_t* getTransients(uint64_t oid, void* dbv) {
      dbType<introManager> dbmm(oid, dbv);
      introManager mm;
      dbmm.readCurrent(&mm);//current because old pointers don't help
      return reinterpret_cast<transients_t*>(mm.transients);
    };

  }

  using namespace introManager_internals;

  // void introManager::allocated(uint64_t oid, void* dbv) {
  // };

  // void introManager::freed(uint64_t oid, void* dbv) {
  // };

  static_assert(WITE::has_update<introManager>::value);
  void introManager::update(uint64_t oid, void* dbv) {
    dbData<introManager> im(oid, dbv);
    transients_t* transients = getTransients(oid, dbv);
    for(auto& button : transients->buttons)
      button.update();
    if(transients->deleteMe) [[unlikely]]
      dbType<introManager>(oid, dbv).destroy();
  };

  void introManager::spunUp(uint64_t oid, void* dbv) {
    dbData<introManager> im(oid, dbv);
    auto* transients = new transients_t(oid, dbv);
    im.transients = reinterpret_cast<void*>(transients);
    im.write();
    transients->updateStage(im);
  };

  void introManager::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    delete transients;
  };

}

