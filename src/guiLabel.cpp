/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include <set>
#include <iostream>

#include "guiLabel.hpp"
#include "../generated/targetPrimary_stub.hpp"
#include "math.hpp"
#include "uiStyle.hpp"

namespace doh {

  guiLabel::guiLabel(textOnlyStyle_t& style, glm::vec4 bbox, std::string labelStr) :
    rect(guiRect::create()),
    rectData(bbox),
    label(guiText::create()),
    labelData(bbox),
    style(style),
    labelStr(labelStr)
  {
    if(bbox.w < bbox.y) [[unlikely]] {
      //setting negative height (usually w = -1) means "make it one line high using style info"
      bbox.w = bbox.y + style.text.charMetric.y + 2*style.text.charMetric.w;
      rectData.extents = bbox;
      labelData.bbox = bbox;
    }
    guiTextFormat(labelContent, "%s", labelStr.c_str());
    rect.writeInstanceData(rectData);
    rect.setStyle(style.rectBuf);
    label.writeIndirectBuffer(labelContent);
    label.writeInstanceData(labelData);
    label.setStyle(style.textBuf);
  };

  guiLabel::guiLabel(textOnlyStyle_t& style, glm::vec2 ul, std::string labelStr) :
    guiLabel(style, glm::vec4(ul, ul.x + labelStr.length() * style.text.charMetric.x + style.text.charMetric.z*2, ul.y + style.text.charMetric.y + style.text.charMetric.w*2), labelStr) {};

  guiLabel::~guiLabel() {
    rect.destroy();
    label.destroy();
  };

  guiLabel::guiLabel(const guiLabel& o) : guiLabel(o.style, glm::vec4(o.rectData.extents), o.labelStr) {};

}
