/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <memory>

#include "dbWrapper.hpp"
#include "dbFull.hpp"
#include "config.hpp"

namespace doh {

  std::unique_ptr<db_t> db;
  bool dbIsTemp = false;
  struct {
    std::unique_ptr<db_t> db;
    bool isTemp;
  } nextDb;

  void dbDestroy() {
    if(!db) [[unlikely]] return;
    db->gracefulShutdown();
    if(dbIsTemp) db->deleteFiles();
    db.reset();
  };

  void dbDestroyAll() {
    if(db) [[likely]] {
      db->gracefulShutdown();
      if(dbIsTemp) db->deleteFiles();
      db.reset();
    }
    if(nextDb.db) [[unlikely]] {
      nextDb.db->gracefulShutdown();
      if(nextDb.isTemp) nextDb.db->deleteFiles();
      nextDb.db.reset();
    }
  };

  void dbCycle() {
    if(!nextDb.db) [[likely]] return;
    if(db) [[likely]] {
      db->gracefulShutdown();
      if(dbIsTemp) [[unlikely]] db->deleteFiles();
    }
    db.reset(nextDb.db.release());
    dbIsTemp = nextDb.isTemp;
    db->endFrame();//frame barrer between setup and first frame
  };

  void createMainMenu() {
    nextDb.db = std::make_unique<db_t>(getSaveDir() / "main-menu", true, true);
    nextDb.isTemp = true;
    mainMenu m;
    nextDb.db->create<mainMenu>(&m);
  };

  void createNewGame(int slot) {
    nextDb.db = std::make_unique<db_t>(getSaveDir() / std::to_string(slot), true, true);
    nextDb.isTemp = false;
    //TODO
  };

  void loadGame(int slot) {
    nextDb.db = std::make_unique<db_t>(getSaveDir() / std::to_string(slot), false, false);
    nextDb.isTemp = false;
  };

  void dbUpdate() {
    if(db) [[likely]]
      db->updateTick();
  };

  void dbEndFrame() {
    if(db) [[likely]]
      db->endFrame();
  };

  std::filesystem::path getSaveDir() {
    static std::filesystem::path ret;
    if(ret.empty())
      ret = getDataDir() / "saves";
    return ret;
  };

  uint64_t getFrame() {
    return db->getFrame();
  };

  dbWrapper getDb() {
    return { reinterpret_cast<void*>(db.get()) };
  };

  uint64_t dbWrapper::getFrame() {
    return reinterpret_cast<db_t*>(dbptr)->getFrame();
  };

}
