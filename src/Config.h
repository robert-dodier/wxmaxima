// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2012 Doug Ilijev <doug.ilijev@gmail.com>
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/*!
\file 
The configuration storage.

This file contains a configuration storage that can be accessed quite fast and that
knows the default values of everything.
*/

#include <wx/wx.h>
#include <wx/config.h>

#ifndef CONFIG_H
#define CONFIG_H

#include "TextStyle.h"
#include "Setup.h"

class Config
{
public:
  //! Returns a pointer to the only instance of the config object
  static Config *Get() { return m_configInstance;}

  Config()
    {
      m_configInstance = this;
      m_config = wxConfig::Get();
      RevertToDefault();
      Read();
    }

  //! Read the config
  void Read();
  //! Reset all values to their default values
  void RevertToDefault();
  //! Writes the whole config
  void Write();

  bool m_matchParens;
  bool m_savePanes;
  bool m_uncompressedWXMX;
  bool m_fixedFontTC;
  bool m_changeAsterisk;
  bool m_usejsmath;
  bool m_keepPercent;
  bool m_abortOnError;
  bool m_pollStdOut;
  bool m_enterEvaluates;
  bool m_saveUntitled;
  bool m_openHCaret;
  bool m_animateLaTeX;
  bool m_TeXExponentsAfterSubscript;
  bool m_flowedTextRequested;
  bool m_exportInput;
  bool m_exportContainsWXMX;
  bool m_exportWithMathJAX;
  bool m_insertAns;
  bool m_fixReorderedIndices;
  bool m_showUserDefinedLabels;
  bool m_usepngCairo;
  bool m_vcFriendlyWXMX;
  bool m_wxcd;
  wxString m_texPreamble;
  wxString m_documentclass;

  //! The maxima command
  wxString m_mc;
  //! The maxima command's parameters
  wxString m_mp;
  wxString m_mathFontName;

  int m_labelWidth;
  int m_undoLimit;
  int m_showLength;
  int m_bitmapScale;
  int m_defaultFramerate;
  int m_displayedDigits;
  int m_autoSaveInterval;
  int m_defaultPort;
  int m_rs;
  int m_language;
  int m_panelSize;
  // Text in math output
  int m_fontEncoding;
  int m_fontSize;
  int m_mathFontSize;
  int m_defaultPlotWidth;
  int m_defaultPlotHeight;

  // All styles we support
  style m_styleDefault,
    m_styleVariable,
    m_styleFunction,
    m_styleNumber,
    m_styleSpecial,
    m_styleGreek,
    m_styleString,
    m_styleInput,
    m_styleMainPrompt,
    m_styleOtherPrompt,
    m_styleLabel,
    m_styleUserDefinedLabel,
    m_styleHighlight,
    m_styleText,
    m_styleSubsubsection,
    m_styleSubsection,
    m_styleSection,
    m_styleTitle,
    m_styleTextBackground,
    m_styleBackground,
    m_styleCellBracket,
    m_styleActiveCellBracket,
    m_styleCursor,
    m_styleSelection,
    m_styleEqualsSelection,
    m_styleOutdated,
    m_styleCodeHighlightingVariable,
    m_styleCodeHighlightingFunction,
    m_styleCodeHighlightingComment,
    m_styleCodeHighlightingNumber,
    m_styleCodeHighlightingString,
    m_styleCodeHighlightingOperator,
    m_styleCodeHighlightingEndOfLine;

private:
  //! The pointer to the instance of the config object we may have instantiated
  static Config *m_configInstance;
  //! The wxConfig we actually read from and write to
  wxConfigBase *m_config;
};
  
#endif // CONFIG_H
