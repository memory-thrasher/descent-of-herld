#!/bin/bash


# Copyright 2024-2025 Wafflecat Games, LLC

# This file is part of The Descent of Herld.

# The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

# The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

# Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.

site_configs="../configs/doh"
if [ -f "$site_configs" ]; then
    source "$site_configs"
else
    echo "warning: site configs not found"
fi;
WITEBUILD="$WITEBASE/build"
BOTHOPTS="-DVK_NO_PROTOTYPES -DVULKAN_HPP_NO_EXCEPTIONS -I$WITEBASE"
COMPILER=clang++
WORKNICE="nice -n10"
if [ -z "${VK_SDK_PATH}" ]; then
    export VK_SDK_PATH="${VULKAN_SDK}"
fi
if [ -z "$VK_INCLUDE" -a -n "$VK_SDK_PATH" ]; then
    export VK_INCLUDE="-I${VK_SDK_PATH}/include"
fi
if [ -n "$VK_SDK_PATH" ]; then
    export VK_ADD_LAYER_PATH="${VK_ADD_LAYER_PATH}${VK_ADD_LAYER_PATH:+:}${VK_SDK_PATH}/share/vulkan/explicit_layer.d"
    #echo "VK_ADD_LAYER_PATH=$VK_ADD_LAYER_PATH"
fi
# if [ -n "$vk_lib_path" ]; then
#     PATH="$vk_lib_path:$PATH"
# fi;
export BOTHOPTS
export COMPILER
export WORKNICE
export WITEBUILD


mkdir build build/shaders build/debug build/release build/windows build/dependmap generated/shaders 2>/dev/null

#delete any obj files that no longer have a corresponding cpp file
find build -type f -iname '*.o' -print0 |
    while IFS= read -d '' OFILE; do
	find src -type f -iname "$(basename "${OFILE%.*}.cpp")" | grep -q . ||
	    rm "${OFILE}" 2>/dev/null;
    done;

ALLDSTFILES=

while IFS= read -d '' SRCFILE; do
    if [[ $SRCFILE =~ .*\.partial\.glsl ]]; then continue; fi;
    DSTFILE="build/shaders/$(basename "${SRCFILE%.*}.spv.h")"
    TMPFILE="generated/shaders/$(basename "${SRCFILE}")"
    ALLDSTFILES="$DSTFILE $ALLDSTFILES"
    if ! [ -f "${DSTFILE}" ] || [ "${SRCFILE}" -nt "${DSTFILE}" ] || [ "$0" -nt "${DSTFILE}" ]; then
	rm "${DSTFILE}" 2>/dev/null
	$WORKNICE generator/preprocessShader.sh "$SRCFILE" "$TMPFILE" "$DSTFILE" &
    fi
done < <(find shaders -iname '*.glsl' -type f -print0)

sleep 0.1s
#wait for compile to finish
while pgrep -lfa preprocessShader &>/dev/null; do sleep 0.05s; done
while IFS=$'\n' read -d ' ' DSTFILE; do
    if [ -n "$DSTFILE" -a ! -e "$DSTFILE" ]; then
	echo "File does not exist. Build failed? $DSTFILE"
	exit 1;
    fi
done <<< "$ALLDSTFILES"
ALLDSTFILES=

generator/onionHelper.sh || exit 1

tests/validateOnion.sh &

#now the actual c++ code
while IFS= read -d '' SRCFILE; do
    DSTFILE="$(basename "${SRCFILE%.*}.o")"
    DBGDSTFILE="build/debug/$DSTFILE"
    WINDSTFILE="build/windows/$DSTFILE"
    RELDSTFILE="build/release/$DSTFILE"
    DEPENDENCIES="build/dependmap/${DSTFILE%.*}.d"
    ALLDSTFILES="$ALLDSTFILES $DBGDSTFILE $RELDSTFILE $WINDSTFILE $DEPENDENCIES"
    (
	#if any target needs recompiled, do them all just for a sanity check
	if [ -f "${DBGDSTFILE}" -a "${DBGDSTFILE}" -nt "$0" -a "${DBGDSTFILE}" -nt "${SRCFILE}" -a \
		-f "${RELDSTFILE}" -a "${RELDSTFILE}" -nt "$0" -a "${RELDSTFILE}" -nt "${SRCFILE}" -a \
		-f "${WINDSTFILE}" -a "${WINDSTFILE}" -nt "$0" -a "${WINDSTFILE}" -nt "${SRCFILE}" ]; then
	    #note: this does not check the win sdk for changes
	    while read depend; do
		if [ ! -f "${depend}" -o "${depend}" -nt "${DBGDSTFILE}" ]; then #all 3 files have essentially the same ctime
		    echo "rebuilding ${SRCFILE}";
		fi
	    done <"${DEPENDENCIES}" | grep -Fm1 rebuild || exit 0;
	else
	    $WORKNICE $COMPILER $VK_INCLUDE -I "build/shaders" -E -o /dev/null -w -ferror-limit=1 -H "${SRCFILE}" --std=c++20 -fPIC -fdata-sections -ffunction-sections -fdata-sections -DDEBUG -g $BOTHOPTS 2>&1 | grep '^\.' | grep -o '[^[:space:]]*$' | sort -u >"${DEPENDENCIES}" &
	fi
	#use dd as a buffer so the output from parallel jobs collides less
	(if ! $WORKNICE $COMPILER $VK_INCLUDE -I "build/shaders" --std=c++20 -fPIC -fdata-sections -ffunction-sections -fdata-sections -DDEBUG -g $BOTHOPTS -Werror -Wall "${SRCFILE}" -c -o "${DBGDSTFILE}" 2>&1 | dd bs=4096 status=none; then
	     rm "${DBGDSTFILE}" 2>/dev/null
	     echo "Failed Build: ${SRCFILE}"
	     #echo -- $WORKNICE $COMPILER $VK_INCLUDE -I "build/shaders" --std=c++20 -fPIC -fdata-sections -ffunction-sections -fdata-sections -DDEBUG -g $BOTHOPTS -Werror -Wall "${SRCFILE}" -c -o "${DBGDSTFILE}"
	 fi)&
	(if ! $WORKNICE $COMPILER $VK_INCLUDE -I "build/shaders" --std=c++20 -fPIC -ffunction-sections -fdata-sections -O3 $BOTHOPTS -Werror -Wall "${SRCFILE}" -c -o "${RELDSTFILE}" 2>&1 | dd bs=4096 status=none; then
	     rm "${RELDSTFILE}" 2>/dev/null
	     echo "[release build] Failed Build: ${SRCFILE}"
	 fi)&
	(if ! $WORKNICE $WIN_COMPILER /std:c++20 -Diswindows /O2 $VK_INCLUDE -I "build/shaders" $BOTHOPTS -c "${SRCFILE}" -o "${WINDSTFILE}" -Wno-unused-result 2>&1 | dd bs=4096 status=none; then
	     rm "${WINDSTFILE}" 2>/dev/null
	     echo "[cross compile for windows] Failed Build: ${SRCFILE}"
	 fi)&
    ) &
done < <(find src -name '*.cpp' -type f -print0)

sleep 0.1s
#wait for compile to finish
while pgrep $WIN_COMPILER_RUNNING_TEST &>/dev/null || pgrep $COMPILER &>/dev/null; do sleep 0.05s; done
while read -d ' ' DSTFILE; do
    if [ -n "$DSTFILE" -a ! -e $DSTFILE ]; then
	echo "File does not exist. Build failed? $DSTFILE"
	exit 1;
    fi
done <<< "$ALLDSTFILES"
ALLDSTFILES=

$WORKNICE $WIN_LINKER /opt:icf=4 /subsystem:windows /entry:mainCRTStartup build/windows/*.o $WITEBUILD/windows/WITE/*.o /defaultlib:SDL2 /defaultlib:SHELL32.lib "/out:build/windows/descentOfHerld.exe" || echo 'windows link failed' &

$WORKNICE $COMPILER -Wl,--icf=all build/release/*.o $WITEBUILD/release/WITE/*.o "-Wl,-rpath,." -fuse-ld=lld -lrt -latomic -L${VK_SDK_PATH}/lib -lvulkan -lSDL2 -Wl,--gc-sections $BOTHOPTS -o "build/release/descentOfHerld" || echo 'release link failed' &

$WORKNICE $COMPILER -DDEBUG -Wl,--icf=all build/debug/*.o $WITEBUILD/WITE/*.o "-Wl,-rpath,." -fuse-ld=lld -lrt -latomic -L${VK_SDK_PATH}/lib -lvulkan -lSDL2 -Wl,--gc-sections $BOTHOPTS -o "build/debug/descentOfHerld" || exit 1

#LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$VK_SDK_PATH/lib" ./build/debug/descentOfHerld

while pgrep $WIN_COMPILER_RUNNING_TEST &>/dev/null || $COMPILER &>/dev/null; do sleep 0.05s; done

echo done

