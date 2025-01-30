
#pragma once
//!!include me

#include <WITE/WITE.hpp>

#include "gpuShared.hpp"

namespace doh {

  struct controllerMenu : WITE::db_singleton {
    static constexpr uint64_t typeId = 10001005;
    static constexpr std::string dbFileId = "controllerMenu";
    static void update(uint64_t oid, void* db);
    // static void allocated(uint64_t oid, void* db);
    // static void freed(uint64_t oid, void* db);
    static void spunUp(uint64_t oid, void* db);
    static void spunDown(uint64_t oid, void* db);
    void* transients;
  };
  //!!registerDbType controllerMenu

}
