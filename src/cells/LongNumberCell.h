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

#ifndef LONGNUMBERCELL_H
#define LONGNUMBERCELL_H

#include "TextCell.h"

/*! A cell containing a long number

  A specialised TextCell, that can display a long number, or shorten it using an ellipsis.
 */
class LongNumberCell final : public TextCell
{
public:
  //! The constructor for cell that, if displayed, means that something is amiss
  LongNumberCell(GroupCell *parent, Configuration **config, const wxString &number);
  LongNumberCell(const LongNumberCell &cell);
  std::unique_ptr<Cell> Copy() const override;
  const CellTypeInfo &GetInfo() override;

  void Recalculate(AFontSize fontsize) override;
  void Draw(wxPoint point) override;
  bool NeedsRecalculation(AFontSize fontSize) const override;
  void SetStyle(TextStyle style) override;

protected:
  virtual void UpdateDisplayedText() override;

private:
  //** Large objects (144 bytes)
  //**
  //! The first few digits
  wxString m_numStart;
  //! The "not all digits displayed" message.
  wxString m_ellipsis;
  //! Last few digits (also used for user defined label)
  wxString m_numEnd;

  //** 4-byte objects (12 bytes)
  //**
  int m_numStartWidth = 0;
  int m_ellipsisWidth = 0;
  //! The number of digits we did display the last time we displayed a number.
  int m_displayedDigits_old = -1;

  //** Bitfield objects (0 bytes)
  //**
  void InitBitFields()
  { // Keep the initailization order below same as the order
    // of bit fields in this class!
  }
};

#endif // LONGNUMBERCELL_H
