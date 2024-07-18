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
  if(nextDb.db)
    db.reset(nextDb.db.release());
  else
    db.reset();
};

void dbDestroyAll() {
  if(db) [[likely]] db.reset();
  if(nextDb.db) [[likely]] nextDb.db.reset();
};

void dbCycle() {
  if(!nextDb.db) [[likely]] return;
  if(db) [[likely]] dbDestroy();
  dbIsTemp = nextDb.isTemp;
};

void createMainMenu() {
  nextDb.db = std::make_unique<db_t>(std::filesystem::temp_directory_path() / "descent-of-herld-main-menu", true, true);
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

std::filesystem::path getSaveDirImpl();

std::filesystem::path getSaveDir() {
  static std::filesystem::path ret;
  if(ret.empty())
    ret = getSaveDirImpl();
  return ret;
};

#ifdef iswindows
#include <windows.h>
#include <shlobj.h>
std::filesystem::path getSaveDirImpl() {
  wchar_t*path;
  std::filesystem::path ret;
  //Fallback: (temp?) std::filesystem::temp_directory_path
  HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path); //Preferred: (user's Documents)
  if(SUCCEEDED(hr))
    ret = std::filesystem::path(path) / "My Games" / "Descent of Herld" / "saves";
  if(path)
    free(path);
  if(ret.empty()) {
    hr = SHGetKnownFolderPath(FOLDERID_AppDataDocuments, 0, NULL, &path); //Alt: (appdata Documents)
    if(SUCCEEDED(hr)) //Preferred: (user's Documents)
      ret = std::filesystem::path(path) / "Descent of Herld" / "saves";
  }
  if(path)
    free(path);
  if(ret.empty())
    ret = std::filesystem::temp_directory_path() / "Descent of Herld" / "saves";
  return ret;
};

#else //!iswindows
#include <unistd.h>
std::filesystem::path getSaveDirImpl() {
  std::filesystem::path ret;
  const char* path;
  path = getenv("XDG_DATA_HOME");
  if(!path) [[unlikely]]
    path = getenv("XDG_CONFIG_HOME");
  if(!path) [[unlikely]]
    path = getenv("HOME");
  ret = std::string(path);
  if(ret.empty()) [[unlikely]]
    ret = std::filesystem::temp_directory_path();
  ret = ret / "Descent of Herld" / "saves";
  return ret;
};
#endif //iswindows

