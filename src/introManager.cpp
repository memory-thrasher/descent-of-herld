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
      dbWrapper owner;
      bool deleteMe = false;
      std::vector<guiLabel> labels;
      std::vector<guiButton> buttons;
      uint64_t stageStartTime = 0;
      transients_t(dbWrapper owner) : owner(owner) {};
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
    transients_t* transients = reinterpret_cast<transients_t*>(im.transients);
    uint64_t stageDelta = getNs() - transients->stageStartTime;
    constexpr uint64_t reprintThreshold = 1e10;//10 seconds
    bool reprint = stageDelta > reprintThreshold;
    if(reprint) {
      transients->labels.clear();
      transients->buttons.clear();
    }
#define printLine(str) transients->labels.emplace_back(textOnlyNormal(), glm::vec4(-0.95f, top += textOnlyNormal().text.charMetric.y * 1.05f, -0.05f, 0), str)
    float top = -0.95f;
    switch(im.stage) {
    case 0:
      if(reprint) {
	printLine("Procedural Core requests Heuristic Core startup.");
      }
      break;
    }
#undef printLine
    for(auto& button : transients->buttons)
      button.update();
    if(transients->deleteMe) [[unlikely]]
      dbType<introManager>(oid, dbv).destroy();
  };

  void introManager::spunUp(uint64_t oid, void* dbv) {
    dbType<introManager> dbmm(oid, dbv);
    introManager mm;
    dbmm.readCurrent(&mm);
    auto* transients = new transients_t(dbv);
    mm.transients = reinterpret_cast<void*>(transients);
    dbmm.write(&mm);
  };

  void introManager::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    delete transients;
  };

}

