/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include <filesystem>
#include <chrono>

namespace doh {

  struct gameSlotInfo_t {
    bool exists, usable = true;
    std::string label, details;
    std::chrono::zoned_time<std::chrono::file_clock::duration> lastWrite;
  };

  void createMainMenu();
  void createNewGame(int slot);
  void dbUpdate();
  void dbEndFrame();
  void dbCycle();//destroys the current db only if a new one is waiting (from a prior create or load call).
  void dbDestroy();//not thread safe, only call when the db is not in use
  void dbDestroyAll();
  void dbLoadGame(int slot);
  std::filesystem::path getSaveDir();
  uint64_t getFrame();
  void getSlotInfo(size_t slotId, gameSlotInfo_t& out);

  struct dbWrapper {
    void* dbptr;
    dbWrapper(void* dbptr) : dbptr(dbptr) {};
    dbWrapper(const dbWrapper&) = default;
    uint64_t getFrame();
    inline operator void*() { return dbptr; };
    //more as needed
  };

  dbWrapper getDb();

}
