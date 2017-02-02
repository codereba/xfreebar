/*
 * Copyright 2010 www.codereba.com
 *
 * This file is part of xfreebar.
 *
 * xfreebar is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xfreebar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xfreebar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#define SPLIT_PART_WIDTH 18
#define ARROW_WIDTH 5
#define ARROW_HEIGHT 3
#define ARROW_AROUND_COLOR RGB( 107, 107, 106 )
#define ARROW_INNER_COLOR RGB( 0, 0, 0 )
#define ARROW_ROUND_COLOR RGB(105, 107, 106)

#define BTN_NORM_BDR_COLOR RGB(198, 198, 198)
#define BTN_HOT_BDR_COLOR RGB(196, 196, 196)
#define BTN_PRESSED_BDR_COLOR RGB(185, 185, 175)
#define BTN_CHECKED_BDR_COLOR RGB(122, 152, 175)

#define BTN_NORM_BK_COLOR RGB(236, 232, 217)
#define BTN_PRESSED_BK_COLOR RGB(220, 220, 220)
#define BTN_HOT_BK_COLOR RGB(248, 249, 250)
#define BTN_CHECKED_BK_COLOR RGB(255, 255, 255)

#define BTN_NORM_BOT_LINE_COLOR1 RGB( 212, 206, 184 )
#define BTN_NORM_BOT_LINE_COLOR2 RGB( 205, 199, 174 )
#define BTN_HOT_BOT_LINE_COLOR1 RGB( 215, 213, 212 )
#define BTN_HOT_BOT_LINE_COLOR2 RGB( 203, 201, 199 )

#define PRESS_STATE_MASK 0x8000
#define ROUND_LEN 3

#define SPLIT_WIDTH 15
#define LINE_COLOR_DECREMENT 0x010101
#define GRADIENT_LINE_REGION 0.1414
#define HOT_HOR_GRADIENT_LINE_REGION ( GRADIENT_LINE_REGION / 2 )

#define BTN_TEXT_LEN MAX_PATH

#define SEP_LEFT_LINE_CLR RGB(182,182, 182)
#define SEP_RIGHT_LINE_CLR RGB(210, 210, 210)

INT32 DrawMoveDepartLine(CDC &DrawDC, LONG  lBeginX, LONG lBeginY, LONG lWidth, LONG lHeight);
INT32 DrawArrow(CDC &DrawDC,RECT &rcDraw);
DWORD GetLinearColor(INT32 nLineNo, DWORD dwTopColor, DWORD dwColorInc = LINE_COLOR_DECREMENT);
DWORD GetTrendColor(INT32 nLineNo, DOUBLE dRedFactor, DOUBLE dGreenFactor, DOUBLE dBlueFactor );
DWORD DrawHotLineBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, INT32 nSplit, const CRect &rcBtn, const CSize szRound );
DWORD DrawPressedBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, INT32 nSplit, const CRect &rcBtn, const CSize szRound );
DWORD DrawPressedSplitBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, const CRect &rcBtn, const CSize szRound);
DWORD DrawDisabledBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, const CRect &rcBtn, const CSize szRound);
INT32 DrawPressedBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound);
INT32 DrawDisabledBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound);
INT32 DrawPressedSplitBtn(CDC &DrawDC, const CRect &rcBtn, const CSize szRound);
INT32 DrawHotLightBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound);
INT32 DrawSeparator(CDC &DrawDC, RECT &rcDraw);
INT32 DrawSplitter(CDC &DrawDC, RECT &rcDraw);
INT32 GradientDraw(CDC &DrawDC,RECT &rcDraw, SIZE zRoundSz, COLORREF clrBegin, COLORREF clrEnd);
INT32 DrawChevronSign(CDC &DrawDC, LONG lBeginX, LONG lBeginY);
DWORD DrawCheckedBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, INT32 nSplit, const CRect &rcBtn, const CSize szRound );
INT32 DrawCheckedBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound);
INT32 DrawHotLightCheckedBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound);
INT32 DrawDragInsertMark( INT32 nPos, CDCHandle dc);
INT32 DrawDisabledImage( HBITMAP hBmp, HDC hDC, LONG left, LONG top, LONG right, LONG bottom );