#!/bin/bash

# Copyright 2024 Wafflecat Games, LLC

# This file is part of The Descent of Herld.

# The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

# The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

# Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.

F=$1
if ! [ -f $F ]; then
    echo 'file not found' $F
fi
G=$F.tmp

#last include first to keep line numbers in order
while grep -qF '#include' $F; do
    INC="$(grep -nF '#include' $F | tail -n1)"
    LN=$(echo "$INC" | cut -d: -f1)
    FN="shaders/$(echo "$INC" | cut -d\  -f2-)"
    if ! [ -e $FN ]; then
	echo "shader preprocessor: file not found: $FN"
	exit 1
    fi
    if ! [ $LN -gt 0 ]; then
	echo "invalid line number"
	exit 1
    fi
    head -n $(($LN-1)) $F >$G
    echo "//include $FN" >>$G
    echo '#line 0' >>$G
    cat $FN >>$G
    echo "//end include $FN" >>$G
    echo '#line' $(($LN+1)) >>$G
    tail -n +$(($LN+1)) $F >>$G
    mv $G $F
done

