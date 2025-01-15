/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include <WITE/WITE.hpp>

#include "dbWrapper.hpp"

namespace doh {

  struct dbTypeBase {
    uint64_t oid;
    dbWrapper db;
    dbTypeBase(uint64_t oid, dbWrapper db) : oid(oid), db(db) {};
    virtual ~dbTypeBase() = default;
    virtual void destroy() = 0;
    virtual bool read(uint64_t frameDelay, void* out) = 0;
    inline bool readCurrent(void* out) { return read(0, out); };
    inline bool readCommitted(void* out) { return read(1, out); };
    virtual void write(void* in) = 0;
    virtual WITE::syncLock* mutex() = 0;
  };

  struct dbTypeFactoryBase {
    dbWrapper db;
    dbTypeFactoryBase(void* db) : db(db) {};
    virtual ~dbTypeFactoryBase() = default;
    virtual dbTypeBase* create(void*) = 0;//caller responsible for destruction of returned (lightweight) obj
  };

  template<class A> struct dbType : public dbTypeBase {
    dbType(uint64_t oid, dbWrapper db) : dbTypeBase(oid, db) {};
    virtual ~dbType() = default;
    virtual void destroy() override;
    bool read(uint64_t frameDelay, A* out);
    inline bool readCurrent(A* out) { return read(0, out); };
    inline bool readCommitted(A* out) { return read(1, out); };
    virtual bool read(uint64_t frameDelay, void* out) override;
    void write(A* in);
    virtual void write(void* in) override;
    virtual WITE::syncLock* mutex() override;
  };

  template<class A> struct dbTypeFactory : public dbTypeFactoryBase {
    dbTypeFactory(void* db) : dbTypeFactoryBase(db) {};
    virtual ~dbTypeFactory() = default;
    virtual dbTypeBase* create(void*) override;//caller responsible for freeing returned (lightweight) obj
    dbType<A> createTyped(A*);//preferred
    template<class... Args> dbType<A> construct(Args... args) {
      A a(std::forward<Args>()...);
      return createTyped(&a);
    };
  };

  template<class A> struct dbRef {
    uint64_t oid;
    template<class... Args> dbType<A> spawn(dbWrapper db, Args... args) {
      return dbTypeFactory<A>(db)::construct(std::forward<Args>()...);
    };
  };

  template<class A> struct dbData : public A {
    dbWrapper db;
    uint64_t oid;
    dbData(const dbData& o) = default;
    dbData(uint64_t r, dbWrapper db, uint64_t latency = 1) : oid(r), db(db) {
      dbType<A>(r, db).read(latency, this);
    };
    dbData(const dbRef<A>& r, dbWrapper db, uint64_t latency = 1) : dbData(r.oid, db, latency) {};
    void write() {
      dbType<A>(oid, db).write(this);
    };
  };

}
