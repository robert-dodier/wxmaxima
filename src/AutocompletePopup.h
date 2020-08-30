// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2009-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//  Copyright (C) 2015-2016 Gunter Königsmann     <wxMaxima@physikbuch.de>
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

/*! The first number that is open for dynamic ID assignment

  If we want to add additional elements to a pop-up this is the
  lowest ID that is guaranteed to be free for this purpose.
 */
#define popid_complete_00 (wxID_HIGHEST + 1000)

/*! \file
  This file declares the class ContentAssistantPopup.

  The content assistant offers more functionality than AutocompletePopup but 
  only works on systems that allow popups to handle key presses.
*/

#ifndef AUTOCOMPLETEPOPUP_H
#define AUTOCOMPLETEPOPUP_H

#include "precomp.h"
#include "Autocomplete.h"
#include "EditorCell.h"
#include <wx/combo.h>
#include <wx/listctrl.h>
//! The maximum number of popup menu entries we show at the same time
#define AC_MENU_LENGTH 25

class AutocompletePopup final : public wxListView, public wxComboPopup
{  
  // Return pointer to the created control
  wxWindow *GetControl() override { return this; }

  // Translate string into a list selection
  void SetStringValue(const wxString& s) override
  {
    int n = wxListView::FindItem(-1,s);
    if (n >= 0 && n < wxListView::GetItemCount() )
      wxListView::Select(n);
  }
  // Get list selection as a string
  wxString GetStringValue() const override
  { return (m_value >= 0) ? wxListView::GetItemText(m_value) : wxString(); }

private:
  struct DonePtr { AutocompletePopup*& observer; ~DonePtr() { observer = nullptr; } };
  int m_value = -1; // current item index
  //! The current string in the autocompletion
  wxString m_partial;

  wxWindow *m_parent = {};
  const DonePtr m_doneptr;
  wxArrayString m_completions;
  AutoComplete *m_autocomplete = {};
  EditorCell *m_editor = {};
  AutoComplete::autoCompletionType m_type;

  //! The position of our pop-up
  wxPoint m_position;
  //! The visible rectangle of the screen
  wxRect m_screenRect;

public:
  //! Define where the popup will appear on Create()
  void SetPosition(wxPoint pos){m_position = pos;}
  //! Create popup control
  bool Create(wxWindow* parent) override;
  ~AutocompletePopup();
  //! Gets the info which keycode the current keypress results in
  void OnChar(wxKeyEvent &event);
  //! Gets the info which key has been pressed with which modifier
  void OnKeyDown(wxKeyEvent &event);

  /*! The constructor of the autocompletion window

    \param parent The parent window
    \param editor The cell that contains the text that is to be completed
    \param autocomplete The autocompletion data
    \param type The type of completion needed
    \param doneptr A pointer that will be set to NULL when the pop-up is destroyed.
  */
  AutocompletePopup(wxWindow *parent, EditorCell *editor, AutoComplete *autocomplete,
                    AutoComplete::autoCompletionType type, AutocompletePopup **doneptr);

  void UpdateResults();

  void OnClick(wxMouseEvent &event);
};

#endif // AUTOCOMPLETEPOPUP_H
