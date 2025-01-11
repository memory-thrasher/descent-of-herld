#!/bin/bash


# Copyright 2024 Wafflecat Games, LLC

# This file is part of The Descent of Herld.

# The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

# The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

# Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.



# this script will scan all source files and generate a header that contains define snippets that help to create the onion with less touching of so many files every time an object gets modified or added

#note: working directory is project root (executed from ../build.sh)
allvars=
dbTypes=
echo > generated/allStubs.hpp_new
echo '#include "allStubs.hpp"' > generated/allImpl.hpp_new
echo '#include <set>' > generated/allStubs.hpp_new
echo '#include <WITE/WITE.hpp>' >> generated/allStubs.hpp_new
while IFS= read -d $'\0' file; do
    objId=
    fileRaw=$(basename "${file%.*}")
    genStub="generated/${fileRaw}_stub.hpp"
    genImpl="generated/${fileRaw}_impl.hpp"
    genStubNew="generated/${fileRaw}_stub.hpp_new"
    genImplNew="generated/${fileRaw}_impl.hpp_new"
    hasGlobalCollection=false
    singletons=
    echo "#include \"../${file}\"" > $genStubNew
    echo "#pragma once" >> $genStubNew
    echo 'namespace doh {' >> $genStubNew
    echo "  struct ${fileRaw} {" >> $genStubNew
    echo "    void* onionObj;" >> $genStubNew
    echo 'namespace doh {' > $genImplNew
    while read line; do
	if [[ "$line" =~ \!\!onion\ (.*?) ]]; then
	    onion="${BASH_REMATCH[1]}"
	    #note: this doesn't do anything (yet) as I have only one onion
	elif [[ "$line" =~ \!\!append\ (.*?)\ (.*?) ]]; then
	    listName="${BASH_REMATCH[1]}"
	    listNewVar="${BASH_REMATCH[2]}"
	    if ! echo "$allvars" | grep -Fwq "$listName"; then
		allvars="$listName $allvars"
		eval "$listName='$listNewVar'"
	    else
		eval "$listName=\"\$$listName, $listNewVar\""
	    fi
	elif [[ "$line" =~ \!\!registerDbType\ (.*?) ]]; then
	    typeName="${BASH_REMATCH[1]}"
	    #note: possible duplicate includes if one header contains multiple types, theoretically harmless
	    echo "#include \"../${file}\"" >>generated/dbTypeInstances.hpp_new
	    echo "#include \"../${file}\"" >>generated/dbHelper.hpp_new
	    echo "template struct doh::dbType<doh::$typeName>;" >>generated/dbTypeInstances.hpp_new
	    echo "template struct doh::dbTypeFactory<doh::$typeName>;" >>generated/dbTypeInstances.hpp_new
	    if [ -z "$dbTypes" ]; then
		dbTypes="$typeName"
	    else
		dbTypes="$dbTypes, $typeName"
	    fi
	elif [[ "$line" =~ \!\!genObj\ (.*?) ]]; then
	    objId="${BASH_REMATCH[1]}.id" #actual factory generated later
	    echo "#define castOnionObj reinterpret_cast<onionFull_t::object_t<$objId>*>(onionObj)" >> $genImplNew
	elif [[ "$line" =~ \!\!genObjGlobalCollection ]]; then
	    hasGlobalCollection=true
	elif [[ "$line" =~ \!\!genObjGetWindow ]]; then
	    echo "    WITE::window& getWindow() const;" >> $genStubNew
	    echo "  WITE::window& ${fileRaw}::getWindow() const {" >> $genImplNew
	    echo "    return castOnionObj->getWindow();" >> $genImplNew
	    echo "  };" >> $genImplNew
	elif [[ "$line" =~ \!\!genObjWrite\ (.*?)\ (.*?)\ (.*?) ]]; then
	    rs="${BASH_REMATCH[1]}"
	    fnName="${BASH_REMATCH[2]}"
	    dataName="${BASH_REMATCH[3]}"
	    echo "    void ${fnName}(const ${dataName}& data);" >> $genStubNew
	    echo "  void ${fileRaw}::${fnName}(const ${dataName}& data) {" >> $genImplNew
	    echo "    castOnionObj->template write<${rs}.id>(data);" >> $genImplNew
	    echo "  };" >> $genImplNew
	elif [[ "$line" =~ \!\!genObjSet\ (.*?)\ (.*?)\ (.*?) ]]; then
	    rs="${BASH_REMATCH[1]}"
	    fnName="${BASH_REMATCH[2]}"
	    dataName="${BASH_REMATCH[3]}"
	    echo "    void ${fnName}(${dataName}& data);" >> $genStubNew
	    echo "  void ${fileRaw}::${fnName}(${dataName}& data) {" >> $genImplNew
	    echo "    castOnionObj->template set<${rs}.id>(&data);" >> $genImplNew
	    echo "  };" >> $genImplNew
	elif [[ "$line" =~ \!\!genObjSingleton\ (.*?)\ (.*?) ]]; then
	    rs="${BASH_REMATCH[1]}"
	    valueName="${BASH_REMATCH[2]}"
	    singletons="    handle->template set<${rs}.id>($valueName);"$'\n'
	elif [[ "$line" =~ \!\!genObjSlowWrite\ (.*?)\ (.*?)\ (.*?) ]]; then
	    rs="${BASH_REMATCH[1]}"
	    fnName="${BASH_REMATCH[2]}"
	    dataName="${BASH_REMATCH[3]}"
	    echo "    void ${fnName}(const ${dataName}& data);" >> $genStubNew
	    echo "  void ${fileRaw}::${fnName}(const ${dataName}& data) {" >> $genImplNew
	    echo "    castOnionObj->template get<${rs}.id>().template slowOutOfBandSet<${dataName}>(data);" >> $genImplNew
	    echo "  };" >> $genImplNew
	elif [[ "$line" =~ \!\!genObjStepControl\ (.*?)\ (.*?) ]]; then
	    stepId="${BASH_REMATCH[1]}"
	    fnName="${BASH_REMATCH[2]}"
	    echo "    void ${fnName}(bool data);" >> $genStubNew
	    echo "  void ${fileRaw}::${fnName}(bool data) {" >> $genImplNew
	    echo "    castOnionObj->template stepEnabled<${stepId}>() = data;" >> $genImplNew
	    echo "  };" >> $genImplNew
	elif [[ "$line" =~ \!\!include\ me ]]; then
	    echo "#include \"../$file\""
	else
	    echo "Unrecognized directive: $line"
	    exit 1;
	fi
    done < <(grep -ho '!!.*' "$file")
    if [ -n "$objId" ]; then
	if $hasGlobalCollection; then
	    echo "    static std::set<${fileRaw}> allInstances;" >> $genStubNew
	    echo "    static WITE::syncLock allInstances_mutex;" >> $genStubNew
	    echo "  std::set<${fileRaw}> ${fileRaw}::allInstances;" >> $genImplNew
	    echo "  WITE::syncLock ${fileRaw}::allInstances_mutex;" >> $genImplNew
	fi
	echo "    auto operator<=>(const ${fileRaw}& o) const = default;" >> $genStubNew
	echo "    void destroy();" >> $genStubNew
	echo "    static ${fileRaw} create();" >> $genStubNew
	echo '  };' >> $genStubNew #end struct
	echo '}' >> $genStubNew #end namespace
	echo "  void ${fileRaw}::destroy() {" >> $genImplNew
	if $hasGlobalCollection; then
	    echo "    WITE::scopeLock lock(&allInstances_mutex);" >> $genImplNew
	    echo "    allInstances.erase(allInstances.find(*this));" >> $genImplNew
	fi
	echo "    getOnionFull()->destroy(castOnionObj);" >> $genImplNew
	echo '  };' >> $genImplNew
	echo "  ${fileRaw} ${fileRaw}::create() { //static" >> $genImplNew
	echo "    ${fileRaw} ret { reinterpret_cast<void*>(getOnionFull()->template create<${objId}>()) };" >> $genImplNew
	if $hasGlobalCollection; then
	    echo "    WITE::scopeLock lock(&allInstances_mutex);" >> $genImplNew
	    echo "    allInstances.insert(ret);" >> $genImplNew
	fi
	if [ -n "$singletons" ]; then
	    echo "    auto handle = reinterpret_cast<onionFull_t::object_t<$objId>*>(ret.onionObj);" >> $genImplNew
	    echo -n "$singletons" >> $genImplNew
	fi
	echo "    return ret;" >> $genImplNew
	echo '  };' >> $genImplNew
	echo '}' >> $genImplNew #end namespace
	echo "#undef castOnionObj" >> $genImplNew
	if ! test -f $genStub || diff -q $genStub $genStubNew | grep -q .; then
	    mv $genStubNew $genStub
	else
	    rm $genStubNew
	fi
	if ! test -f $genImpl || diff -q $genImpl $genImplNew | grep -q .; then
	    mv $genImplNew $genImpl
	else
	    rm $genImplNew
	fi
	echo "#include \"../$genStub\"" >> generated/allStubs.hpp_new
	echo "#include \"../$genImpl\"" >> generated/allImpl.hpp_new
    else
	rm $genStubNew $genImplNew
    fi
done < <(find src -type f -not -iname '*~' -print0) >generated/onionHelper.hpp_new

while read -d' ' var; do
    echo -n "#define expand_$var(T) constexpr auto $var = WITE::concat<T, "
    eval echo -n \$$var
    echo '>();'
    echo -n "#define expand_raw_$var "
    eval echo \$$var
done <<<$allvars >>generated/onionHelper.hpp_new

if [ -n "$dbTypes" ]; then
    # echo '#include "dbHelper.hpp"' >>generated/allStubs.hpp_new
    echo "#define dbTypes $dbTypes" >>generated/dbHelper.hpp_new
fi

if ! test -f generated/dbHelper.hpp || diff -q generated/dbHelper.hpp_new generated/dbHelper.hpp | grep -q .; then
    mv generated/dbHelper.hpp_new generated/dbHelper.hpp
else
    rm generated/dbHelper.hpp_new
fi

if ! test -f generated/dbTypeInstances.hpp || diff -q generated/dbTypeInstances.hpp_new generated/dbTypeInstances.hpp | grep -q .; then
    mv generated/dbTypeInstances.hpp_new generated/dbTypeInstances.hpp
else
    rm generated/dbTypeInstances.hpp_new
fi

#avoid updating the file if nothing changed to save build time
if ! test -f generated/onionHelper.hpp || diff -q generated/onionHelper.hpp_new generated/onionHelper.hpp | grep -q .; then
    mv generated/onionHelper.hpp_new generated/onionHelper.hpp
else
    rm generated/onionHelper.hpp_new
fi

if ! test -f generated/allImpl.hpp || diff -q generated/allImpl.hpp_new generated/allImpl.hpp | grep -q .; then
    mv generated/allImpl.hpp_new generated/allImpl.hpp
else
    rm generated/allImpl.hpp_new
fi

if ! test -f generated/allStubs.hpp || diff -q generated/allStubs.hpp_new generated/allStubs.hpp | grep -q .; then
    mv generated/allStubs.hpp_new generated/allStubs.hpp
else
    rm generated/allStubs.hpp_new
fi



