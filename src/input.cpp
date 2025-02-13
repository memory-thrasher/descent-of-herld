/*
  Copyright 2024-2025 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <memory>

#include "input.hpp"
#include "config.hpp"

namespace doh {

  struct inputConfigData {
    //only writable during inputUpdate, which is called by main when no other threads are doing anything, so no sync needed, hence "unsafe" functions are used
    inputConfigFile_t file;
    WITE::dbIndex<control> byControl;
    WITE::dbIndex<uint32_t> byAction;
    inputConfigData() : file(getDataDir() / "inputConfig.wdb", false), byControl(getDataDir() / "inputConfigByControl.wdb", false, decltype(byControl)::read_cb_F::make<inputConfigData>(this, &inputConfigData::derefControl)), byAction(getDataDir() / "inputConfigByAction.wdb", false, decltype(byAction)::read_cb_F::make<inputConfigData>(this, &inputConfigData::derefAction)) {};
    void derefControl(uint64_t eid, control& out) {
      out = file.deref_unsafe(eid).id;
    };
    void derefAction(uint64_t eid, uint32_t& out) {
      out = file.deref_unsafe(eid).action;
    };
  };

  std::unique_ptr<inputConfigData> config;

  void inputInit() {//called by main only
    WITE::winput::initInput();
    config = std::make_unique<inputConfigData>();
  };

  void inputUpdate() {//called by main only
    WITE::winput::pollInput();
    WITE::concurrentReadLock_read lock(&WITE::winput::allInputData_mutex);
    bool updated = false;
    //MAYBE change this to a manual operation in the settings menu, if this causes slowness
    for(auto& pair : WITE::winput::allInputData) {
      for(int i = 0;i < 3;i++) {
	auto& axis = pair.second.axes[i];
	if(axis.justChanged()) [[unlikely]] {
	  control c { pair.first, static_cast<uint8_t>(i) };
	  uint64_t eid = config->byControl.findAny(c);
	  if(eid == WITE::NONE) [[unlikely]] {
	    updated = true;
	    eid = config->file.allocate_unsafe();
	    controlConfiguration& cc = config->file.deref_unsafe(eid);
	    cc.id = c;
	    cc.action = 0;
	    cc.min = axis.min;
	    cc.max = axis.max;
	    config->byControl.insert(eid, c);
	  } else {
	    controlConfiguration& cc = config->file.deref_unsafe(eid);
	    if(axis.min < cc.min) [[unlikely]] cc.min = axis.min;
	    if(axis.max > cc.max) [[unlikely]] cc.max = axis.max;
	  }
	}
      }
    }
    if(updated) [[unlikely]]
      config->byControl.rebalance();//likely framerate drop here, but only happens the first time a control is used since the game was installed (or the config was wiped).
  };

  controlConfiguration* getControl(const control& c) {
    uint64_t eid = config->byControl.findAny(c);
    return eid == WITE::NONE ? NULL : &config->file.deref_unsafe(eid);
  };

  controlConfiguration* getControl(const inputConfigFile_t::iterator_t& it) {
    uint64_t eid = *it;
    return eid == WITE::NONE ? NULL : &config->file.deref_unsafe(eid);
  };

  controlConfiguration* getControl(uint32_t controlActionId) {
    uint64_t eid = config->byAction.findAny(controlActionId);
    return eid == WITE::NONE ? NULL : &config->file.deref_unsafe(eid);
  };

  void getControlValue(const control& c, controlValue& out) {
    controlConfiguration* cc = getControl(c);
    ASSERT_TRAP(cc != NULL, "Attempted to get value from control not found in db");
    WITE::winput::compositeInputData cid;
    WITE::winput::getInput(c, cid);
    float upperBandLowerBound = WITE::max(cc->deadzone, cc->min);
    float lowerBandUpperBound = WITE::min(-cc->deadzone, cc->max);
    out = { cid.axes[c.axisId], std::abs(out.current) < cc->deadzone ? 0 : out.current > 0 ? (out.current - upperBandLowerBound) / (cc->max - upperBandLowerBound) : (out.current - lowerBandUpperBound) / (lowerBandUpperBound - cc->min) };
  };

  inputConfigFile_t::iterator_t getControlBegin() {
    return config->file.begin();
  };

  inputConfigFile_t::iterator_t getControlEnd() {
    return config->file.end();
  };


}
