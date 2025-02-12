/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#include "uxGridLayout.hpp"
#include "uxPanel.hpp"

namespace doh {

  uxGridLayout::uxGridLayout(float height, std::initializer_list<float> fs, glm::vec2 padding) :
    columns(fs.size()),
    count(0),
    cellHeight(height),
    totalWidth(0),
    columnWidths(std::make_unique<float[]>(columns)),
    padding(padding)
  {
    auto iter = fs.begin();
    for(size_t i = 0;i < columns;i++)
      totalWidth += columnWidths[i] = *(iter++);
    totalWidth += (columns - 1) * padding.x;
  };

  void uxGridLayout::reset() {
    count = 0;
  };

  void uxGridLayout::handle(uxBase* b) {
    uint32_t colId = count % columns;
    glm::vec4 bbox { panel->bounds.x + panel->scrollOffset.x, 0, 0, 0};
    for(uint32_t i = 0;i < colId;i++)
      bbox.x += columnWidths[i] + padding.x;
    bbox.y = (cellHeight + padding.y) * (count / columns) + panel->bounds.y + panel->scrollOffset.y;
    bbox.z = bbox.x + columnWidths[colId];
    bbox.w = bbox.y + cellHeight;
    b->setBounds(bbox);
    count++;
  };

  void uxGridLayout::finalize() {
    uint32_t rows = count == 0 ? 0 : (count - 1) / columns + 1;
    panel->updateScrollBars({ totalWidth, rows * (cellHeight + padding.y) });
  };

}
