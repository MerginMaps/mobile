/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Text {
  id: root

  property bool isMMFont: (
                            (root.font == __style.h1) ||
                            (root.font == __style.h2) ||
                            (root.font == __style.h3) ||
                            (root.font == __style.t1) ||
                            (root.font == __style.t2) ||
                            (root.font == __style.t3) ||
                            (root.font == __style.t4) ||
                            (root.font == __style.t5) ||
                            (root.font == __style.p1) ||
                            (root.font == __style.p2) ||
                            (root.font == __style.p3) ||
                            (root.font == __style.p4) ||
                            (root.font == __style.p5) ||
                            (root.font == __style.p6) ||
                            (root.font == __style.p7)
                          )


  lineHeightMode: root.isMMFont ? Text.FixedHeight : Text.ProportionalHeight
  lineHeight: {
    if (!root.isMMFont) return 1.0 // Text.ProportionalHeight

    if (root.font == __style.h1) return __style.fontLineHeight48;
    if (root.font == __style.h2) return __style.fontLineHeight36;
    if (root.font == __style.h3) return __style.fontLineHeight24;
    if (root.font == __style.t1) return __style.fontLineHeight18;
    if (root.font == __style.t2) return __style.fontLineHeight16;
    if (root.font == __style.t3) return __style.fontLineHeight14;
    if (root.font == __style.t4) return __style.fontLineHeight12;
    if (root.font == __style.t5) return __style.fontLineHeight10;
    if (root.font == __style.p1) return __style.fontLineHeight32;
    if (root.font == __style.p2) return __style.fontLineHeight24;
    if (root.font == __style.p3) return __style.fontLineHeight20;
    if (root.font == __style.p4) return __style.fontLineHeight16;
    if (root.font == __style.p5) return __style.fontLineHeight14;
    if (root.font == __style.p6) return __style.fontLineHeight12;
    if (root.font == __style.p7) return __style.fontLineHeight10;
  }

  color: {
    if (
        (root.font == __style.h1) ||
        (root.font == __style.h2) ||
        (root.font == __style.h3) ||
        (root.font == __style.t1) ||
        (root.font == __style.t2) ||
        (root.font == __style.t3) ||
        (root.font == __style.t4) ||
        (root.font == __style.t5)
      ) {
      return __style.forestColor
    }

    return __style.nightColor
  }
}
