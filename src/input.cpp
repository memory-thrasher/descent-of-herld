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
#include "uxInteractable.hpp"

namespace doh {

  struct inputConfigData {
    //only writable during inputUpdate, which is called by main when no other threads are doing anything, so no sync needed, hence "unsafe" functions are used
    inputConfigFile_t file;
    WITE::dbIndex<control> byControl;
    WITE::dbFile<controller, 4> controllers;//edited by controllerMenu
    WITE::dbFile<controlActionMapping, 256> actions;
    WITE::dbIndex<globalAction> actionsByActionId;
    inputConfigData() : file(getDataDir() / "controllerConfig.wdb", false),
			byControl(getDataDir() / "controllerConfigByControl.wdb", false, decltype(byControl)::read_cb_F::make<inputConfigData>(this, &inputConfigData::derefControl)),
			controllers(getDataDir() / "controllers.wdb", false),
			actions(getDataDir() / "controlBindings.wdb", false),
			actionsByActionId(getDataDir() / "controlBindingsByActionId.wdb", false, decltype(actionsByActionId)::read_cb_F::make<inputConfigData>(this, &inputConfigData::derefAction))
    {};
    void derefControl(uint64_t eid, control& out) {
      out = file.deref_unsafe(eid).id;
    };
    void derefAction(uint64_t eid, globalAction& out) {
      out = actions.deref_unsafe(eid).actionId;
    };
  };

  int32_t getSdlJoyIdxFromControllerId(uint64_t id) {
    int32_t joyCnt = SDL_NumJoysticks();
    for(int32_t i = 0;i < joyCnt;i++)
      if(WITE::winput::joyId(i) == id) [[unlikely]]
	return i;
    ASSERT_TRAP(false, "joy by id not found");
    return -1;
  };

  std::string getSysName(const controllerId& c) {
    using namespace std::string_literals;
    std::string ret;
    switch(c.type) {
    case WITE::winput::type_e::key: ret = "keyboard"s; break;
    case WITE::winput::type_e::mouse: ret = "mouse "s + std::to_string(c.id) + " axis"s; break;
    case WITE::winput::type_e::mouseButton: ret = "mouse "s + std::to_string(c.id) + " button"s; break;
    case WITE::winput::type_e::mouseWheel: ret = "mouse "s + std::to_string(c.id) + " wheel"s; break;
    case WITE::winput::type_e::joyButton:
      ret = std::format("{:.24s} button", SDL_JoystickNameForIndex(getSdlJoyIdxFromControllerId(c.id)));
      break;
    case WITE::winput::type_e::joyAxis:
      ret = std::format("{:.24s} axis", SDL_JoystickNameForIndex(getSdlJoyIdxFromControllerId(c.id)));
      break;
    }
    return ret;
  };

  std::unique_ptr<inputConfigData> config;

  void inputInit() {//called by main only
    WITE::winput::initInput();
    config = std::make_unique<inputConfigData>();
    //prepopulate common keyboard keys (avoid some symbols absent from many global layouts)
    control c { { WITE::winput::type_e::key, 0 }, 0 };
    for(uint32_t key = SDLK_0;key <= SDLK_z;key++) {
      c.controlId = key;
      uint64_t eid = config->byControl.findAny(c);
      if(eid == WITE::NONE) [[unlikely]] {
	eid = config->file.allocate_unsafe();
	controlConfiguration& cc = config->file.deref_unsafe(eid);
	cc.id = c;
	cc.min = 0;
	cc.max = 1;
	config->byControl.insert(eid, c);
      }
      if(key == SDLK_9) [[unlikely]] key = SDLK_a - 1;
    }
    config->byControl.rebalance();
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
    uxInteractable::globalUpdate();
  };

  controller& getController(const controllerId& c) {
    controller* ret = NULL;
    for(uint64_t eid : config->controllers) {//heap scan: how many controllers does anyone have? Also this is only used when displaying the name of the controller, during the tutorial or when editing in the settings menu
      controller& r = config->controllers.deref_unsafe(eid);
      if(c == r.id) [[unlikely]]
	ret = &r;
    }
    if(ret == NULL) [[unlikely]] {
      uint64_t eid = config->controllers.allocate_unsafe();
      controller& r = config->controllers.deref_unsafe(eid);
      r.id = c;
      std::string nom = getSysName(c);
      if(nom.size() >= sizeof(controller::label)) [[unlikely]]
	nom.resize(sizeof(controller::label) - 1);
      WITE::strcpy(r.label, nom.c_str());
      ret = &r;
    }
    return *ret;
  };

  void deleteControl(const control& c) {
    uint64_t eid = config->byControl.findAny(c);
    if(eid != WITE::NONE) [[likely]] {
      config->byControl.remove(c);
      config->file.free_unsafe(eid);
    }
  };

  controlConfiguration* getControl(const control& c) {
    uint64_t eid = config->byControl.findAny(c);
    return eid == WITE::NONE ? NULL : &config->file.deref_unsafe(eid);
  };

  controlConfiguration* getControl(const inputConfigFile_t::iterator_t& it) {
    uint64_t eid = *it;
    return eid == WITE::NONE ? NULL : &config->file.deref_unsafe(eid);
  };

  controlConfiguration* getControl(globalAction aid) {
    return getControl(getControlActionMapping(aid).controlId);//might be NULL
  };

  void getControlValue(globalAction aid, controlValue& out) {
    getControlValue(getControlActionMapping(aid).controlId, out);
  };

  void getControlValue(const control& c, controlValue& out) {
    if(c.axisId == control::nullAxis) [[unlikely]] {
      out = {};
      return;
    }
    controlConfiguration* cc = getControl(c);
    ASSERT_TRAP(cc != NULL, "Attempted to get value from control not found in db");
    WITE::winput::compositeInputData cid;
    WITE::winput::getInput(c, cid);
    if(cc->min != cc->min || cc->min > cid.axes[cc->id.axisId].min) [[unlikely]]
      cc->min = cid.axes[cc->id.axisId].min;
    if(cc->max != cc->max || cc->max < cid.axes[cc->id.axisId].max) [[unlikely]]
      cc->max = cid.axes[cc->id.axisId].max;
    float upperBandLowerBound = WITE::max(cc->deadzone, cc->min);
    float lowerBandUpperBound = WITE::min(-cc->deadzone, cc->max);
    out = { cid.axes[c.axisId], 0 };
    out.normalizedValue = std::abs(out.current) < cc->deadzone || cc->min == cc->max ? 0 : out.current >= 0 ? (out.current - upperBandLowerBound) / (cc->max - upperBandLowerBound) : (out.current - lowerBandUpperBound) / (lowerBandUpperBound - cc->min);
  };

  inputConfigFile_t::iterator_t getControlBegin() {
    return config->file.begin();
  };

  inputConfigFile_t::iterator_t getControlEnd() {
    return config->file.end();
  };

  std::string getSysName(const control& c) {
    using namespace std::string_literals;
    if(c.axisId == control::nullAxis) [[unlikely]]
      return "unassigned";
    switch(c.type) {
    case WITE::winput::type_e::mouse: return std::format("{} axis", static_cast<char>('X' + c.axisId));
    case WITE::winput::type_e::mouseButton:
      static constexpr std::string btnLut[4] = { "?", "left", "middle", "right" };
      return std::format("{} button", c.controlId <= 3 ? btnLut[c.controlId] : std::to_string(c.controlId));
    case WITE::winput::type_e::mouseWheel: return std::format("wheel {} axis", c.controllerId, static_cast<char>('X' + c.axisId));
    case WITE::winput::type_e::joyButton:
      static constexpr const char *const hatDirs[] = { "up", "down", "left", "right" };
      if(c.controlId >= 4096) [[unlikely]]
	return std::format("hat {} dir {}", (c.controlId - 4096) / 4, hatDirs[(c.controlId - 4096) % 4]);
      else
	return std::format("btn {}", c.controlId);
    case WITE::winput::type_e::joyAxis:
      return std::format("{} axis", c.controlId < 3 ? std::to_string(static_cast<char>('X' + c.controlId)) : std::to_string(c.controlId));
    case WITE::winput::type_e::key:
      std::string ret;
      if(c.controlId <= 'z' && c.controlId >= '!') [[likely]]
	ret = std::string(1, static_cast<char>(c.controlId));
      else
	switch(c.controlId) {
	default: return "keycode "s + std::format("{:X}", c.controlId);
	case SDLK_RETURN: ret = "return"s; break;
	case SDLK_ESCAPE: ret = "escape"s; break;
	case SDLK_BACKSPACE: ret = "backspace"s; break;
	case SDLK_TAB: ret = "tab"s; break;
	case SDLK_SPACE: ret = "space"s; break;
	case SDLK_CAPSLOCK: ret = "caps lock"s; break;
	case SDLK_F1: ret = "f1"s; break;
	case SDLK_F2: ret = "f2"s; break;
	case SDLK_F3: ret = "f3"s; break;
	case SDLK_F4: ret = "f4"s; break;
	case SDLK_F5: ret = "f5"s; break;
	case SDLK_F6: ret = "f6"s; break;
	case SDLK_F7: ret = "f7"s; break;
	case SDLK_F8: ret = "f8"s; break;
	case SDLK_F9: ret = "f9"s; break;
	case SDLK_F10: ret = "f10"s; break;
	case SDLK_F11: ret = "f11"s; break;
	case SDLK_F12: ret = "f12"s; break;
	case SDLK_PRINTSCREEN: ret = "print screen"s; break;
	case SDLK_SCROLLLOCK: ret = "scroll lock"s; break;
	case SDLK_PAUSE: ret = "pause"s; break;
	case SDLK_INSERT: ret = "insert"s; break;
	case SDLK_HOME: ret = "home"s; break;
	case SDLK_PAGEUP: ret = "page up"s; break;
	case SDLK_DELETE: ret = "delete"s; break;
	case SDLK_END: ret = "end"s; break;
	case SDLK_PAGEDOWN: ret = "page down"s; break;
	case SDLK_RIGHT: ret = "right"s; break;
	case SDLK_LEFT: ret = "left"s; break;
	case SDLK_DOWN: ret = "down"s; break;
	case SDLK_UP: ret = "up"s; break;
	case SDLK_NUMLOCKCLEAR: return "num lock"s;
	case SDLK_KP_DIVIDE: return "numpad /"s;
	case SDLK_KP_MULTIPLY: return "numpad *"s;
	case SDLK_KP_MINUS: return "numpad -"s;
	case SDLK_KP_PLUS: return "numpad +"s;
	case SDLK_KP_ENTER: return "numpad enter"s;
	case SDLK_KP_1: return "numpad 1"s;
	case SDLK_KP_2: return "numpad 2"s;
	case SDLK_KP_3: return "numpad 3"s;
	case SDLK_KP_4: return "numpad 4"s;
	case SDLK_KP_5: return "numpad 5"s;
	case SDLK_KP_6: return "numpad 6"s;
	case SDLK_KP_7: return "numpad 7"s;
	case SDLK_KP_8: return "numpad 8"s;
	case SDLK_KP_9: return "numpad 9"s;
	case SDLK_KP_0: return "numpad 0"s;
	case SDLK_KP_PERIOD: return "numpad ."s;
	case SDLK_LCTRL: ret = "L ctrl"; break;
	case SDLK_LSHIFT: ret = "L shift"; break;
	case SDLK_LALT: ret = "L alt"; break;
	case SDLK_LGUI: ret = "L gui"; break;
	case SDLK_RCTRL: ret = "R ctrl"; break;
	case SDLK_RSHIFT: ret = "R shift"; break;
	case SDLK_RALT: ret = "R alt"; break;
	case SDLK_RGUI: ret = "R gui"; break;
	case SDLK_MODE: ret = "mode"; break;
	}
      return ret + " key"s;
    };
  };

  controlActionMapping& getControlActionMapping(globalAction actionId) {
    uint64_t eid = config->actionsByActionId.findAny(actionId);
    controlActionMapping*ret;
    if(eid == WITE::NONE) [[unlikely]] {
      eid = config->actions.allocate_unsafe();
      config->actionsByActionId.insert(eid, actionId);
      ret = &config->actions.deref_unsafe(eid);
      ret->actionId = actionId;
      ret->controlId.axisId = control::nullAxis;
    } else {
      ret = &config->actions.deref_unsafe(eid);
    }
    return *ret;
  };

  std::string actionCategoryToString(controlActionCategory actionCategory) {
    using namespace std::string_literals;
    switch(actionCategory) {
    case controlActionCategory::menuNavigation: return "Menu Navigation"s;
    case controlActionCategory::schematic: return "Schematic"s;
    case controlActionCategory::flightDefaults: return "Flight Defaults"s;
    }
  };

}
