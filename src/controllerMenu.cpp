/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <map>
#include <string>

#include "controllerMenu.hpp"
#include "settingsMenu.hpp"
#include "dbType.hpp"
#include "uiStyle.hpp"
#include "guiButton.hpp"
// #include "uxButtonVolatile.hpp"
#include "uxLabelVolatile.hpp"
#include "uxTextInput.hpp"
// #include "uxPanel.hpp"
#include "uxGridLayout.hpp"
#include "input.hpp"
#include "uxTabbedView.hpp"

namespace doh {

  using namespace std::string_literals;

  namespace controllerMenu_internals {

    struct controller {
      WITE::winput::type_e type;
      uint32_t id;
      controller(const WITE::winput::inputIdentifier& i) : type(i.type), id(i.controllerId) {};
      auto operator<=>(const controller& o) const = default;
    };

    std::string to_string(const controller& c) {
      std::string ret;
      switch(c.type) {
      case WITE::winput::type_e::key: ret = "keyboard"s; break;
      case WITE::winput::type_e::mouse: ret = "mouse "s + std::to_string(c.id) + " axis"s; break;
      case WITE::winput::type_e::mouseButton: ret = "mouse "s + std::to_string(c.id) + " button"s; break;
      case WITE::winput::type_e::mouseWheel: ret = "mouse "s + std::to_string(c.id) + " wheel"s; break;
      case WITE::winput::type_e::joyButton: ret = "joy "s + std::to_string(c.id) + " button"s; break;
      case WITE::winput::type_e::joyAxis: ret = "joy "s + std::to_string(c.id) + " axis"s; break;
      }
      return ret;
    };

    struct uxControl {
      uxLabelVolatile sysName, value;
      uxTextInput input;//TODO change listener? Apply button?
      //TODO deadzone?
      uxControl() = delete;
      uxControl(const uxControl&) = delete;
      uxControl(std::string sysName, std::string userLabel) : sysName(textOnlyNormal(), sysName), value(textOnlyNormal(), "ND"s), input(textInputNormal(), userLabel.c_str()) {};
    };

    struct uxController {
      uxPanel* panel = NULL;
      uxGridLayout layout { btnNormal().height, { textOnlyNormal().text.widthToFitChars(16), textOnlyNormal().text.widthToFitChars(4), textInputNormal().textFocused.widthToFitChars(96) } };
      std::map<control, uxControl> controls {};
      void redraw() {
	panel->clear();
	for(auto& pair : controls) {
	  panel->push(&pair.second.sysName);
	  panel->push(&pair.second.value);
	  panel->push(&pair.second.input);
	}
	panel->redraw();
      };
    };

    struct transients_t {
      dbWrapper owner;
      guiButton exitBtn;
      uxTabbedView tabs;
      std::map<controller, uxController> controllers;
      bool deleteMe = false;
      transients_t(dbWrapper owner) :
	owner(owner),
	exitBtn(btnNormal(), { -0.95f, -0.95f }, "Back",
		guiButton::clickAction_F::make([this](uxButton*){
		  deleteMe = true;
		  dbTypeFactory<settingsMenu>(this->owner).construct();
		})),
	tabs(btnNormal(), 32, { 0.005f, 0.03f })
      {
	tabs.setBounds({ -0.95f, -0.95f + btnNormal().height * 3, 0.95f, 0.95f });
	update();
      };
      void update() {
	bool updated = false;
	auto end = getControlEnd();
	auto it = getControlBegin();
	while(it != end) {
	  controlConfiguration* ite = getControl(it);
	  controller c = ite->id;
	  auto& uxc = controllers[c];//possible implicit creation
	  if(uxc.panel == NULL) [[unlikely]] {
	    if(!updated) [[unlikely]]
	      tabs.updateVisible(false);
	    updated = true;
	    uxc.panel = &tabs.emplaceTab(to_string(c));
	    uxc.panel->setLayout(&uxc.layout);
	  }
	  if(!uxc.controls.contains(ite->id)) [[unlikely]] {
	    if(!updated) [[unlikely]]
	      tabs.updateVisible(false);
	    updated = true;
	    uxc.controls.emplace(std::piecewise_construct, std::make_tuple(ite->id), std::make_tuple(std::to_string(ite->id.controlId) + "_"s + std::to_string(ite->id.axisId), ""s));
	    //controls will be pushed into panel by uxController::redraw below
	  }
	  ++it;
	}
	if(updated) [[unlikely]] {
	  for(auto& pair : controllers)
	    pair.second.redraw();
	  tabs.redraw();
	  tabs.setVisible(true);
	  tabs.updateVisible(true);
	}
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
    transients->tabs.update();
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
