/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "gameManager.hpp"
#include "dbType.hpp"
#include "math.hpp"

namespace doh {

  namespace gameManager_internals {

    struct transients_t {
      dbWrapper owner;
      bool deleteMe = false;
      transients_t(dbWrapper owner) : owner(owner) {};
    };

    transients_t* getTransients(uint64_t oid, void* dbv) {
      dbType<gameManager> dbmm(oid, dbv);
      gameManager mm;
      dbmm.readCurrent(&mm);//current because old pointers don't help
      return reinterpret_cast<transients_t*>(mm.transients);
    };

  }

  using namespace gameManager_internals;

  void gameManager::allocated(uint64_t oid, void* dbv) {
    //new game: spawn everything
    dbData<gameManager> gm(oid, dbv);
    gm.intro.spawn(dbv);
    gm.write();
    //TODO spawn ship
    //TODO spawn sector
    //TODO spawn main window
  };

  // void gameManager::freed(uint64_t oid, void* dbv) {
  // };

  static_assert(WITE::has_update<gameManager>::value);
  void gameManager::update(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    if(transients->deleteMe) [[unlikely]]
      dbType<gameManager>(oid, dbv).destroy();
  };

  void gameManager::spunUp(uint64_t oid, void* dbv) {
    dbType<gameManager> dbmm(oid, dbv);
    gameManager mm;
    dbmm.readCurrent(&mm);
    auto* transients = new transients_t(dbv);
    mm.transients = reinterpret_cast<void*>(transients);
    dbmm.write(&mm);
  };

  void gameManager::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    delete transients;
  };

}

