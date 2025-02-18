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
#include "uxLabelVolatile.hpp"
#include "uxTextInput.hpp"
#include "uxGridLayout.hpp"
#include "input.hpp"
#include "uxTabbedView.hpp"

namespace doh {

  using namespace std::string_literals;

  namespace controllerMenu_internals {

    constexpr float deadzoneScale = 10;

    std::vector<control> pendingDelete;

    struct uxControl {
      uxLabelVolatile sysName, value;
      uxButtonVolatile deleteBtn;
      uxTextInput input;
      uxSlider deadzone;
      float displayValue = NAN, displayNormValue;
      uxControl() = delete;
      uxControl(const uxControl&) = delete;
      uxControl(controlConfiguration* ite) :
	sysName(textOnlyNormal(), getSysName(ite->id)),
	value(textOnlyNormal(), "ND"s),
	deleteBtn(btnNormal(), "Delete"s, [ite](uxButtonVolatile*) {
	  pendingDelete.emplace_back(ite->id);
	}),
	input(textInputNormal(), ite->label)
      {
	float maxDead = WITE::max(abs(ite->min), abs(ite->max)) * deadzoneScale;
	deadzone.setDomain({ maxDead, -1 });
	deadzone.value.x = ite->deadzone * deadzoneScale;
      };
    };

    struct uxController {
      //this controller's tab:
      uxPanel* panel = NULL;
      uxButtonVolatile* btn;
      uxLabelVolatile sysNameHeader, valueHeader, inputHeader, deleteHeader, deadzoneHeader;
      uxGridLayout layout;
      std::map<control, uxControl> controls {};
      //this controller's row in the controllers tab:
      uxLabelVolatile sysName;
      uxTextInput input;
      controller& c;
      uxController(controller& c, uxTab& tab) :
	sysNameHeader(textOnlyNormal(), "System Name"),
	valueHeader(textOnlyNormal(), "raw / adj"),
	inputHeader(textOnlyNormal(), "User-Defined Label (enter text, like 'trigger' or 'hat up')"),
	deleteHeader(textOnlyNormal(), "Delete"),
	deadzoneHeader(textOnlyNormal(), "Deadzone"),
	layout(btnNormal().height, {
	  textOnlyNormal().text.widthToFitChars(16),
	  textOnlyNormal().text.widthToFitChars(16),
	  btnNormal().textHov.widthToFitChars(6),
	  textInputNormal().textFocused.widthToFitChars(63),
	  0.25f
	}),
	//all controllers tab
	sysName(textOnlyNormal(), getSysName(c.id)),
	input(textInputNormal(), c.label),
	c(c)
      {
	input.maxLen = sizeof(controller::label);
	btn = &tab.btn;
	panel = &tab.panel;
	panel->setLayout(&layout);
	input.setVisible(true);
      };

      void redraw() {
	panel->clear();
	panel->push(&sysNameHeader);
	panel->push(&valueHeader);
	panel->push(&deleteHeader);
	panel->push(&inputHeader);
	panel->push(&deadzoneHeader);
	for(auto& pair : controls) {
	  panel->push(&pair.second.sysName);
	  panel->push(&pair.second.value);
	  panel->push(&pair.second.deleteBtn);
	  panel->push(&pair.second.input);
	  pair.second.input.setVisible(true);
	  pair.second.input.maxLen = sizeof(controlConfiguration::label) - 1;
	  panel->push(&pair.second.deadzone);
	  pair.second.deadzone.setVisible(true);
	}
	panel->redraw();
      };

      void update() {
	if(strcmp(input.content, c.label)) [[unlikely]] {
	  WITE::strcpy(c.label, input.content);
	  btn->labelStr = c.label;
	}
      };

    };

    struct transients_t {
      dbWrapper owner;
      guiButton exitBtn;
      //TODO some help info labels across the top
      uxTabbedView tabs;
      uxPanel* controllersPanel;
      uxGridLayout controllersLayout;
      std::map<controllerId, uxController> controllers;
      bool deleteMe = false;

      transients_t(dbWrapper owner) :
	owner(owner),
	exitBtn(btnNormal(), { -0.95f, -0.95f }, "Back",
		guiButton::clickAction_F::make([this](uxButton*){
		  deleteMe = true;
		  dbTypeFactory<settingsMenu>(this->owner).construct();
		})),
	tabs(btnNormal(), 32, { 0.005f, 0.03f }),
	controllersLayout(btnNormal().height, { textOnlyNormal().text.widthToFitChars(32), textInputNormal().textFocused.widthToFitChars(32) })
      {
	tabs.setBounds({ -0.95f, -0.95f + btnNormal().height * 3, 0.95f, 0.95f });
	controllersPanel = &tabs.emplaceTab("Controller Config").panel;
	controllersPanel->setLayout(&controllersLayout);
	update();
      };

      void update() {
	bool updated = !pendingDelete.empty();
	for(const control& c : pendingDelete) {
	  deleteControl(c);
	  controllers.at(c).controls.erase(c);
	}
	pendingDelete.clear();
	auto end = getControlEnd();
	auto it = getControlBegin();
	while(it != end) {
	  controlConfiguration* ite = getControl(it);
	  controllerId c = ite->id;
	  if(!controllers.contains(c)) [[unlikely]] {
	    if(!updated) [[unlikely]]
	      tabs.updateVisible(false);
	    updated = true;
	    controller& con = getController(c);
	    controllers.emplace(std::piecewise_construct, std::tie(c), std::tie(con, tabs.emplaceTab(con.label)));
	  }
	  uxController& uxc = controllers.at(c);
	  if(!uxc.controls.contains(ite->id)) [[unlikely]] {
	    if(!updated) [[unlikely]]
	      tabs.updateVisible(false);
	    updated = true;
	    // WARN(getSysName(ite->id), " = axis: ", static_cast<uint32_t>(ite->id.axisId), ", controlId: ", ite->id.controlId, ", controllerId: ", ite->id.controllerId, ", type: ", static_cast<uint32_t>(ite->id.type));
	    uxc.controls.emplace(std::piecewise_construct, std::make_tuple(ite->id), std::make_tuple(ite));
	    //controls will be pushed into panel by uxController::redraw below
	  }
	  auto& row = controllers.at(c).controls.at(ite->id);
	  controlValue value;
	  getControlValue(ite->id, value);
	  {//update config from control fields
	    if(std::strcmp(ite->label, row.input.content)) [[unlikely]]
	      WITE::strcpy(ite->label, row.input.content, sizeof(ite->label));
	    float maxDead = WITE::max(abs(ite->min), abs(ite->max)) * deadzoneScale;
	    if(row.deadzone.domain.x != maxDead) [[unlikely]]
	      row.deadzone.setDomain({ maxDead, -1 });
	    if(row.deadzone.value.x != ite->deadzone) [[unlikely]]
	      ite->deadzone = row.deadzone.value.x / deadzoneScale;
	  }
	  if(row.value.isVisible() && (row.displayValue != value.current ||
				       row.displayNormValue != value.normalizedValue)) [[unlikely]] {
	    row.value.setLabel(std::format("{:4.2f} / {:4.2f}", value.current, value.normalizedValue));
	    row.displayValue = value.current;
	    row.displayNormValue = value.normalizedValue;
	  }
	  ++it;
	}
	for(auto& pair : controllers)
	  pair.second.update();
	if(updated) [[unlikely]] {
	  controllersPanel->clear();
	  for(auto& pair : controllers) {
	    pair.second.redraw();
	    controllersPanel->push(&pair.second.sysName);
	    controllersPanel->push(&pair.second.input);
	  }
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
    transients->update();
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
