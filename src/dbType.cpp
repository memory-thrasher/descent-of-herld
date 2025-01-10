/*
Copyright 2024 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "dbWrapper.hpp"
#include "dbType.hpp"
#include "dbFull.hpp"

namespace doh {

  template<class A> void dbType<A>::destroy() {
    reinterpret_cast<db_t*>(db.dbptr)->template destroy<A>(oid);
  };

  template<class A> bool dbType<A>::read(uint64_t frameDelay, A* out) {
    return reinterpret_cast<db_t*>(db.dbptr)->template read<A>(oid, frameDelay, out);
  };

  template<class A> bool dbType<A>::read(uint64_t frameDelay, void* out) {
    return read(frameDelay, reinterpret_cast<A*>(out));
  };

  template<class A> void dbType<A>::write(A* in) {
    reinterpret_cast<db_t*>(db.dbptr)->template write<A>(oid, in);
  };

  template<class A> void dbType<A>::write(void* in) {
    write(reinterpret_cast<A*>(in));
  };

  template<class A> WITE::syncLock* dbType<A>::mutex() {
    return reinterpret_cast<db_t*>(db.dbptr)->template mutexFor<A>(oid);
  };

  template<class A> dbTypeBase* dbTypeFactory<A>::create(void* data) {
    return reinterpret_cast<dbTypeBase*>(new dbType<A>(createTyped(reinterpret_cast<A*>(data))));
  };

  template<class A> dbType<A> dbTypeFactory<A>::createTyped(A* data) {
    return { reinterpret_cast<db_t*>(db.dbptr)->template create<A>(data), db };
  };

}

//a bunch of explicit instantiations like: template struct dbType<mainMenu>;
#include "../generated/dbTypeInstances.hpp"
