// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//                          cw.ahbong        <cw.ahbong@gmail.com>
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

#ifndef PLOTFORMATWIZ_H
#define PLOTFORMATWIZ_H

#include "precomp.h"
#include <wx/wx.h>
#include <wx/statline.h>
#include "Configuration.h"

class PlotFormatWiz : public wxDialog
{
public:
  PlotFormatWiz(
          wxWindow *parent, int id,
          Configuration *cfg,
          const wxString &title,
          const wxPoint &pos = wxDefaultPosition,
          const wxSize &size = wxDefaultSize,
          long style = wxDEFAULT_DIALOG_STYLE);

  void SetValue(wxString s);

  wxString GetValue();

private:
  void set_properties();

  void do_layout();

protected:
  wxStaticText *label_1;
  wxComboBox *combo_box_1;
  wxStaticLine *static_line_1;
  wxButton *button_1;
  wxButton *button_2;
};

#endif // PLOTFORMATWIZ_H
