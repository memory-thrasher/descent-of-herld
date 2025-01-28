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
if [ -z "${VK_SDK_PATH}" ]; then
    export VK_SDK_PATH="${VULKAN_SDK}"
fi
if [ -n "$VK_SDK_PATH" ]; then
    export VK_ADD_LAYER_PATH="${VK_SDK_PATH}/share/vulkan/explicit_layer.d"
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$VK_SDK_PATH/lib"
fi

#run, then quit: if a tracepoint is struck or an error occurs, the process will still be running, causing quit to prompt to kill it, so the user can enter n to perform debug, while a successful run closes automatically
gdb -ex 'set debuginfod enabled off' -ex run -ex quit ./build/debug/descentOfHerld
