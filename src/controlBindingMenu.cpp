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

#include "controlBindingMenu.hpp"
#include "settingsMenu.hpp"
#include "dbType.hpp"
#include "uiStyle.hpp"
#include "guiButton.hpp"
#include "uxLabelVolatile.hpp"
#include "uxControlSelector.hpp"
#include "uxGridLayout.hpp"
#include "input.hpp"
#include "uxTabbedView.hpp"

namespace doh {

  using namespace std::string_literals;

  namespace controlBindingMenu_internals {

    struct uxAction {
      uint32_t actionId;
      uxLabelVolatile actionName, value;
      uxControlSelector selector;
      float displayNormValue = NAN;
      uxAction() = delete;
      uxAction(const uxAction&) = delete;
      uxAction(uint32_t actionId) :
	actionName(textOnlyNormal(), globalActionDetails[actionId].label),
	value(textOnlyNormal(), "ND"),
	selector(btnNormal(), getControlActionMapping(actionId))
      {};
    };

    struct uxActionCategory {
      uxPanel* panel = NULL;
      uxButtonVolatile* btn;
      uxGridLayout layout;
      std::map<uint32_t, uxAction> actions {};
      uxActionCategory(uxTab& tab) :
	layout(btnNormal().height, {
	  textOnlyNormal().text.widthToFitChars(32),
	  btnNormal().textHov.widthToFitChars(96),
	  textOnlyNormal().text.widthToFitChars(16)
	})
      {
	btn = &tab.btn;
	panel = &tab.panel;
	panel->setLayout(&layout);
      };

    };

    struct transients_t {
      dbWrapper owner;
      guiButton exitBtn;
      //TODO some help info labels across the top
      uxTabbedView tabs;
      std::map<controlActionCategory, uxActionCategory> categories;
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
	for(const controlActionDetails& cad : globalActionDetails) {
	  if(!categories.contains(cad.categoryId)) [[unlikely]]
	    categories.emplace(std::piecewise_construct,
			       std::tie(cad.categoryId),
			       std::tie(tabs.emplaceTab(actionCategoryToString(cad.categoryId))));
	  uxActionCategory& uac = categories.at(cad.categoryId);
	  uint32_t aid = static_cast<uint32_t>(cad.actionId);
	  uac.actions.emplace(std::piecewise_construct, std::make_tuple(aid), std::make_tuple(aid));
	  uxAction& ua = uac.actions.at(aid);
	  uac.panel->push(&ua.actionName);
	  uac.panel->push(&ua.selector);
	  uac.panel->push(&ua.value);
	}
	tabs.redraw();
	tabs.setVisible(true);
	tabs.updateVisible(true);
      };

    };

    transients_t* getTransients(uint64_t oid, void* dbv) {
      dbType<controlBindingMenu> dbmm(oid, dbv);
      controlBindingMenu mm;
      dbmm.readCurrent(&mm);//current because old pointers don't help
      return reinterpret_cast<transients_t*>(mm.transients);
    };

  }

  using namespace controlBindingMenu_internals;

  // void controlBindingMenu::allocated(uint64_t oid, void* dbv) {
  // };

  // void controlBindingMenu::freed(uint64_t oid, void* dbv) {
  // };

  void controlBindingMenu::update(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    transients->exitBtn.update();
    transients->tabs.update();
    if(transients->deleteMe) [[unlikely]]
      dbType<controlBindingMenu>(oid, dbv).destroy();
  };

  void controlBindingMenu::spunUp(uint64_t oid, void* dbv) {
    dbType<controlBindingMenu> dbmm(oid, dbv);
    controlBindingMenu mm;
    dbmm.readCurrent(&mm);
    auto* transients = new transients_t(dbv);
    mm.transients = reinterpret_cast<void*>(transients);
    dbmm.write(&mm);
  };

  void controlBindingMenu::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    delete transients;
  };

}
