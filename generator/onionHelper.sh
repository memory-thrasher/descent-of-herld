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
while IFS= read -d $'\0' file; do
    # echo "//note: begin $file"
    while read line; do
	# echo "processing: $line"
	if [[ "$line" =~ \!\!onion\ (.*?) ]]; then
	    onion="${BASH_REMATCH[1]}"
	    #note: this doesn't do anything (yet) as I have only one onion
	elif [[ "$line" =~ \!\!append\ (.*?)\ (.*?) ]]; then
	    listName="${BASH_REMATCH[1]}"
	    listNewVar="${BASH_REMATCH[2]}"
	    # echo "//$listName += $listNewVar"
	    if ! echo "$allvars" | grep -Fwq "$listName"; then
		allvars="$listName $allvars"
		eval "$listName='$listNewVar'"
	    else
		eval "$listName=\"\$$listName, $listNewVar\""
	    fi
	    # echo -n "//$listName="
	    # eval echo \$$listName
	elif [[ "$line" =~ \!\!include\ me ]]; then
	    echo "#include \"../$file\""
	else
	    echo "Unrecognized directive: $line"
	    exit 1;
	fi
    done < <(grep -ho '!!.*' "$file")
done < <(find src -type f -not -iname '*~' -print0) >generated/onionHelper.hpp_new

while read -d' ' var; do
    echo -n "#define expand_$var(T) constexpr auto $var = WITE::concat<T, "
    eval echo -n \$$var
    echo '>();'
    echo -n "#define expand_raw_$var "
    eval echo \$$var
done <<<$allvars >>generated/onionHelper.hpp_new

#avoid updating the file if nothing changed to save build time
if diff -q generated/onionHelper.hpp_new generated/onionHelper.hpp | grep -q .; then
    mv generated/onionHelper.hpp_new generated/onionHelper.hpp
else
    rm generated/onionHelper.hpp_new
fi



