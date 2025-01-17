/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <set>

#include "windowPrimary.hpp"
#include "dbType.hpp"
#include "cameraStuff.hpp"
#include "uiStyle.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "../generated/spaceSkybox_stub.hpp"
#include "math.hpp"
#include "guiButton.hpp"
#include "fpsCounter.hpp"

namespace doh {

  namespace windowPrimary_internals {

    struct transients_t {
      targetPrimary camera = targetPrimary::create();
      // spaceSkybox space = spaceSkybox::create();
      fpsCounter fps;
    };

    transients_t* getTransients(uint64_t oid, void* dbv) {
      dbType<windowPrimary> dbmbg(oid, dbv);
      windowPrimary mbg;
      dbmbg.readCurrent(&mbg);//current because old pointers don't help
      return reinterpret_cast<transients_t*>(mbg.transients);
    };

  };

  using namespace windowPrimary_internals;

  void windowPrimary::allocated(uint64_t oid, void* dbv) {
    dbData<windowPrimary> data(oid, dbv);
    data.cameraData = {
      { 0, 0, 0, 0 },
      { 10, 1000, 100000, 250 },//chunks, chunks, chunks, sectors
    };
    data.cameraTrans = {
      { 0, 0, 0 },
      { 0, 0, 100000 },
      { 204887, 20487, 2348 },
    };
    data.cameraTrans.rotate({ 0, 1, 0 }, -std::numbers::pi_v<float>/4);
    data.fov = 1/std::tan(glm::radians(WITE::configuration::getOption("fov", 90.0f)/2));
    data.write();
  };

  // void windowPrimary::freed(uint64_t oid, void* dbv) {
  // };

  void windowPrimary::update(uint64_t oid, void* dbv) {
    dbData<windowPrimary> data(oid, dbv);
    transients_t* transients = reinterpret_cast<transients_t*>(data.transients);
    const auto size = transients->camera.getWindow().getVecSize();
    transients->fps.update();
    data.cameraData.geometry = { size, data.fov*size.x/size.y, data.fov };
    transients->camera.writeCameraData(data.cameraData);
    data.cameraTrans.rotate({ 1, 0, 0 }, -0.00002f);
    // data.cameraTrans.sector = focalSector;
    data.cameraTrans.chunk = {};
    data.cameraTrans.meters = {};
    data.cameraTrans.stabilize();
    compoundTransform_packed_t cameraTransPacked;
    data.cameraTrans.pack(&cameraTransPacked);
    transients->camera.writeCameraTransform(cameraTransPacked);
    // transients->testNeb.mapCopySetEnabled(false);
    data.write();
  };

  void windowPrimary::spunUp(uint64_t oid, void* dbv) {
    dbData<windowPrimary> data(oid, dbv);
    auto* transients = new transients_t();
    data.transients = reinterpret_cast<void*>(transients);
    data.write();
    // transients->testNeb.writeInstanceData(glm::vec4(focalSector, 2500));
    // generateNebula(focalSector, transients->testNebMap);
    // transients->testNeb.writeMap(transients->testNebMap);
    // transients->testNeb.mapCopySetEnabled(true);
  };

  void windowPrimary::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    transients->camera.destroy();
    delete transients;
  };

}

