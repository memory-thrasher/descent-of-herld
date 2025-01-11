/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <WITE/WITE.hpp>

#include "dbWrapper.hpp"
#include "onionWrapper.hpp"
#include "config.hpp"

using namespace doh;

int main(int argc, const char** argv) {
  WITE::configuration::setOptions(argc, argv);
  loadConfig();
  WITE::gpu::init("Descent of Herld", {}, {}, {"VK_EXT_mesh_shader"});
  WITE::winput::initInput();
  WITE::wsound::initSound();
  loadDefaults();
  createOnionFull();
  createMainMenu();
  dbCycle();
  while(!WITE::shutdownRequested()) {
    WITE::winput::pollInput();
    dbUpdate();
    render();
    dbEndFrame();
    //TODO call onion between frame process
    dbCycle();
  }
  dbDestroyAll();
  onionDestroyAll();
}

