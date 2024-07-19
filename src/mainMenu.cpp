/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "mainMenu.hpp"
#include "dbFull.hpp"

namespace doh {

  struct transients_t {
    onionFull_t::object_t<OL_primaryCamera.id>* camera;
    onionFull_t::object_t<OL_guiRect.id>* rect_test;
  };

  transients_t* getTransients(uint64_t oid, void* dbv) {
    auto db = reinterpret_cast<db_t*>(dbv);
    mainMenu mm;
    db->readCommitted<mainMenu>(oid, &mm);
    return reinterpret_cast<transients_t*>(mm.transients);
  };

  void mainMenu::allocated(uint64_t oid, void* dbv) {
    new(getTransients(oid, dbv))transients_t();
  };

  void mainMenu::freed(uint64_t oid, void* dbv) {
    delete getTransients(oid, dbv);
  };

  void mainMenu::update(uint64_t oid, void* dbv) {
    // getTransients(oid, dbv)
  };

  void mainMenu::spunUp(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    auto on = getOnionFull();
    transients->camera = on->create<OL_primaryCamera.id>();
    transients->rect_test = on->create<OL_guiRect.id>();
  };

  void mainMenu::spunDown(uint64_t oid, void* dbv) {
    transients_t* transients = getTransients(oid, dbv);
    auto on = getOnionFull();
    on->destroy(transients->camera);
    on->destroy(transients->rect_test);
  };

}

