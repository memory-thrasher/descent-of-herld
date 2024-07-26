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
    glm::vec4 normalBorder = getOptionColor("btn-border-color", { 0.5f, 0.35f, 0, 1 });
    glm::vec4 normalFill = getOptionColor("btn-fill-color", { 0.1f, 0.07f, 0, 1 });
    glm::vec4 normalTextColor = getOptionColor("btn-text-color", { 1, 1, 1, 1 });
    glm::vec4 hovBorder = getOptionColor("btn-hov-border-color", { 0.7f, 0.55f, 0, 1 });
    glm::vec4 hovFill = getOptionColor("btn-hov-fill-color", { 0.3f, 0.21f, 0, 1 });
    glm::vec4 hovTextColor = getOptionColor("btn-hov-text-color", { 1, 1, 1, 1 });
    glm::vec4 pressBorder = getOptionColor("btn-press-border-color", { 0.9f, 0.75f, 0, 1 });
    glm::vec4 pressFill = getOptionColor("btn-press-fill-color", { 0.5f, 0.4f, 0, 1 });
    glm::vec4 pressTextColor = getOptionColor("btn-press-text-color", { 1, 1, 1, 1 });
    {
      float clip = WITE::configuration::getOption("btn-huge-corner", 0.01f);//x&y relative to screen width
      float borderWidth = WITE::configuration::getOption("btn-huge-border-width", 0.001f);//x&y relative to screen width
      float textWidth = WITE::configuration::getOption("btn-huge-text-width", 0.01f);
      float textHeight = WITE::configuration::getOption("btn-huge-text-height", 0.01f);
      float width = WITE::configuration::getOption("btn-huge-width", 0.3f);
      float height = WITE::configuration::getOption("btn-huge-height", 0.1f);
      auto& btn = btnHuge();
      btn.rectNormal = { { clip, clip, clip, clip }, normalBorder,
			 { borderWidth, borderWidth, borderWidth, borderWidth }, normalFill };
      btn.rectHov = { { clip, clip, clip, clip }, hovBorder,
			 { borderWidth, borderWidth, borderWidth, borderWidth }, hovFill };
      btn.rectPress = { { clip, clip, clip, clip }, pressBorder,
			 { borderWidth, borderWidth, borderWidth, borderWidth }, pressFill };
      btn.textNormal = { normalTextColor, { textWidth, textHeight, clip, (textHeight - height)/2 } };
      btn.textHov = { hovTextColor, { textWidth, textHeight, clip, (textHeight - height)/2 } };
      btn.textPress = { pressTextColor, { textWidth, textHeight, clip, (textHeight - height)/2 } };
      btn.width = width;
      btn.height = height;
      btn.pushToBuffers();
    }
    //TODO other sizes
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

