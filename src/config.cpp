/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <fstream>
#include <iostream>
#include <WITE/WITE.hpp>

#include "config.hpp"
#include "uiStyle.hpp"

namespace doh {

  std::filesystem::path getDataDirImpl();

  std::filesystem::path getDataDir() {
    static std::filesystem::path ret;
    if(ret.empty()) {
      const char* fromCli = WITE::configuration::getOption("dataDir");
      ret = fromCli == NULL ? getDataDirImpl() : std::filesystem::path(std::string(fromCli));
      std::cout << "Note: using data directory: " << ret << std::endl;
    }
    return ret;
  };

  void loadDefaults() {//both for populating global buffers and populating any missing arguments with nontrivial defaults
    int clip = WITE::configuration::getOption("buttonCornerClip", 25);
    int borderWidth = WITE::configuration::getOption("buttonBorderWidth", 5);
    {
      glm::vec4 border = getOptionColor("buttonColorBorder", { 0.5f, 0.35f, 0, 1 });
      glm::vec4 fill = getOptionColor("buttonColorFill", { 0.1f, 0.07f, 0, 1 });
      auto& btn = btnStyle();
      guiRectStyle_t styleData = { { clip, clip, clip, clip }, border,
				   { borderWidth, borderWidth, borderWidth, borderWidth }, fill };
      btn.slowOutOfBandSet(styleData);
    }
    {
      glm::vec4 border = getOptionColor("buttonHovColorBorder", { 0.7f, 0.55f, 0, 1 });
      glm::vec4 fill = getOptionColor("buttonHovColorFill", { 0.3f, 0.21f, 0, 1 });
      auto& btn = btnStyleHov();
      guiRectStyle_t styleData = { { clip, clip, clip, clip }, border,
				   { borderWidth, borderWidth, borderWidth, borderWidth }, fill };
      btn.slowOutOfBandSet(styleData);
    }
  };

  void loadConfig() {
    //The file is not an ini and to call it "text" is not wrong, and why not let it be editable with a double click?
    if(!std::filesystem::is_directory(getDataDir()))
      ASSERT_TRAP(std::filesystem::create_directories(getDataDir()), "failed to create data directory");
    const std::filesystem::path confPath = getDataDir() / "config.txt";
    if(std::filesystem::is_regular_file(confPath)) {//note: follows symlink
      LOG("note: loading config from: ", confPath);
      std::ifstream config { confPath };
      if(config.is_open()) {
	static constexpr size_t MAX_LINE_LENGTH = 1024;
	char lineTemp[MAX_LINE_LENGTH];
	while(!config.eof()) {
	  config.getline(lineTemp, MAX_LINE_LENGTH);
	  WITE::configuration::appendOption(lineTemp);
	  LOG("Config file option: ", lineTemp);
	}
      } else {
	WARN("failed to open config file, continuing with defaults");
      }
    } else {
      LOG("note: config file does not exist, creating: ", confPath);
    }
    loadDefaults();
    saveConfig();//apply any cli options or defaults
  };

  void saveConfig() {
    std::ofstream config { getDataDir() / "config.txt" };
    ASSERT_TRAP(config.is_open(), "failed to create config file");
    WITE::configuration::trimOptions();
    WITE::configuration::dumpOptions(config);
    config << std::flush;
  };

  glm::vec4 getOptionColor(const char* key, glm::vec4 def) {
    char* option = WITE::configuration::getOption(key);
    if(option) {
      uint32_t raw = std::stoul(option, NULL, 16);
      return glm::vec4(((raw >> (3*8)) & 0xFF) / 255.0f,
		       ((raw >> (2*8)) & 0xFF) / 255.0f,
		       ((raw >> (1*8)) & 0xFF) / 255.0f,
		       ((raw >> (0*8)) & 0xFF) / 255.0f);
    } else
      return def;
  };

#ifdef iswindows
#include <windows.h>
#include <shlobj.h>
  std::filesystem::path getDataDirImpl() {
    wchar_t*path;
    std::filesystem::path ret;
    //Fallback: (temp?) std::filesystem::temp_directory_path
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path); //Preferred: (user's Documents)
    if(SUCCEEDED(hr))
      ret = std::filesystem::path(path) / "My Games" / "DescentOfHerld";
    if(path)
      free(path);
    if(ret.empty()) {
      hr = SHGetKnownFolderPath(FOLDERID_AppDataDocuments, 0, NULL, &path); //Alt: (appdata Documents)
      if(SUCCEEDED(hr)) //Preferred: (user's Documents)
	ret = std::filesystem::path(path) / "DescentOfHerld";
    }
    if(path)
      free(path);
    if(ret.empty())
      ret = std::filesystem::temp_directory_path() / "DescentOfHerld";
    return ret;
  };

#else //!iswindows
#include <unistd.h>
  std::filesystem::path getDataDirImpl() {
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
    else
      ret /= ".config";
    ret = ret / "DescentOfHerld";
    return ret;
  };
#endif //iswindows

};

