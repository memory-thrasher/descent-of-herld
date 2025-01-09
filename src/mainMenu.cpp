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
#include "../generated/spaceSkybox_stub.hpp"
#include "math.hpp"
#include "guiButton.hpp"
#include "fpsCounter.hpp"

namespace doh {

  constexpr glm::uvec3 focalSector { 204887, 20487, 2348 };

  static constexpr size_t saveSlots = 10;

  std::vector<std::unique_ptr<guiButton>> buttons;

  struct btn_t {
    std::string label;
    guiButton::clickAction action;
  };

  template<class T> void setButtons(uint64_t oid, void* dbv, const T& newButtons) {
    #error concurrent modification of buttons bc this is called from update of old button
    //TODO keep all buttons (in transient), disable most buttons
    buttons.clear();
    size_t i = 0;
    for(const btn_t& btn : newButtons) {
      buttons.emplace_back(std::make_unique<guiButton>(btnHuge(), glm::vec2{ -0.95f, -0.95f + btnHuge().height * 1.25f * i }, btn.label, btn.action, oid, dbv));
      i++;
    }
  };

  void gotoNewGame(guiButton* btn) {
    btn_t btns[saveSlots];
    for(size_t i = 0;i < saveSlots;i++)
      btns[i] = { std::string("Slot ") + std::to_string(i), guiButton::clickAction_F::make([i](guiButton*){
	WARN("clicked button ", i);
      }) };
    setButtons(btn->ownerId, btn->ownerDb, btns);
  };

  void gotoMain(uint64_t oid, void* dbv) {
    const btn_t btns[] {
      { "Continue", guiButton::clickAction_F::make([](guiButton*){ /*TODO*/ }) },
      { "Load", guiButton::clickAction_F::make([](guiButton*){ /*TODO*/ }) },
      { "New", guiButton::clickAction_F::make(gotoNewGame) },
      { "Settings", guiButton::clickAction_F::make([](guiButton*){ /*TODO*/ }) },
      { "Exit", guiButton::clickAction_F::make([](guiButton*){ WITE::requestShutdown(); }) },
    };
    setButtons(oid, dbv, btns);
  };

  void gotoMain(guiButton* btn) {
    gotoMain(btn->ownerId, btn->ownerDb);
  };

  struct transients_t {
    targetPrimary camera = targetPrimary::create();
    // nebula testNeb = nebula::create();
    // nebulaMap_t testNebMap;
    spaceSkybox space = spaceSkybox::create();
    fpsCounter fps;
    size_t viewId = 0;
  };

  static_assert(WITE::dbAllocationBatchSizeOf<mainMenu>::value == 1);
  static_assert(WITE::dbLogAllocationBatchSizeOf<mainMenu>::value == 1);

  transients_t* getTransients(uint64_t oid, void* dbv) {
    auto db = reinterpret_cast<db_t*>(dbv);
    mainMenu mm;
    db->readCurrent<mainMenu>(oid, &mm);//current because old pointers don't help
    return reinterpret_cast<transients_t*>(mm.transients);
  };

  void switchView(size_t viewId, guiButton*) {
    //?
  };

  void mainMenu::allocated(uint64_t oid, void* dbv) {
    auto db = reinterpret_cast<db_t*>(dbv);
    mainMenu mm;
    db->readCurrent<mainMenu>(oid, &mm);
    mm.cameraData = {
      { 0, 0, 0, 0 },
      { 10, 1000, 100000, 250 },//chunks, chunks, chunks, sectors
    };
    mm.cameraTrans = {
      { 0, 0, 0 },
      { 0, 0, 100000 },
      { 204887, 20487, 2348 },
    };
    mm.cameraTrans.rotate({ 0, 1, 0 }, -std::numbers::pi_v<float>/4);
    mm.fov = 1/std::tan(glm::radians(WITE::configuration::getOption("fov", 90.0f)/2));
    db->write(oid, &mm);
  };

  void mainMenu::freed(uint64_t oid, void* dbv) {
  };

  static_assert(WITE::has_update<mainMenu>::value);
  void mainMenu::update(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    const auto size = transients->camera.getWindow().getVecSize();
    for(auto& btn : buttons)
      btn->update();
    transients->fps.update();
    auto db = reinterpret_cast<db_t*>(dbv);
    mainMenu mm;
    db->readCommitted<mainMenu>(oid, &mm);
    mm.cameraData.geometry = { size, mm.fov*size.x/size.y, mm.fov };
    transients->camera.writeCameraData(mm.cameraData);
    mm.cameraTrans.rotate({ 1, 0, 0 }, -0.00002f);
    mm.cameraTrans.sector = focalSector;
    mm.cameraTrans.chunk = {};
    mm.cameraTrans.meters = {};
    mm.cameraTrans.moveSectors(mm.cameraTrans.orientation[2] * -150.0f);
    mm.cameraTrans.stabilize();
    compoundTransform_packed_t cameraTransPacked;
    mm.cameraTrans.pack(&cameraTransPacked);
    transients->camera.writeCameraTransform(cameraTransPacked);
    // transients->testNeb.mapCopySetEnabled(false);
    db->write(oid, &mm);
  };

  void mainMenu::spunUp(uint64_t oid, void* dbv) {
    auto db = reinterpret_cast<db_t*>(dbv);
    mainMenu mm;
    db->readCurrent<mainMenu>(oid, &mm);
    auto* transients = new transients_t();
    mm.transients = reinterpret_cast<void*>(transients);
    db->write<mainMenu>(oid, &mm);
    gotoMain(oid, dbv);
    // transients->testNeb.writeInstanceData(glm::vec4(focalSector, 2500));
    // generateNebula(focalSector, transients->testNebMap);
    // transients->testNeb.writeMap(transients->testNebMap);
    // transients->testNeb.mapCopySetEnabled(true);
  };

  void mainMenu::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    transients->camera.destroy();
    delete transients;
  };

}

