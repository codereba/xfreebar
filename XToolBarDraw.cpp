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

#include "StdAfx.h"
#include "XToolBarDraw.h"
INT32 DrawMoveDepartLine(CDC &DrawDC, LONG  lBeginX, LONG lBeginY, LONG lWidth, LONG lHeight)
{
#define MOVE_DEPART_LINE_HEADER_WIDTH 4
#define MOVE_DEPART_LINE_BODY_WIDHT 2
#define MOVE_DEPART_LINE_HEIGHT 30
#define MOVE_DEPART_LINE_COLOR RGB(0, 0, 0)

	CPen PenLine;
	CPenHandle PenOld;
	PenLine.CreatePen(PS_SOLID, MOVE_DEPART_LINE_BODY_WIDHT, MOVE_DEPART_LINE_COLOR);
	PenOld = (HPEN)DrawDC.SelectPen(PenLine);

	//LONG lBeginX;
	//LONG lBeginY;
	DrawDC.SetPixel(lBeginX, lBeginY, MOVE_DEPART_LINE_COLOR);
	DrawDC.SetPixel(lBeginX + MOVE_DEPART_LINE_HEADER_WIDTH - 2, lBeginY, MOVE_DEPART_LINE_COLOR);
	DrawDC.SetPixel(lBeginX, lBeginY + MOVE_DEPART_LINE_HEIGHT, MOVE_DEPART_LINE_COLOR);
	DrawDC.SetPixel(lBeginX + MOVE_DEPART_LINE_HEADER_WIDTH - 2, lBeginY + MOVE_DEPART_LINE_HEIGHT, MOVE_DEPART_LINE_COLOR);
#define MOVE_DEPART_LINE_OFFSET (MOVE_DEPART_LINE_HEADER_WIDTH - MOVE_DEPART_LINE_BODY_WIDHT) / 2
	DrawDC.MoveTo(lBeginX + MOVE_DEPART_LINE_OFFSET, lBeginY);
	DrawDC.LineTo(lBeginX + MOVE_DEPART_LINE_OFFSET, lBeginY + MOVE_DEPART_LINE_HEIGHT);
	DrawDC.SelectPen(PenOld);
	return 0;
}

INT32 DrawArrow(CDC &DrawDC,RECT &rcDraw)
{
	UINT32 uArrowX;
	UINT32 uArrowY;
	UINT32 uSplitPartX;

	uSplitPartX = rcDraw.right - SPLIT_PART_WIDTH;
	uArrowX = uSplitPartX + SPLIT_PART_WIDTH / 2 - ARROW_WIDTH / 2;
	uArrowY = rcDraw.top + (rcDraw.bottom - rcDraw.top) / 2 - ARROW_HEIGHT / 2;

	//Draw the around pixels of arrow
	DrawDC.SetPixel(uArrowX, uArrowY, ARROW_AROUND_COLOR);
	DrawDC.SetPixel(uArrowX + 1, uArrowY + 1, ARROW_AROUND_COLOR);
	DrawDC.SetPixel(uArrowX + 2, uArrowY + 2, ARROW_AROUND_COLOR);
	DrawDC.SetPixel(uArrowX + 3, uArrowY + 1, ARROW_AROUND_COLOR);
	DrawDC.SetPixel(uArrowX + 4, uArrowY, ARROW_AROUND_COLOR);
	
	//Draw the inner pixels of arrow
	DrawDC.SetPixel(uArrowX + 1, uArrowY, ARROW_INNER_COLOR);
	DrawDC.SetPixel(uArrowX + 2, uArrowY, ARROW_INNER_COLOR);
	DrawDC.SetPixel(uArrowX + 3, uArrowY, ARROW_INNER_COLOR);
	DrawDC.SetPixel(uArrowX + 2, uArrowY + 1, ARROW_INNER_COLOR);

	return 0;
}

DWORD GetLinearColor(INT32 nLineNo, DWORD dwTopColor, DWORD dwColorInc)
{
	DWORD dwColor;
	dwColor = dwTopColor - (nLineNo * dwColorInc);
	return dwColor;
}

DWORD GetTrendColor(INT32 nLineNo, DOUBLE dRedFactor, DOUBLE dGreenFactor, DOUBLE dBlueFactor )
{
	DWORD dwColor;
	dwColor = ((DWORD)(nLineNo * dRedFactor)) | (((DWORD)(nLineNo * dGreenFactor)) << 8) | (((DWORD)(nLineNo * dBlueFactor)) << 16);
	return dwColor;
}

DWORD DrawCheckedBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, INT32 nSplit, const CRect &rcBtn, const CSize szRound )
{
	//ATLTRACE("Enter DrawPressedBtnLine\n");
	DWORD dwLineColor;
	INT32 nGradientLns = rcBtn.Height() * GRADIENT_LINE_REGION;
	INT32 nLeft;
	INT32 nRight;
	DOUBLE dRoundScale = szRound.cy / szRound.cx;
	DWORD dwBdrInnerColor;
	DWORD dwBdrOuterColor;
	DWORD dwBdrCenterColor;
	INT32 nRoundLen = 0; 
	INT32 nHeight;
	nHeight = rcBtn.Height();
	INT32 nReverseLine = nHeight - nLineNo - 1;

	if( nLineNo < szRound.cy)
	{
		nRoundLen = ( szRound.cy - nLineNo )* dRoundScale;

	}
	else if( nReverseLine < szRound.cy)
	{
		nRoundLen = (szRound.cy - (nReverseLine)) * dRoundScale;
	}
	
	if(nLineNo == 0)
	{
		dwLineColor = BTN_CHECKED_BDR_COLOR;
	}
	else if(nLineNo < nGradientLns && nLineNo > 0 )
	{
		dwLineColor = GetLinearColor(nGradientLns - nLineNo, BTN_CHECKED_BK_COLOR, LINE_COLOR_DECREMENT * 5);
	}
	else if( nLineNo >= nGradientLns && (nReverseLine)>= nGradientLns )
	{
		dwLineColor = GetLinearColor(nLineNo - nGradientLns, BTN_CHECKED_BK_COLOR, LINE_COLOR_DECREMENT);
	}
	else if ( (nReverseLine) < nGradientLns && nLineNo < nHeight - 1)
	{
		dwLineColor = GetLinearColor(nGradientLns - (nReverseLine), BTN_CHECKED_BK_COLOR - ( nHeight - nGradientLns ) * LINE_COLOR_DECREMENT, LINE_COLOR_DECREMENT * 4);
	}
	else if (nLineNo == nHeight - 1)
	{
		dwLineColor = BTN_CHECKED_BDR_COLOR;
	}

	DWORD dwPixelColor;
	INT32 nFillPixelLen;
#define BDR_PIXEL_LEN 3
	if(nGradientLns > nRoundLen + BDR_PIXEL_LEN - 1)
	{
		nFillPixelLen = nGradientLns;
	}
	else
	{
		nFillPixelLen = nRoundLen + BDR_PIXEL_LEN - 1;
	}

	CPenHandle OldPen;
	nLeft = rcBtn.left;
	nRight = rcBtn.right;
//#define CALC_COLOR_PART(color, part) (((DWORD)(color & 0xff)) / part ) | (((((DWORD)(color & 0xff00)) >> 8) / part) << 8) | (((((DWORD)(color & 0xff0000)) >> 16 ) / part ) << 16)
	dwBdrInnerColor = BTN_CHECKED_BDR_COLOR + GetTrendColor( 15, 1, 1, 1 );
	dwBdrCenterColor = BTN_CHECKED_BDR_COLOR + GetTrendColor( 20, 1, 1, 1 );
	dwBdrOuterColor = BTN_CHECKED_BDR_COLOR + GetTrendColor( 25, 1, 1, 1 ) + GetTrendColor( 2, 0.8, 0.7, 1.1 );
	if(PenDraw.m_hPen != NULL) PenDraw.DeleteObject();
	PenDraw.CreatePen(PS_SOLID, 1, dwLineColor);
	OldPen = DrawDC.SelectPen(PenDraw);
	DrawDC.MoveTo(nLeft + nFillPixelLen, rcBtn.top + nLineNo);
	DrawDC.LineTo(nRight - nFillPixelLen + 1, rcBtn.top + nLineNo);
	if(nSplit)
	{
		DrawDC.SetPixel( nRight - SPLIT_WIDTH, rcBtn.top + nLineNo, BTN_CHECKED_BDR_COLOR);
	}
	
	INT32 nRoundBeginX;
	nRoundBeginX = nRoundLen - 1;
	if( nRoundBeginX < 0)
	{
		nRoundBeginX = 0;
	}

	if( nLineNo < szRound.cy || nReverseLine < szRound.cy )
	{
		for(INT32 i = 0; i < nFillPixelLen; i++)
		{
			if( i >= nRoundBeginX && i < nRoundBeginX + BDR_PIXEL_LEN)
			{
				if( i == nRoundBeginX)
				{
					dwPixelColor = dwBdrOuterColor;
				}
				else if(i == nRoundBeginX + 1)
				{
					dwPixelColor = dwBdrCenterColor;
				}
				else if(i == nRoundBeginX + 2)
				{
					dwPixelColor = dwBdrInnerColor;
				}
				else
				{
					ATLASSERT(FALSE);
				}
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
			else if( i < nGradientLns && i >= nRoundLen + 3)
			{
				dwPixelColor = GetLinearColor(nGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
		}
	}
	else
	{
		for(INT32 i = 1; i < nFillPixelLen; i++)
		{
			if( i < nGradientLns )
			{
				dwPixelColor = GetLinearColor(nGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
		}
		DrawDC.SetPixel(nLeft, rcBtn.top + nLineNo, BTN_CHECKED_BDR_COLOR );
		DrawDC.SetPixel(nRight, rcBtn.top + nLineNo, BTN_CHECKED_BDR_COLOR );
	}

	//if( nSplit )
	//{
	//	PenDraw.DeleteObject();
	//	PenDraw.CreatePen( PS_SOLID, 1, BTN_CHECKED_BDR_COLOR);
	//	DrawDC.MoveTo(nRight - SPLIT_WIDTH, rcBtn.top);
	//	DrawDC.LineTo(nRight - SPLIT_WIDTH, rcBtn.bottom);
	//}
	DrawDC.SelectPen(OldPen);
	return dwLineColor;
}


DWORD DrawCheckedLightBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, INT32 nSplit, const CRect &rcBtn, const CSize szRound )
{
	//ATLTRACE("Enter DrawHotLineBtnLine\n");
	DWORD dwLineColor;
	INT32 nGradientLns = rcBtn.Height() * GRADIENT_LINE_REGION;
	INT32 nLeft;
	INT32 nRight;
	DOUBLE dRoundScale = szRound.cy / szRound.cx;
	DWORD dwBdrInnerColor;
	DWORD dwBdrOuterColor;
	DWORD dwBdrCenterColor;
	INT32 nRoundLen = 0; 
	INT32 nHeight;
	nHeight = rcBtn.Height();
	INT32 nReverseLine = nHeight - nLineNo - 1;
	INT32 nHotGradientLns = rcBtn.Height() * HOT_HOR_GRADIENT_LINE_REGION;

	if( nLineNo < szRound.cy)
	{
		nRoundLen = ( szRound.cy - nLineNo )* dRoundScale;
	}
	else if( nReverseLine < szRound.cy)
	{
		nRoundLen = (szRound.cy - (nReverseLine)) * dRoundScale;
	}
	
	if(nLineNo == 0)
	{
		dwLineColor = BTN_CHECKED_BDR_COLOR;
	}
	else if( nReverseLine > nGradientLns)
	{
		dwLineColor = GetLinearColor(nLineNo, BTN_CHECKED_BK_COLOR, LINE_COLOR_DECREMENT );
	}
	else if ( nReverseLine <= nGradientLns && nLineNo < nHeight - 1)
	{
		dwLineColor = GetLinearColor(nGradientLns - (nReverseLine), BTN_CHECKED_BK_COLOR - ( nHeight - nGradientLns ) * LINE_COLOR_DECREMENT, LINE_COLOR_DECREMENT * 4);
	}
	else if (nLineNo == nHeight - 1)
	{
		dwLineColor = BTN_CHECKED_BDR_COLOR;
	}

	CPenHandle OldPen;
	DWORD dwPixelColor;
	INT32 nFillPixelLen;
#define BDR_PIXEL_LEN 3
	if(nGradientLns > nRoundLen + BDR_PIXEL_LEN - 1)
	{
		nFillPixelLen = nHotGradientLns;
	}
	else
	{
		nFillPixelLen = nRoundLen + BDR_PIXEL_LEN - 1;
	}

	nLeft = rcBtn.left;
	nRight = rcBtn.right;
//#define CALC_COLOR_PART(color, part) (((DWORD)(color & 0xff)) / part ) | (((((DWORD)(color & 0xff00)) >> 8) / part) << 8) | (((((DWORD)(color & 0xff0000)) >> 16 ) / part ) << 16)
	dwBdrInnerColor = BTN_CHECKED_BDR_COLOR + GetTrendColor( 15, 1, 1, 1 );
	dwBdrCenterColor = BTN_CHECKED_BDR_COLOR + GetTrendColor( 20, 1, 1, 1 );
	dwBdrOuterColor = BTN_CHECKED_BDR_COLOR + GetTrendColor( 25, 1, 1, 1 ) + GetTrendColor( 2, 0.8, 0.7, 1.1 );
	
	if(PenDraw.m_hPen != NULL) PenDraw.DeleteObject();
	PenDraw.CreatePen(PS_SOLID, 1, dwLineColor);
	OldPen = DrawDC.SelectPen(PenDraw);
	DrawDC.MoveTo(nLeft + nFillPixelLen, rcBtn.top + nLineNo);
	DrawDC.LineTo(nRight - nFillPixelLen + 1, rcBtn.top + nLineNo);
	if(nSplit)
	{
		DrawDC.SetPixel( nRight - SPLIT_WIDTH, rcBtn.top + nLineNo, BTN_CHECKED_BDR_COLOR);
	}
	
	INT32 nRoundBeginX;
	nRoundBeginX = nRoundLen - 1;
	if( nRoundBeginX < 0)
	{
		nRoundBeginX = 0;
	}

	if( nLineNo < szRound.cy || nReverseLine < szRound.cy )
	{
		for(INT32 i = 0; i < nFillPixelLen; i++)
		{
			if( i >= nRoundBeginX && i < nRoundBeginX + BDR_PIXEL_LEN)
			{
				if( i == nRoundBeginX)
				{
					dwPixelColor = dwBdrOuterColor;
				}
				else if(i == nRoundBeginX + 1)
				{
					dwPixelColor = dwBdrCenterColor;
				}
				else if(i == nRoundBeginX + 2)
				{
					dwPixelColor = dwBdrInnerColor;
				}
				else
				{
					ATLASSERT(FALSE);
				}
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
			else if( i < nGradientLns && i >= nRoundLen + 3)
			{
				dwPixelColor = GetLinearColor(nGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
		}
	}
	else
	{
		for(INT32 i = 1; i < nFillPixelLen; i++)
		{
			if( i < nGradientLns )
			{
				dwPixelColor = GetLinearColor(nGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
		}
		DrawDC.SetPixel(nLeft, rcBtn.top + nLineNo, BTN_PRESSED_BDR_COLOR );
		DrawDC.SetPixel(nRight, rcBtn.top + nLineNo, BTN_PRESSED_BDR_COLOR );
	}

	DrawDC.SelectPen(OldPen);
	return dwLineColor;
}

DWORD DrawHotLineBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, INT32 nSplit, const CRect &rcBtn, const CSize szRound )
{
	//ATLTRACE("Enter DrawHotLineBtnLine\n");
	DWORD dwLineColor;
	DWORD dwColorDec;
	INT32 nGradientLns = rcBtn.Height() * GRADIENT_LINE_REGION;
	INT32 nLeft;
	INT32 nRight;
	DOUBLE dRoundScale = szRound.cy / szRound.cx;
	DWORD dwBdrInnerColor;
	DWORD dwBdrOuterColor;
	DWORD dwBdrCenterColor;
	INT32 nRoundLen = 0; 
	INT32 nHeight;
	nHeight = rcBtn.Height();
	INT32 nReverseLine = nHeight - nLineNo - 1;
	INT32 nHotGradientLns = rcBtn.Height() * HOT_HOR_GRADIENT_LINE_REGION;

	if( nLineNo < szRound.cy)
	{
		nRoundLen = ( szRound.cy - nLineNo )* dRoundScale;
	}
	else if( nReverseLine < szRound.cy)
	{
		nRoundLen = (szRound.cy - (nReverseLine)) * dRoundScale;
	}
	
	if(nLineNo == 0)
	{
		dwLineColor = BTN_HOT_BDR_COLOR;
	}
	else if( nReverseLine > nGradientLns)
	{
		dwLineColor = GetLinearColor(nLineNo, BTN_HOT_BK_COLOR, LINE_COLOR_DECREMENT );
	}
	else if ( nReverseLine <= nGradientLns && nLineNo < nHeight - 1)
	{
		dwLineColor = GetLinearColor(nGradientLns - (nReverseLine), BTN_HOT_BK_COLOR - ( nHeight - nGradientLns ) * LINE_COLOR_DECREMENT, LINE_COLOR_DECREMENT * 4);
	}
	else if (nLineNo == nHeight - 1)
	{
		dwLineColor = BTN_HOT_BDR_COLOR;
	}

	CPenHandle OldPen;
	DWORD dwPixelColor;
	INT32 nFillPixelLen;
#define BDR_PIXEL_LEN 3
	if(nGradientLns > nRoundLen + BDR_PIXEL_LEN - 1)
	{
		nFillPixelLen = nHotGradientLns;
	}
	else
	{
		nFillPixelLen = nRoundLen + BDR_PIXEL_LEN - 1;
	}

	nLeft = rcBtn.left;
	nRight = rcBtn.right;
//#define CALC_COLOR_PART(color, part) (((DWORD)(color & 0xff)) / part ) | (((((DWORD)(color & 0xff00)) >> 8) / part) << 8) | (((((DWORD)(color & 0xff0000)) >> 16 ) / part ) << 16)
	dwBdrInnerColor = BTN_HOT_BDR_COLOR + GetTrendColor( 15, 1, 1, 1 );
	dwBdrCenterColor = BTN_HOT_BDR_COLOR + GetTrendColor( 20, 1, 1, 1 );
	dwBdrOuterColor = BTN_HOT_BDR_COLOR + GetTrendColor( 25, 1, 1, 1 ) + GetTrendColor( 2, 0.8, 0.7, 1.1 );
	
	if(PenDraw.m_hPen != NULL) PenDraw.DeleteObject();
	PenDraw.CreatePen(PS_SOLID, 1, dwLineColor);
	OldPen = DrawDC.SelectPen(PenDraw);
	DrawDC.MoveTo(nLeft + nFillPixelLen, rcBtn.top + nLineNo);
	DrawDC.LineTo(nRight - nFillPixelLen + 1, rcBtn.top + nLineNo);
	if(nSplit)
	{
		DrawDC.SetPixel( nRight - SPLIT_WIDTH, rcBtn.top + nLineNo, BTN_HOT_BDR_COLOR);
	}
	
	INT32 nRoundBeginX;
	nRoundBeginX = nRoundLen - 1;
	if( nRoundBeginX < 0)
	{
		nRoundBeginX = 0;
	}

	if( nLineNo < szRound.cy || nReverseLine < szRound.cy )
	{
		for(INT32 i = 0; i < nFillPixelLen; i++)
		{
			if( i >= nRoundBeginX && i < nRoundBeginX + BDR_PIXEL_LEN)
			{
				if( i == nRoundBeginX)
				{
					dwPixelColor = dwBdrOuterColor;
				}
				else if(i == nRoundBeginX + 1)
				{
					dwPixelColor = dwBdrCenterColor;
				}
				else if(i == nRoundBeginX + 2)
				{
					dwPixelColor = dwBdrInnerColor;
				}
				else
				{
					ATLASSERT(FALSE);
				}
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
			else if( i < nGradientLns && i >= nRoundLen + 3)
			{
				dwPixelColor = GetLinearColor(nGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
		}
	}
	else
	{
		for(INT32 i = 1; i < nFillPixelLen; i++)
		{
			if( i < nGradientLns )
			{
				dwPixelColor = GetLinearColor(nGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
		}
		DrawDC.SetPixel(nLeft, rcBtn.top + nLineNo, BTN_PRESSED_BDR_COLOR );
		DrawDC.SetPixel(nRight, rcBtn.top + nLineNo, BTN_PRESSED_BDR_COLOR );
	}

	DrawDC.SelectPen(OldPen);
	return dwLineColor;
}

DWORD DrawPressedBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, INT32 nSplit, const CRect &rcBtn, const CSize szRound )
{
	//ATLTRACE("Enter DrawPressedBtnLine\n");
	DWORD dwLineColor;
	INT32 nGradientLns = rcBtn.Height() * GRADIENT_LINE_REGION;
	INT32 nLeft;
	INT32 nRight;
	DOUBLE dRoundScale = szRound.cy / szRound.cx;
	DWORD dwBdrInnerColor;
	DWORD dwBdrOuterColor;
	DWORD dwBdrCenterColor;
	INT32 nRoundLen = 0; 
	INT32 nHeight;
	nHeight = rcBtn.Height();
	INT32 nReverseLine = nHeight - nLineNo - 1;

	if( nLineNo < szRound.cy)
	{
		nRoundLen = ( szRound.cy - nLineNo )* dRoundScale;

	}
	else if( nReverseLine < szRound.cy)
	{
		nRoundLen = (szRound.cy - (nReverseLine)) * dRoundScale;
	}
	
	if(nLineNo == 0)
	{
		dwLineColor = BTN_PRESSED_BDR_COLOR;
	}
	else if(nLineNo < nGradientLns && nLineNo > 0 )
	{
		dwLineColor = GetLinearColor(nGradientLns - nLineNo, BTN_PRESSED_BK_COLOR, LINE_COLOR_DECREMENT * 5);
	}
	else if( nLineNo >= nGradientLns && (nReverseLine)>= nGradientLns )
	{
		dwLineColor = GetLinearColor(nLineNo - nGradientLns, BTN_PRESSED_BK_COLOR, LINE_COLOR_DECREMENT);
	}
	else if ( (nReverseLine) < nGradientLns && nLineNo < nHeight - 1)
	{
		dwLineColor = GetLinearColor(nGradientLns - (nReverseLine), BTN_PRESSED_BK_COLOR - ( nHeight - nGradientLns ) * LINE_COLOR_DECREMENT, LINE_COLOR_DECREMENT * 4);
	}
	else if (nLineNo == nHeight - 1)
	{
		dwLineColor = BTN_PRESSED_BDR_COLOR;
	}

	DWORD dwPixelColor;
	INT32 nFillPixelLen;
#define BDR_PIXEL_LEN 3
	if(nGradientLns > nRoundLen + BDR_PIXEL_LEN - 1)
	{
		nFillPixelLen = nGradientLns;
	}
	else
	{
		nFillPixelLen = nRoundLen + BDR_PIXEL_LEN - 1;
	}

	CPenHandle OldPen;
	nLeft = rcBtn.left;
	nRight = rcBtn.right;
//#define CALC_COLOR_PART(color, part) (((DWORD)(color & 0xff)) / part ) | (((((DWORD)(color & 0xff00)) >> 8) / part) << 8) | (((((DWORD)(color & 0xff0000)) >> 16 ) / part ) << 16)
	dwBdrInnerColor = BTN_PRESSED_BDR_COLOR + GetTrendColor( 15, 1, 1, 1 );
	dwBdrCenterColor = BTN_PRESSED_BDR_COLOR + GetTrendColor( 20, 1, 1, 1 );
	dwBdrOuterColor = BTN_PRESSED_BDR_COLOR + GetTrendColor( 25, 1, 1, 1 ) + GetTrendColor( 2, 0.8, 0.7, 1.1 );
	if(PenDraw.m_hPen != NULL) PenDraw.DeleteObject();
	PenDraw.CreatePen(PS_SOLID, 1, dwLineColor);
	OldPen = DrawDC.SelectPen(PenDraw);
	DrawDC.MoveTo(nLeft + nFillPixelLen, rcBtn.top + nLineNo);
	DrawDC.LineTo(nRight - nFillPixelLen + 1, rcBtn.top + nLineNo);
	if(nSplit)
	{
		DrawDC.SetPixel( nRight - SPLIT_WIDTH, rcBtn.top + nLineNo, BTN_PRESSED_BDR_COLOR);
	}
	
	INT32 nRoundBeginX;
	nRoundBeginX = nRoundLen - 1;
	if( nRoundBeginX < 0)
	{
		nRoundBeginX = 0;
	}

	if( nLineNo < szRound.cy || nReverseLine < szRound.cy )
	{
		for(INT32 i = 0; i < nFillPixelLen; i++)
		{
			if( i >= nRoundBeginX && i < nRoundBeginX + BDR_PIXEL_LEN)
			{
				if( i == nRoundBeginX)
				{
					dwPixelColor = dwBdrOuterColor;
				}
				else if(i == nRoundBeginX + 1)
				{
					dwPixelColor = dwBdrCenterColor;
				}
				else if(i == nRoundBeginX + 2)
				{
					dwPixelColor = dwBdrInnerColor;
				}
				else
				{
					ATLASSERT(FALSE);
				}
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
			else if( i < nGradientLns && i >= nRoundLen + 3)
			{
				dwPixelColor = GetLinearColor(nGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
		}
	}
	else
	{
		for(INT32 i = 1; i < nFillPixelLen; i++)
		{
			if( i < nGradientLns )
			{
				dwPixelColor = GetLinearColor(nGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwPixelColor );
			}
		}
		DrawDC.SetPixel(nLeft, rcBtn.top + nLineNo, BTN_PRESSED_BDR_COLOR );
		DrawDC.SetPixel(nRight, rcBtn.top + nLineNo, BTN_PRESSED_BDR_COLOR );
	}

	//if( nSplit )
	//{
	//	PenDraw.DeleteObject();
	//	PenDraw.CreatePen( PS_SOLID, 1, BTN_PRESSED_BDR_COLOR);
	//	DrawDC.MoveTo(nRight - SPLIT_WIDTH, rcBtn.top);
	//	DrawDC.LineTo(nRight - SPLIT_WIDTH, rcBtn.bottom);
	//}
	DrawDC.SelectPen(OldPen);
	return dwLineColor;
}

DWORD DrawPressedSplitBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, const CRect &rcBtn, const CSize szRound )
{
	//ATLTRACE("Enter DrawPressedBtnLine\n");
	DWORD dwLeftLineColor;
	DWORD dwRightLineColor;
	INT32 nGradientLns = rcBtn.Height() * GRADIENT_LINE_REGION;
	INT32 nLeft;
	INT32 nRight;
	DOUBLE dRoundScale = szRound.cy / szRound.cx;
	DWORD dwRightBdrInnerColor;
	DWORD dwRightBdrOuterColor;
	DWORD dwRightBdrCenterColor;
	DWORD dwLeftBdrInnerColor;
	DWORD dwLeftBdrOuterColor;
	DWORD dwLeftBdrCenterColor;
	INT32 nRoundLen = 0; 
	INT32 nHeight;
	nHeight = rcBtn.Height();
	INT32 nReverseLine = nHeight - nLineNo - 1;

	if( nLineNo < szRound.cy)
	{
		nRoundLen = ( INT32 )( ( szRound.cy - nLineNo )* dRoundScale );

	}
	else if( nReverseLine < szRound.cy)
	{
		nRoundLen = ( INT32 )( (szRound.cy - (nReverseLine)) * dRoundScale );
	}
	
	if(nLineNo == 0)
	{
		dwLeftLineColor = BTN_HOT_BDR_COLOR;
		dwRightLineColor = BTN_PRESSED_BDR_COLOR;
	}
	else if(nLineNo < nGradientLns && nLineNo > 0 )
	{
		dwLeftLineColor = GetLinearColor(nGradientLns - nLineNo, BTN_HOT_BK_COLOR, LINE_COLOR_DECREMENT * 5);
		dwRightLineColor = GetLinearColor(nGradientLns - nLineNo, BTN_PRESSED_BK_COLOR, LINE_COLOR_DECREMENT * 5);
	}
	else if( nLineNo >= nGradientLns && (nReverseLine)>= nGradientLns )
	{
		dwLeftLineColor = GetLinearColor(nLineNo - nGradientLns, BTN_HOT_BK_COLOR, LINE_COLOR_DECREMENT);
		dwRightLineColor = GetLinearColor(nLineNo - nGradientLns, BTN_PRESSED_BK_COLOR, LINE_COLOR_DECREMENT);
	}
	else if ( (nReverseLine) < nGradientLns && nLineNo < nHeight - 1)
	{
		dwLeftLineColor = GetLinearColor(nGradientLns - nReverseLine, BTN_HOT_BK_COLOR - ( nHeight - nGradientLns ) * LINE_COLOR_DECREMENT, LINE_COLOR_DECREMENT * 4);
		dwRightLineColor = GetLinearColor(nGradientLns - nReverseLine, BTN_PRESSED_BK_COLOR - ( nHeight - nGradientLns ) * LINE_COLOR_DECREMENT, LINE_COLOR_DECREMENT * 4);
	}
	else if (nLineNo == nHeight - 1)
	{
		dwLeftLineColor = BTN_HOT_BDR_COLOR;
		dwRightLineColor = BTN_PRESSED_BDR_COLOR;
	}

	DWORD dwLeftPixelColor;
	DWORD dwRightPixelColor;
	INT32 nFillPixelLen;
#define BDR_PIXEL_LEN 3
	if(nGradientLns > nRoundLen + BDR_PIXEL_LEN - 1)
	{
		nFillPixelLen = nGradientLns;
	}
	else
	{
		nFillPixelLen = nRoundLen + BDR_PIXEL_LEN - 1;
	}

	CPenHandle OldPen;
	nLeft = rcBtn.left;
	nRight = rcBtn.right;
//#define CALC_COLOR_PART(color, part) (((DWORD)(color & 0xff)) / part ) | (((((DWORD)(color & 0xff00)) >> 8) / part) << 8) | (((((DWORD)(color & 0xff0000)) >> 16 ) / part ) << 16)
	dwRightBdrInnerColor = BTN_PRESSED_BDR_COLOR + GetTrendColor( 15, 1, 1, 1 );
	dwRightBdrCenterColor = BTN_PRESSED_BDR_COLOR + GetTrendColor( 20, 1, 1, 1 );
	dwRightBdrOuterColor = BTN_PRESSED_BDR_COLOR + GetTrendColor( 25, 1, 1, 1 ) + GetTrendColor( 2, 0.8, 0.7, 1.1 );

	dwLeftBdrInnerColor = BTN_HOT_BDR_COLOR + GetTrendColor( 15, 1, 1, 1 );
	dwLeftBdrCenterColor = BTN_HOT_BDR_COLOR + GetTrendColor( 20, 1, 1, 1 );
	dwLeftBdrOuterColor = BTN_HOT_BDR_COLOR + GetTrendColor( 25, 1, 1, 1 ) + GetTrendColor( 2, 0.8, 0.7, 1.1 );

	if(PenDraw.m_hPen != NULL) PenDraw.DeleteObject();
	PenDraw.CreatePen(PS_SOLID, 1, dwLeftLineColor);
	OldPen = DrawDC.SelectPen(PenDraw);
	DrawDC.MoveTo(nLeft + nFillPixelLen, rcBtn.top + nLineNo);
	DrawDC.LineTo(nRight - SPLIT_WIDTH, rcBtn.top + nLineNo);
	PenDraw.DeleteObject();
	PenDraw.CreatePen(PS_SOLID, 1, dwRightLineColor);
	DrawDC.SelectPen(PenDraw);
	DrawDC.MoveTo(nRight - SPLIT_WIDTH + 1, rcBtn.top + nLineNo);
	DrawDC.LineTo(nRight - nFillPixelLen + 1, rcBtn.top + nLineNo);

	DrawDC.SetPixel( nRight - SPLIT_WIDTH, rcBtn.top + nLineNo, BTN_PRESSED_BDR_COLOR);
	
	INT32 nRoundBeginX;
	nRoundBeginX = nRoundLen - 1;
	if( nRoundBeginX < 0)
	{
		nRoundBeginX = 0;
	}

	if( nLineNo < szRound.cy || nReverseLine < szRound.cy )
	{
		for(INT32 i = 0; i < nFillPixelLen; i++)
		{
			if( i >= nRoundBeginX && i < nRoundBeginX + BDR_PIXEL_LEN)
			{
				if( i == nRoundBeginX)
				{
					dwLeftPixelColor = dwLeftBdrOuterColor;
					dwRightPixelColor = dwRightBdrOuterColor;
				}
				else if(i == nRoundBeginX + 1)
				{
					dwLeftPixelColor = dwLeftBdrCenterColor;
					dwRightPixelColor = dwRightBdrCenterColor;
				}
				else if(i == nRoundBeginX + 2)
				{
					dwLeftPixelColor = dwLeftBdrInnerColor;
					dwRightPixelColor = dwRightBdrInnerColor;
				}
				else
				{
					ATLASSERT(FALSE);
				}
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwLeftPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwRightPixelColor );
			}
			else if( i < nGradientLns && i >= nRoundLen + 3)
			{
				dwLeftPixelColor = GetLinearColor(nGradientLns - i, dwLeftLineColor, LINE_COLOR_DECREMENT * 4);
				dwRightPixelColor = GetLinearColor(nGradientLns - i , dwRightLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwLeftPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwRightPixelColor );
			}
		}
	}
	else
	{
		for(INT32 i = 1; i < nFillPixelLen; i++)
		{
			if( i < nGradientLns )
			{
				dwLeftPixelColor = GetLinearColor(nGradientLns - i, dwLeftLineColor, LINE_COLOR_DECREMENT * 4);
				dwRightPixelColor = GetLinearColor(nGradientLns - i, dwRightLineColor, LINE_COLOR_DECREMENT * 4);
				DrawDC.SetPixel(nLeft + i, rcBtn.top + nLineNo, dwLeftPixelColor );
				DrawDC.SetPixel(nRight - i, rcBtn.top + nLineNo, dwRightPixelColor );
			}
		}
		DrawDC.SetPixel(nLeft, rcBtn.top + nLineNo, BTN_HOT_BDR_COLOR );
		DrawDC.SetPixel(nRight, rcBtn.top + nLineNo, BTN_PRESSED_BDR_COLOR );
	}

	//if( nSplit )
	//{
	//	PenDraw.DeleteObject();
	//	PenDraw.CreatePen( PS_SOLID, 1, BTN_PRESSED_BDR_COLOR);
	//	DrawDC.MoveTo(nRight - SPLIT_WIDTH, rcBtn.top);
	//	DrawDC.LineTo(nRight - SPLIT_WIDTH, rcBtn.bottom);
	//}
	DrawDC.SelectPen(OldPen);
	return dwLeftLineColor;
}

//DWORD DrawPressedSplitBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, const CRect &rcBtn, const CSize szRound)
//{
//	DWORD dwLineColor;
//	DWORD dwColorDec;
//	CRect rcSplit;
//	CRect rcBtnLeft;
//	CPenHandle OldPen;
//	INT32 nHeight = rcBtn.Height();
//	INT32 nGradientLns = rcBtn.Height() * GRADIENT_LINE_REGION;
//	INT32 nHotGradientLns = rcBtn.Height() * HOT_HOR_GRADIENT_LINE_REGION;
//	rcSplit = rcBtn;
//	rcSplit.left = rcSplit.right - SPLIT_WIDTH;
//	rcBtnLeft = rcBtn;
//	rcBtnLeft.right -= SPLIT_WIDTH;
//	INT32 nReverseLine;
//	nReverseLine = nHeight - nLineNo - 1;
//
//	if(rcBtnLeft.Height() - nLineNo < nGradientLns)
//	{
//		dwColorDec = LINE_COLOR_DECREMENT * 5;
//	}
//	else
//	{
//		dwColorDec = LINE_COLOR_DECREMENT;
//	}
//	dwLineColor = GetLinearColor(nLineNo, BTN_HOT_BK_COLOR, dwColorDec );
//	
//	if(PenDraw.m_hPen != NULL) PenDraw.DeleteObject();
//	PenDraw.CreatePen(PS_SOLID, 1, dwLineColor);
//	OldPen = DrawDC.SelectPen(PenDraw);
//	DrawDC.MoveTo(rcBtnLeft.left + nHotGradientLns, rcBtnLeft.top + nLineNo);
//	DrawDC.LineTo(rcBtnLeft.right, rcBtnLeft.top + nLineNo);
//	DWORD dwPixelColor;
//	for(INT32 i = 0; i < nHotGradientLns; i++)
//	{
//		dwPixelColor = GetLinearColor(nHotGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
//		DrawDC.SetPixel(rcBtn.left + i, rcBtn.top + nLineNo, dwPixelColor );
//	}
//
//	if(nLineNo < nGradientLns )
//	{
//		dwLineColor = GetLinearColor(nGradientLns - nLineNo, BTN_PRESSED_BK_COLOR, LINE_COLOR_DECREMENT * 5);
//	}
//	else if( nLineNo >= nGradientLns && nReverseLine >= nGradientLns )
//	{
//		dwLineColor = GetLinearColor(nLineNo - nGradientLns, BTN_PRESSED_BK_COLOR, LINE_COLOR_DECREMENT);
//	}
//	else
//	{
//		dwLineColor = GetLinearColor(GRADIENT_LINE_REGION - (nHeight - nLineNo), BTN_PRESSED_BK_COLOR - ( nHeight - GRADIENT_LINE_REGION ) * LINE_COLOR_DECREMENT, LINE_COLOR_DECREMENT * 4);
//	}
//
//	PenDraw.DeleteObject();
//	PenDraw.CreatePen(PS_SOLID, 1, dwLineColor);
//	DrawDC.SelectPen(PenDraw);
//	DrawDC.MoveTo(rcBtn.left, rcBtn.top + nLineNo);
//	DrawDC.LineTo(rcBtn.right - nGradientLns, rcBtn.top + nLineNo);
//
//	for(INT32 i = 0; i < nGradientLns; i++)
//	{
//		dwPixelColor = GetLinearColor(nGradientLns - i, dwLineColor, LINE_COLOR_DECREMENT * 4);
//		DrawDC.SetPixel(rcBtn.right - i, rcBtn.top + nLineNo, dwPixelColor );
//	}
//	DrawDC.SelectPen(OldPen);
//	return dwLineColor;
//}

INT32 DrawPressedBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound)
{
	INT32 nHeight;
	nHeight = rcBtn.Height();
	CPen DrawPen;
	for(INT32 i = 0; i < nHeight; i++)
	{
		DrawPressedBtnLine(DrawDC, DrawPen, i, nSplit, rcBtn, szRound);
	}
	return 0;
}

INT32 DrawCheckedBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound)
{
	INT32 nHeight;
	nHeight = rcBtn.Height();
	CPen DrawPen;
	for(INT32 i = 0; i < nHeight; i++)
	{
		DrawCheckedBtnLine(DrawDC, DrawPen, i, nSplit, rcBtn, szRound);
	}
	return 0;
}

INT32 DrawHotLightCheckedBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound)
{
	INT32 nHeight;
	nHeight = rcBtn.Height();
	CPen DrawPen;
	for(INT32 i = 0; i < nHeight; i++)
	{
		DrawCheckedLightBtnLine(DrawDC, DrawPen, i, nSplit, rcBtn, szRound);
	}
	return 0;
}

INT32 DrawPressedSplitBtn(CDC &DrawDC, const CRect &rcBtn, const CSize szRound)
{
	INT32 nHeight;
	nHeight = rcBtn.Height();
	CPen DrawPen;
	for(INT32 i = 0; i < nHeight; i++)
	{
		DrawPressedSplitBtnLine(DrawDC, DrawPen, i, rcBtn, szRound);
	}
	return 0;
}

INT32 DrawHotLightBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound)
{
	INT32 nHeight;
	nHeight = rcBtn.Height();
	CPen DrawPen;
	for(INT32 i = 0; i < nHeight; i++)
	{
		DrawHotLineBtnLine(DrawDC, DrawPen, i, nSplit, rcBtn, szRound);
	}
	return 0;
}

INT32 DrawSeparator(CDC &DrawDC, RECT &rcDraw)
{
	ATLASSERT(DrawDC != NULL);
	ATLASSERT(rcDraw.right >= rcDraw.left && rcDraw.bottom >= rcDraw.top );

	INT32 nXMid;
	CPen PenLine;
	CPenHandle OldPen;
	nXMid = rcDraw.left + ( rcDraw.right - rcDraw.left ) / 2;
	PenLine.CreatePen(PS_SOLID, 1, SEP_LEFT_LINE_CLR);
	ATLASSERT(NULL != PenLine.m_hPen );
	
	OldPen = DrawDC.SelectPen(PenLine);
	//ATLTRACE("Get the old pen, that's type :%d\n", ::GetObjectType(OldPen.m_hPen));
	DrawDC.MoveTo(nXMid, rcDraw.top);
	DrawDC.LineTo(nXMid, rcDraw.bottom);
	PenLine.DeleteObject();
	PenLine.CreatePen(PS_SOLID, 1, SEP_RIGHT_LINE_CLR);
	//DrawDC.SelectPen(OldPen);
	DrawDC.SelectPen(PenLine);
	DrawDC.MoveTo(nXMid + 1, rcDraw.top);
	DrawDC.LineTo(nXMid + 1, rcDraw.bottom);
	DrawDC.SelectPen(OldPen);
	return 0;
}

INT32 DrawSplitter(CDC &DrawDC, RECT &rcDraw)
{
	ATLASSERT(DrawDC != NULL);
	ATLASSERT(rcDraw.right >= rcDraw.left && rcDraw.bottom >= rcDraw.top );

	INT32 nXMid;
	CPen PenLine;
	CPenHandle OldPen;
	nXMid = rcDraw.left + ( rcDraw.right - rcDraw.left ) / 2;
	PenLine.CreatePen(PS_SOLID, 1, SEP_LEFT_LINE_CLR);
	ATLASSERT(NULL != PenLine.m_hPen );
	
	OldPen = DrawDC.SelectPen(PenLine);
	//ATLTRACE("Get the old pen, that's type :%d\n", ::GetObjectType(OldPen.m_hPen));
#define SPLITTER_GRIP_SPACE 3

	for( INT32 i = rcDraw.top + SPLITTER_GRIP_SPACE; i < rcDraw.bottom - SPLITTER_GRIP_SPACE; i += SPLITTER_GRIP_SPACE )
	{
		DrawDC.SetPixel( nXMid - 1, rcDraw.top + i, SEP_LEFT_LINE_CLR );
	}

	DrawDC.MoveTo(nXMid, rcDraw.top);
	DrawDC.LineTo(nXMid, rcDraw.bottom);
	PenLine.DeleteObject();
	PenLine.CreatePen(PS_SOLID, 1, SEP_RIGHT_LINE_CLR);
	//DrawDC.SelectPen(OldPen);
	DrawDC.SelectPen(PenLine);
	DrawDC.MoveTo(nXMid + 1, rcDraw.top);
	DrawDC.LineTo(nXMid + 1, rcDraw.bottom);

	for( INT32 i = rcDraw.top + SPLITTER_GRIP_SPACE; i < rcDraw.bottom - SPLITTER_GRIP_SPACE; i += SPLITTER_GRIP_SPACE )
	{
		DrawDC.SetPixel( nXMid + 2, rcDraw.top + i, SEP_LEFT_LINE_CLR );
	}

	DrawDC.SelectPen(OldPen);
	return 0;
}

INT32 GradientDraw(CDC &DrawDC,RECT &rcDraw, SIZE zRoundSz, COLORREF clrBegin, COLORREF clrEnd)
{
	ATLASSERT(rcDraw.right > rcDraw.left && rcDraw.bottom > rcDraw.top);

	//ATLTRACE("The begin color is %d, the end color is %d\n", clrBegin, clrEnd);

	COLORREF clrTemp;
	clrTemp = clrBegin;
	CPen PenLine;
	CPenHandle OldPen;
	SHORT sRed, sBlue, sGreen, TempRed,TempGreen, TempBlue;
	DOUBLE dRedGrRate, dGreenGrRate, dBlueGrRate;
	SHORT sDestRed, sDestGreen, sDestBlue;
	DOUBLE dRoundRate = (DOUBLE)zRoundSz.cx / zRoundSz.cy;
	INT32 nRoundWidth;
	INT32 nHeight;
	INT32 nDrawedLineNum;
	nHeight = rcDraw.bottom - rcDraw.top;

	sBlue = (clrBegin & 0xff0000) >> 16;
	sGreen = (clrBegin & 0xff00) >> 8;
	sRed  = (clrBegin & 0xff);

	sDestBlue = (clrBegin & 0xff0000) >> 16;
	sDestGreen = (clrBegin & 0xff00) >> 8;
	sDestRed  = (clrBegin & 0xff);

	dRedGrRate = (DOUBLE)(sDestRed - sRed) / nHeight;
	dGreenGrRate = (DOUBLE)(sDestGreen - sGreen) / nHeight ;
	dBlueGrRate = (DOUBLE)(sDestRed - sBlue) / nHeight ;
	nRoundWidth = zRoundSz.cx;

	//DrawDC.MoveTo(rcDraw + nRoundWidth .left, rcDraw.top);
	PenLine.CreatePen(PS_SOLID, 1, clrTemp);
	OldPen = DrawDC.SelectPen(PenLine);

	for(INT32 i = rcDraw.top; i < rcDraw.bottom; i++)
	{
		nDrawedLineNum = i - rcDraw.top;

		if( (rcDraw.bottom - i) <= zRoundSz.cy )
		{
			nRoundWidth = ( INT32 )( zRoundSz.cx - ( rcDraw.bottom - i) * dRoundRate );
		}
		else if( nDrawedLineNum <= zRoundSz.cy )
		{
			nRoundWidth = ( INT32 )( zRoundSz.cx - ( nDrawedLineNum * dRoundRate) );
		}
		else
		{
			nRoundWidth = 0;
		}

		//ATLTRACE("Gradient Draw Line %d\n", i);
		DrawDC.MoveTo(rcDraw.left + nRoundWidth, i);
		DrawDC.LineTo(rcDraw.right - nRoundWidth, i);

		if(PenLine.m_hPen)
		{
			PenLine.DeleteObject();
		}
		
		TempRed = ( SHORT )( sRed + ( nDrawedLineNum * dRedGrRate) );
		TempBlue = ( SHORT )( sBlue + ( nDrawedLineNum * dBlueGrRate) );
		TempGreen = ( SHORT )( sGreen + ( nDrawedLineNum * dGreenGrRate) );

		if(TempRed > 255)
			TempRed = 255;

		if(TempGreen > 255)
			TempGreen = 255;

		if(TempBlue > 255)
			TempBlue = 255;

		PenLine.CreatePen(PS_SOLID, 1, RGB(TempRed, TempGreen, TempBlue));
		(HPEN)DrawDC.SelectPen(PenLine);
	}
	DrawDC.SelectPen(OldPen);
	return 0;
}

INT32 DrawChevronSign(CDC &DrawDC, LONG lBeginX, LONG lBeginY)
{
#define CHEVRON_LINE_PIXELS 2
#define CHEVRON_LINE2_OFFSET (CHEVRON_LINE_PIXELS + 2)
	CPen PenDraw;
	CPenHandle PenOld;
	PenDraw.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	PenOld = DrawDC.SelectPen(PenDraw);
	DrawDC.MoveTo(lBeginX, lBeginY);
	DrawDC.LineTo(lBeginX + CHEVRON_LINE_PIXELS, lBeginY);
	DrawDC.MoveTo(lBeginX + 1, lBeginY + 1);
	DrawDC.LineTo(lBeginX + 1 + CHEVRON_LINE_PIXELS, lBeginY + 1);
	DrawDC.MoveTo(lBeginX + 2, lBeginY + 2);
	DrawDC.LineTo(lBeginX + 2 + CHEVRON_LINE_PIXELS, lBeginY + 2);
	DrawDC.MoveTo(lBeginX + 1, lBeginY + 3);
	DrawDC.LineTo(lBeginX + 1 + CHEVRON_LINE_PIXELS, lBeginY + 3);
	DrawDC.MoveTo(lBeginX, lBeginY + 4);
	DrawDC.LineTo(lBeginX + CHEVRON_LINE_PIXELS, lBeginY + 4);

	lBeginX += CHEVRON_LINE2_OFFSET;
	DrawDC.MoveTo(lBeginX, lBeginY);
	DrawDC.LineTo(lBeginX + CHEVRON_LINE_PIXELS, lBeginY);
	DrawDC.MoveTo(lBeginX + 1, lBeginY + 1);
	DrawDC.LineTo(lBeginX + 1 + CHEVRON_LINE_PIXELS, lBeginY + 1);
	DrawDC.MoveTo(lBeginX + 2, lBeginY + 2);
	DrawDC.LineTo(lBeginX + 2 + CHEVRON_LINE_PIXELS, lBeginY + 2);
	DrawDC.MoveTo(lBeginX + 1, lBeginY + 3);
	DrawDC.LineTo(lBeginX + 1 + CHEVRON_LINE_PIXELS, lBeginY + 3);
	DrawDC.MoveTo(lBeginX, lBeginY + 4);
	DrawDC.LineTo(lBeginX + CHEVRON_LINE_PIXELS, lBeginY + 4);

	DrawDC.SelectPen(PenOld);
	return 0;
}

INT32 DrawDragInsertMark( INT32 nPos, CDCHandle dc)
{
	INT32 nBottom;
	dc.MoveTo( nPos - 1, 0 );
	dc.LineTo( nPos - 1, nBottom );
	dc.MoveTo( nPos, 0 );
	dc.LineTo( nPos, nBottom );
	dc.MoveTo( nPos + 1, 0 );
	dc.LineTo( nPos + 1, nBottom );
	return 0;
}

DWORD DrawDisabledBtnLine(CDC &DrawDC, CPen &PenDraw, INT32 nLineNo, const CRect &rcBtn, const CSize szRound )
{
	return 0;
}

INT32 DrawDisabledBtn(CDC &DrawDC, const CRect &rcBtn, INT32 nSplit, const CSize szRound)
{
	return 0;
}

INT32 DrawDisabledImage( HBITMAP hBmp, HDC hDC, LONG left, LONG top, LONG lWidth, LONG lHeight )
{
	CDC MemDC;
	HBITMAP hOldBmp;
	BITMAP BmpInfo;
	INT32 nWidth;
	INT32 nHeight;
	DWORD PixClr;

#define DISABLED_COLOR RGB( 220, 220, 220 )
	INT32 nRet = ::GetObject(hBmp, sizeof(BITMAP), &BmpInfo);
	MemDC.CreateCompatibleDC( hDC );
	hOldBmp = (HBITMAP)MemDC.SelectBitmap( hBmp );
	nHeight = BmpInfo.bmHeight;
	nWidth = BmpInfo.bmWidth;

	for( INT32 i = 0; i < nHeight; i ++ )
	{
		for( INT32 j = 0; j < nWidth; j++ )
		{
			PixClr = MemDC.GetPixel( i, j );
			//ATLTRACE( _T( "0x%x" ), PixClr );
			if( 0 == PixClr )
			{
				MemDC.SetPixel( i, j, DISABLED_COLOR );
			}
		}
		//ATLTRACE( _T( "\r\n" ) );
	}
	
	BitBlt(hDC, left, top, lWidth, lHeight, MemDC, 0, 0, SRCPAINT );
	return 0;
}