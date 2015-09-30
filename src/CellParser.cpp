// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
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

#include "CellParser.h"

#include <wx/font.h>
#include "Config.h"
#include "MathCell.h"

CellParser::CellParser(wxDC& dc) : m_dc(dc)
{
  m_scale = 1.0;
  m_zoomFactor = 1.0; // affects returned fontsizes
  m_top = -1;
  m_bottom = -1;
  m_forceUpdate = false;
  m_indent = MC_GROUP_LEFT_INDENT;
  m_changeAsterisk = false;
  m_outdated = false;
  m_TeXFonts = false;

  if (wxFontEnumerator::IsValidFacename(m_fontCMEX = wxT("jsMath-cmex10")) &&
      wxFontEnumerator::IsValidFacename(m_fontCMSY = wxT("jsMath-cmsy10")) &&
      wxFontEnumerator::IsValidFacename(m_fontCMRI = wxT("jsMath-cmr10")) &&
      wxFontEnumerator::IsValidFacename(m_fontCMMI = wxT("jsMath-cmmi10")) &&
      wxFontEnumerator::IsValidFacename(m_fontCMTI = wxT("jsMath-cmti10")))
    m_TeXFonts = Config::Get()->m_usejsmath;

  m_keepPercent =  Config::Get()->m_keepPercent;

  ReadStyle();
}

CellParser::CellParser(wxDC& dc, double scale) : m_dc(dc)
{
  m_scale = scale;
  m_zoomFactor = 1.0; // affects returned fontsizes
  m_top = -1;
  m_bottom = -1;
  m_forceUpdate = false;
  m_indent = MC_GROUP_LEFT_INDENT;
  m_changeAsterisk = false;
  m_outdated = false;
  m_TeXFonts = false;

  if (wxFontEnumerator::IsValidFacename(m_fontCMEX = wxT("jsMath-cmex10")) &&
      wxFontEnumerator::IsValidFacename(m_fontCMSY = wxT("jsMath-cmsy10")) &&
      wxFontEnumerator::IsValidFacename(m_fontCMRI = wxT("jsMath-cmr10")) &&
      wxFontEnumerator::IsValidFacename(m_fontCMMI = wxT("jsMath-cmmi10")) &&
      wxFontEnumerator::IsValidFacename(m_fontCMTI = wxT("jsMath-cmti10")))
    m_TeXFonts = Config::Get()->m_usejsmath;

  m_keepPercent = Config::Get()->m_keepPercent;

  ReadStyle();
}

CellParser::~CellParser()
{}

wxString CellParser::GetFontName(int type)
{
  if (type == TS_TITLE || type == TS_SUBSECTION || type == TS_SUBSUBSECTION || type == TS_SECTION || type == TS_TEXT)
  {
    return m_styles[type].font;
  }
  else if (type == TS_NUMBER || type == TS_VARIABLE || type == TS_FUNCTION ||
      type == TS_SPECIAL_CONSTANT || type == TS_STRING)
  {
    return m_mathFontName;
  }
  return m_fontName;
}

void CellParser::ReadStyle()
{
  // Font
  m_fontName = Config::Get()->m_styleDefault.font;
  // Default fontsize
  m_defaultFontSize = Config::Get()->m_fontSize;
  m_mathFontSize = Config::Get()->m_mathFontSize;

  // Encoding - used only for comments

  m_fontEncoding = (wxFontEncoding)Config::Get()->m_fontEncoding;

  // Math font
  m_mathFontName = Config::Get()-> m_mathFontName;

  wxString tmp;

  // Normal text
  m_styles[TS_DEFAULT] = Config::Get() -> m_styleDefault;

  // Text
  m_styles[TS_TEXT]    = Config::Get() -> m_styleText;
  
  // Variables in highlighted code
  m_styles[TS_CODE_VARIABLE] = Config::Get() -> m_styleCodeHighlightingVariable;

  // Keywords in highlighted code
  m_styles[TS_CODE_FUNCTION] = Config::Get() -> m_styleCodeHighlightingFunction;

  // Comments in highlighted code
  m_styles[TS_CODE_COMMENT] =  Config::Get() -> m_styleCodeHighlightingComment;

  // Numbers in highlighted code
  m_styles[TS_CODE_NUMBER]  = Config::Get()  -> m_styleCodeHighlightingNumber;

  // Strings in highlighted code
  m_styles[TS_CODE_STRING]  = Config::Get()  -> m_styleCodeHighlightingString;

  // Operators in highlighted code
  m_styles[TS_CODE_OPERATOR] = Config::Get() -> m_styleCodeHighlightingOperator;
    
  // Line endings in highlighted code
  m_styles[TS_CODE_ENDOFLINE] = Config::Get()-> m_styleCodeHighlightingEndOfLine;
    
  // Subsubsection
  m_styles[TS_SUBSUBSECTION]  = Config::Get()-> m_styleSubsubsection;

  // Subsection
  m_styles[TS_SUBSECTION]  =    Config::Get()-> m_styleSubsection;

  // Section
  m_styles[TS_SECTION]     =    Config::Get()-> m_styleSection;

  // Title
  m_styles[TS_TITLE]       =    Config::Get()-> m_styleTitle;

  // Main prompt
  m_styles[TS_MAIN_PROMPT]  =    Config::Get()-> m_styleMainPrompt;

  // Other prompt
  m_styles[TS_OTHER_PROMPT]  =    Config::Get()-> m_styleOtherPrompt;

  // Labels
  m_styles[TS_LABEL]         =    Config::Get()-> m_styleLabel;

  // User-defined Labels
  m_styles[TS_USERLABEL]     =    Config::Get()-> m_styleUserDefinedLabel;

  // Special constant
  m_styles[TS_SPECIAL_CONSTANT] = Config::Get()-> m_styleSpecial;

  // Input
  m_styles[TS_INPUT] = Config::Get()-> m_styleInput;

  // Number
  m_styles[TS_NUMBER] = Config::Get()-> m_styleNumber;

  // String
  m_styles[TS_STRING] = Config::Get()-> m_styleString;

  // Greek
  m_styles[TS_GREEK_CONSTANT] = Config::Get()-> m_styleGreek;

  // Variables
  m_styles[TS_VARIABLE] = Config::Get()-> m_styleVariable;

  // FUNCTIONS
  m_styles[TS_FUNCTION] = Config::Get()-> m_styleFunction;

  // Highlight
  m_styles[TS_HIGHLIGHT] = Config::Get()-> m_styleHighlight;

  // Text background
  m_styles[TS_TEXT_BACKGROUND] = Config::Get()-> m_styleTextBackground;
  
  // Cell bracket colors
  m_styles[TS_CELL_BRACKET] = Config::Get()-> m_styleCellBracket;

  m_styles[TS_ACTIVE_CELL_BRACKET] = Config::Get()-> m_styleActiveCellBracket;

  // Cursor (hcaret in MathCtrl and caret in EditorCell)
  m_styles[TS_CURSOR] = Config::Get()-> m_styleCursor;

  // Selection color defaults to light grey on windows
  m_styles[TS_SELECTION] = Config::Get()-> m_styleSelection;
  m_styles[TS_EQUALSSELECTION] = Config::Get()->m_styleEqualsSelection;

  // Outdated cells
  m_styles[TS_OUTDATED] = Config::Get()-> m_styleOutdated;

  m_dc.SetPen(*(wxThePenList->FindOrCreatePen(m_styles[TS_DEFAULT].color, 1, wxPENSTYLE_SOLID)));
}

wxFontWeight CellParser::IsBold(int st)
{
  if (m_styles[st].bold)
    return wxFONTWEIGHT_BOLD;
  return wxFONTWEIGHT_NORMAL;
}

wxFontStyle CellParser::IsItalic(int st)
{
  if (m_styles[st].italic)
    return wxFONTSTYLE_SLANT;
  return wxFONTSTYLE_NORMAL;
}

bool CellParser::IsUnderlined(int st)
{
  return m_styles[st].underlined;
}

wxString CellParser::GetSymbolFontName()
{
#if defined __WXMSW__
  return wxT("Symbol");
#endif
  return m_fontName;
}

wxColour CellParser::GetColor(int st)
{
  if (m_outdated)
    return m_styles[TS_OUTDATED].color;
  return m_styles[st].color;
}

/*
wxFontEncoding CellParser::GetGreekFontEncoding()
{
#if wxUSE_UNICODE || defined (__WXGTK20__) || defined (__WXMAC__)
  return wxFONTENCODING_DEFAULT;
#elif defined __WXMSW__
  return wxFONTENCODING_CP1253;
#else
  return wxFONTENCODING_ISO8859_7;
#endif
}
*/
