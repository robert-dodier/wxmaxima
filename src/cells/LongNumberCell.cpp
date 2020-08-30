// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2014-2018 Gunter Königsmann <wxMaxima@physikbuch.de>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//
//  SPDX-License-Identifier: GPL-2.0+

/*! \file
  This file defines the class LongNumberCell

  LongNumberCell is the Cell that is used as a placeholder for items that
  should be overridden, before they are displayed.
 */

#include "LongNumberCell.h"
#include "CellImpl.h"
#include "StringUtils.h"

LongNumberCell::LongNumberCell(GroupCell *parent,
                               Configuration **config,
                               const wxString &number)
  : TextCell(parent, config, number, TS_NUMBER)
{
  InitBitFields();
}

// cppcheck-suppress uninitMemberVar symbolName=LongNumberCell::m_alt
// cppcheck-suppress uninitMemberVar symbolName=LongNumberCell::m_altJs
// cppcheck-suppress uninitMemberVar symbolName=LongNumberCell::m_initialToolTip
LongNumberCell::LongNumberCell(const LongNumberCell &cell):
    TextCell(cell.m_group, cell.m_configuration)
{
}

DEFINE_CELL(LongNumberCell)

void LongNumberCell::UpdateDisplayedText()
{
  unsigned int displayedDigits = (*m_configuration)->GetDisplayedDigits();
  if (m_displayedText.Length() > displayedDigits)
  {
    int left = displayedDigits / 3;
    if (left > 30) left = 30;      
    m_numStart = m_displayedText.Left(left);
    m_ellipsis = wxString::Format(_("[%i digits]"), (int) m_displayedText.Length() - 2 * left);
    m_numEnd = m_displayedText.Right(left);
  }
  else
  {
    m_numStart.clear();
    m_ellipsis.clear();
    m_numEnd.clear();
  }
  m_sizeCache.clear();
  m_displayedDigits_old = (*m_configuration)->GetDisplayedDigits();
  m_textStyle = TS_NUMBER;
}

bool LongNumberCell::NeedsRecalculation(AFontSize fontSize) const
{
  return TextCell::NeedsRecalculation(fontSize) ||
    (m_displayedDigits_old != (*m_configuration)->GetDisplayedDigits());
}

void LongNumberCell::SetStyle(TextStyle style)
{
  wxASSERT (style == TS_NUMBER);
  TextCell::SetStyle(TS_NUMBER);
}

void LongNumberCell::Recalculate(AFontSize fontsize)
{
  // If the config settings about how many digits to display has changed we
  // need to regenerate the info which number to show.
  if ((m_displayedDigits_old != (*m_configuration)->GetDisplayedDigits()))
    UpdateDisplayedText();
  
  if(NeedsRecalculation(fontsize))
  {      
    if(m_numStart.IsEmpty())
      TextCell::Recalculate(fontsize);
    else
    {
      Cell::Recalculate(fontsize);
      m_fontSize = fontsize;
      SetFont(fontsize);
      Configuration *configuration = (*m_configuration);
      wxDC *dc = configuration->GetDC();
      auto numStartSize = CalculateTextSize(dc, m_numStart, numberStart);
      auto ellipsisSize = CalculateTextSize(dc, m_ellipsis, ellipsis);
      auto numEndSize   = CalculateTextSize(dc, m_numEnd,   numberEnd);
      m_numStartWidth = numStartSize.GetWidth();
      m_ellipsisWidth = ellipsisSize.GetWidth();
      m_width = m_numStartWidth + m_ellipsisWidth + numEndSize.GetWidth();
      m_height = wxMax(
        wxMax(numStartSize.GetHeight(), ellipsisSize.GetHeight()), numEndSize.GetHeight());
      m_center = m_height / 2;
    }
  }
}

void LongNumberCell::Draw(wxPoint point)
{
  if((point.x >= 0) && (point.y >= 0))
    SetCurrentPoint(point);
  if (InUpdateRegion())
  {
    if(m_numStart == wxEmptyString)
      TextCell::Draw(point);
    else
    {
      Cell::Draw(point);
      SetForeground();
      SetFont(m_fontSize);
      Configuration *configuration = (*m_configuration);
      wxDC *dc = configuration->GetDC();
      dc->DrawText(m_numStart,
                   point.x + MC_TEXT_PADDING,
                   point.y - m_center + MC_TEXT_PADDING);
      dc->DrawText(m_numEnd,
                   point.x + MC_TEXT_PADDING + m_numStartWidth +
                   m_ellipsisWidth,
                   point.y - m_center + MC_TEXT_PADDING);
      wxColor textColor = dc->GetTextForeground();
      wxColor backgroundColor = dc->GetTextBackground();
      dc->SetTextForeground(
        wxColor(
          (textColor.Red() + backgroundColor.Red()) / 2,
          (textColor.Green() + backgroundColor.Green()) / 2,
          (textColor.Blue() + backgroundColor.Blue()) / 2
          )
        );
      dc->DrawText(m_ellipsis,
                   point.x + MC_TEXT_PADDING + m_numStartWidth,
                   point.y - m_center + MC_TEXT_PADDING);
    }
  }
}

