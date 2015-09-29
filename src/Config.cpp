// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2008-2009 Ziga Lenarcic <zigalenarcic@users.sourceforge.net>
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

/*\file

  The C code for the preferences dialog.
*/

#include "Config.h"
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/font.h>
#include <wx/settings.h>

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)>(b) ? (b) : (a))

/*! The enum that chooses the language in the language drop-down menu. 

  \attention
  - Should match whatever is put in the m_language.
*/
const int langs[] =
{
  wxLANGUAGE_DEFAULT,
  wxLANGUAGE_CATALAN,
  wxLANGUAGE_CHINESE_SIMPLIFIED,
  wxLANGUAGE_CHINESE_TRADITIONAL,
  wxLANGUAGE_CZECH,
  wxLANGUAGE_DANISH,
  wxLANGUAGE_ENGLISH,
  wxLANGUAGE_FRENCH,
  wxLANGUAGE_GALICIAN,
  wxLANGUAGE_GERMAN,
  wxLANGUAGE_GREEK,
  wxLANGUAGE_HUNGARIAN,
  wxLANGUAGE_ITALIAN,
  wxLANGUAGE_JAPANESE,
  wxLANGUAGE_NORWEGIAN_BOKMAL,
  wxLANGUAGE_POLISH,
  wxLANGUAGE_PORTUGUESE_BRAZILIAN,
  wxLANGUAGE_RUSSIAN,
  wxLANGUAGE_SPANISH,
  wxLANGUAGE_TURKISH,
  wxLANGUAGE_UKRAINIAN
};

#define LANGUAGE_NUMBER sizeof(langs)/(signed)sizeof(langs[1])

void Config::RevertToDefault()
{

  // The default values for all config items that will be used if there is no saved
  // configuration data for this item.
  m_matchParens = true;
  m_savePanes = true;
  m_rs  = true;
  m_uncompressedWXMX=true;
  m_fixedFontTC = true;
  m_changeAsterisk = false;
  m_usejsmath = true;
  m_keepPercent = true;
  m_abortOnError = true;
  m_pollStdOut = false;
  m_enterEvaluates = false;
  m_saveUntitled = true;
  m_openHCaret = false;
  m_AnimateLaTeX = true;
  m_TeXExponentsAfterSubscript=false;
  m_flowedTextRequested = true;
  m_exportInput = true;
  m_exportContainsWXMX = false;
  m_exportWithMathJAX = true;
  m_insertAns = true;
  m_fixReorderedIndices = false;
  m_showUserDefinedLabels = true;
# if defined (__WXMAC__)
  m_usepngCairo=false;
# else
  m_usepngCairo=true;
# endif

  
  m_labelWidth = 4;
  m_undoLimit = 0;
  m_showLength = 0;
  m_bitmapScale = 3;
  m_defaultFramerate = 2;
  m_displayedDigits = 100;
  m_autoSaveInterval = 0;
  m_defaultPort = 4010;
  m_language = wxLANGUAGE_UNKNOWN;
  m_panelSize = 1;
  m_fontEncoding = wxFONTENCODING_DEFAULT;
  m_defaultPlotWidth = 600;
  m_defaultPlotHeight = 400;

  m_texPreamble=wxEmptyString;
  m_documentclass=wxT("article");

  m_fontSize = m_mathFontSize = 12;
  m_mathFontName = wxEmptyString;
  m_styleBackground.color = wxT("white");
  m_styleTextBackground.color = wxT("white");
  m_styleHighlight.color = wxT("red");
  m_styleCellBracket.color = wxT("rgb(0,0,0)");
  m_styleActiveCellBracket.color = wxT("rgb(255,0,0)");
  m_styleCursor.color = wxT("rgb(0,0,0)");
  m_styleOutdated.color = wxT("rgb(153,153,153)");
  // Selection color defaults to light grey on windows
#if defined __WXMSW__
  m_styleSelection.color = wxColour(wxT("light grey"));
#else
  m_styleSelection.color = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
#endif
  m_styleEqualsSelection.color = wxT("rgb(192,192,255)");
  m_styleDefault.color = wxT("black");
  m_styleDefault.bold = false;
  m_styleDefault.italic = false;
  m_styleDefault.underlined = false;
  m_styleMainPrompt.color = wxT("rgb(255,128,128)");
  m_styleMainPrompt.bold = false;
  m_styleMainPrompt.italic = false;
  m_styleMainPrompt.underlined = false;
  m_styleOtherPrompt.color = wxT("red");
  m_styleOtherPrompt.bold = false;
  m_styleOtherPrompt.italic = true;
  m_styleOtherPrompt.underlined = false;
  m_styleLabel.color = wxT("rgb(255,192,128)");
  m_styleLabel.bold = false;
  m_styleLabel.italic = false;
  m_styleLabel.underlined = false;
  m_styleUserDefinedLabel.color = wxT("rgb(255,64,0)");
  m_styleUserDefinedLabel.bold = false;
  m_styleUserDefinedLabel.italic = false;
  m_styleUserDefinedLabel.underlined = false;
  m_styleSpecial.color = m_styleDefault.color;
  m_styleSpecial.bold = false;
  m_styleSpecial.italic = false;
  m_styleSpecial.underlined = false;
  m_styleInput.color = wxT("blue");
  m_styleInput.bold = false;
  m_styleInput.italic = false;
  m_styleInput.underlined = false;
  m_styleNumber.color = m_styleDefault.color;
  m_styleNumber.bold = false;
  m_styleNumber.italic = false;
  m_styleNumber.underlined = false;
  m_styleString.color = m_styleDefault.color;
  m_styleString.bold = false;
  m_styleString.italic = true;
  m_styleString.underlined = false;
  m_styleString.color = m_styleDefault.color;
  m_styleString.bold = false;
  m_styleString.italic = true;
  m_styleString.underlined = false;
  m_styleGreek.color = m_styleDefault.color;
  m_styleGreek.bold = false;
  m_styleGreek.italic = false;
  m_styleGreek.underlined = false;
  m_styleVariable.color = m_styleDefault.color;
  m_styleVariable.bold = false;
  m_styleVariable.italic = true;
  m_styleVariable.underlined = false;
  m_styleFunction.color = m_styleDefault.color;
  m_styleFunction.bold = false;
  m_styleFunction.italic = false;
  m_styleFunction.underlined = false;
  m_styleText.color = wxT("black");
  m_styleText.bold = true;
  m_styleText.italic = false;
  m_styleText.underlined = false;
  m_styleText.font = m_styleDefault.font;
  m_styleText.fontSize = m_fontSize;

  // Variables in highlighted code
  m_styleCodeHighlightingVariable.color = wxT("rgb(0,128,0)");
  m_styleCodeHighlightingVariable.bold = false;
  m_styleCodeHighlightingVariable.italic = true;
  m_styleCodeHighlightingVariable.underlined = false;

  // Functions in highlighted code
  m_styleCodeHighlightingFunction.color = wxT("rgb(128,0,0)");
  m_styleCodeHighlightingFunction.bold = false;
  m_styleCodeHighlightingFunction.italic = true;
  m_styleCodeHighlightingFunction.underlined = false;

  // Comments in highlighted code
  m_styleCodeHighlightingComment.color = wxT("rgb(64,64,64)");
  m_styleCodeHighlightingComment.bold = false;
  m_styleCodeHighlightingComment.italic = true;
  m_styleCodeHighlightingComment.underlined = false;

  // Numbers in highlighted code
  m_styleCodeHighlightingNumber.color = wxT("rgb(128,64,0)");
  m_styleCodeHighlightingNumber.bold = false;
  m_styleCodeHighlightingNumber.italic = true;
  m_styleCodeHighlightingNumber.underlined = false;

  // Strings in highlighted code
  m_styleCodeHighlightingString.color = wxT("rgb(0,0,128)");
  m_styleCodeHighlightingString.bold = false;
  m_styleCodeHighlightingString.italic = true;
  m_styleCodeHighlightingString.underlined = false;

  // Operators in highlighted code
  m_styleCodeHighlightingOperator.color = wxT("rgb(0,0,128)");
  m_styleCodeHighlightingOperator.bold = false;
  m_styleCodeHighlightingOperator.italic = true;
  m_styleCodeHighlightingOperator.underlined = false;
  // Line endings in highlighted code
  m_styleCodeHighlightingEndOfLine.color = wxT("rgb(128,128,128)");
  m_styleCodeHighlightingEndOfLine.bold = false;
  m_styleCodeHighlightingEndOfLine.italic = true;
  m_styleCodeHighlightingEndOfLine.underlined = false;

  // Subsubsection
  m_styleSubsubsection.color = wxT("black");
  m_styleSubsubsection.bold = true;
  m_styleSubsubsection.italic = false;
  m_styleSubsubsection.underlined = false;
  m_styleSubsubsection.font = m_styleDefault.font;
  m_styleSubsubsection.fontSize = 14;
  // Subsection
  m_styleSubsection.color = wxT("black");
  m_styleSubsection.bold = true;
  m_styleSubsection.italic = false;
  m_styleSubsection.underlined = false;
  m_styleSubsection.font = m_styleDefault.font;
  m_styleSubsection.fontSize = 16;
  // Section
  m_styleSection.color = wxT("black");
  m_styleSection.bold = true;
  m_styleSection.italic = true;
  m_styleSection.underlined = false;
  m_styleSection.font = m_styleDefault.font;
  m_styleSection.fontSize = 18;
  // Title
  m_styleTitle.color = wxT("black");
  m_styleTitle.bold = true;
  m_styleTitle.italic = false;
  m_styleTitle.underlined = true;
  m_styleTitle.font = m_styleDefault.font;
  m_styleTitle.fontSize = 24;

  m_mp = wxEmptyString;
  m_mc = wxEmptyString;
}

void Config::Read()
{
  m_config->Read(wxT("maxima"), &m_mp);
  m_config->Read(wxT("parameters"), &m_mc);
  m_config->Read(wxT("AUI/savePanes"), &m_savePanes);
  m_config->Read(wxT("usepngCairo"), &m_usepngCairo);
  m_config->Read(wxT("DefaultFramerate"), &m_defaultFramerate);
  m_config->Read(wxT("defaultPlotWidth"), &m_defaultPlotWidth);
  m_config->Read(wxT("defaultPlotHeight"), &m_defaultPlotHeight);
  m_config->Read(wxT("displayedDigits"), &m_displayedDigits);
  m_config->Read(wxT("OptimizeForVersionControl"), &m_uncompressedWXMX);
  m_config->Read(wxT("AnimateLaTeX"), &m_AnimateLaTeX);
  m_config->Read(wxT("TeXExponentsAfterSubscript"), &m_TeXExponentsAfterSubscript);
  m_config->Read(wxT("flowedTextRequested"), &m_flowedTextRequested);
  m_config->Read(wxT("exportInput"), &m_exportInput);
  m_config->Read(wxT("exportContainsWXMX"), &m_exportContainsWXMX);
  m_config->Read(wxT("exportWithMathJAX"), &m_exportWithMathJAX);
  m_config->Read(wxT("pos-restore"), &m_rs);
  m_config->Read(wxT("matchParens"), &m_matchParens);
  m_config->Read(wxT("showLength"), &m_showLength);
  m_config->Read(wxT("language"), &m_language);
  m_config->Read(wxT("documentclass"), &m_documentclass);
  m_config->Read(wxT("pos-restore"), &m_rs);
  m_config->Read(wxT("texPreamble"), &m_texPreamble);
  m_config->Read(wxT("autoSaveInterval"), &m_autoSaveInterval);
  m_config->Read(wxT("changeAsterisk"), &m_changeAsterisk);
  m_config->Read(wxT("fixedFontTC"), &m_fixedFontTC);
  m_config->Read(wxT("panelSize"), &m_panelSize);
  m_config->Read(wxT("enterEvaluates"), &m_enterEvaluates);
  m_config->Read(wxT("saveUntitled"), &m_saveUntitled);
  m_config->Read(wxT("openHCaret"), &m_openHCaret);
  m_config->Read(wxT("insertAns"), &m_insertAns);
  m_config->Read(wxT("labelWidth"), &m_labelWidth);
  m_config->Read(wxT("undoLimit"), &m_undoLimit);
  m_config->Read(wxT("bitmapScale"), &m_bitmapScale);
  m_config->Read(wxT("fixReorderedIndices"), &m_fixReorderedIndices);
  m_config->Read(wxT("showUserDefinedLabels"), &m_showUserDefinedLabels);
  m_config->Read(wxT("usejsmath"), &m_usejsmath);
  m_config->Read(wxT("keepPercent"), &m_keepPercent);
  m_config->Read(wxT("abortOnError"), &m_abortOnError);
  m_config->Read(wxT("pollStdOut"), &m_pollStdOut);
  m_config->Read(wxT("fontsize"), &m_fontSize);
  m_config->Read(wxT("mathfontsize"), &m_mathFontSize);
  m_config->Read(wxT("fontEncoding"), &m_fontEncoding);
  m_config->Read(wxT("Style/Math/fontname"), &m_mathFontName);
  m_config->Read(wxT("Style/Subsection/fontsize"), &m_styleSubsection.fontSize);
  m_config->Read(wxT("Style/Subsection/fontname"), &m_styleSubsection.font);
  m_config->Read(wxT("Style/Background/color"),&m_styleBackground.color);
  m_config->Read(wxT("Style/TextBackground/color"),&m_styleTextBackground.color);
  m_config->Read(wxT("Style/Highlight/color"),&m_styleHighlight.color);
  m_config->Read(wxT("Style/CellBracket/color"),&m_styleCellBracket.color);
  m_config->Read(wxT("Style/ActiveCellBracket/color"),&m_styleActiveCellBracket.color);
  m_config->Read(wxT("Style/Cursor/color"),&m_styleCursor.color);
  m_config->Read(wxT("Style/Outdated/color"),&m_styleOutdated.color);
  m_config->Read(wxT("Style/Selection/color"),&m_styleSelection.color);
  m_config->Read(wxT("Style/EqualsSelection/color"),&m_styleEqualsSelection.color);

#define READ_STYLE(style, where)                                          \
  {                                                                       \
  wxString tmp;                                                           \
  if (m_config->Read(wxT(where "color"), &tmp)) style.color.Set(tmp);     \
  m_config->Read(wxT(where "bold"),                                       \
               &style.bold);                                              \
  m_config->Read(wxT(where "italic"),                                     \
               &style.italic);                                            \
  m_config->Read(wxT(where "underlined"),                                 \
               &style.underlined);                                        \
  }
  
  READ_STYLE(m_styleString, "Style/String/")
  READ_STYLE(m_styleNumber, "Style/Number/")
  READ_STYLE(m_styleInput, "Style/Input/")
  READ_STYLE(m_styleDefault, "Style/NormalText/")
  READ_STYLE(m_styleMainPrompt, "Style/MainPrompt/")
  READ_STYLE(m_styleOtherPrompt, "Style/OtherPrompt/")
  READ_STYLE(m_styleLabel, "Style/Label/")
  READ_STYLE(m_styleUserDefinedLabel, "Style/UserDefinedLabel/")
  READ_STYLE(m_styleSpecial, "Style/Special/")
  READ_STYLE(m_styleString, "Style/String/Operator")
  READ_STYLE(m_styleGreek, "Style/Greek/")
  READ_STYLE(m_styleVariable, "Style/Variable/")
  READ_STYLE(m_styleFunction, "Style/Function/")
  READ_STYLE(m_styleText, "Style/Text/")
  READ_STYLE(m_styleCodeHighlightingVariable, "Style/CodeHighlighting/Variable/")
  READ_STYLE(m_styleCodeHighlightingFunction, "Style/CodeHighlighting/Function/")
  READ_STYLE(m_styleCodeHighlightingComment, "Style/CodeHighlighting/Comment/")
  READ_STYLE(m_styleCodeHighlightingNumber, "Style/CodeHighlighting/Number/")
  READ_STYLE(m_styleCodeHighlightingString, "Style/CodeHighlighting/String/")
  READ_STYLE(m_styleCodeHighlightingOperator, "Style/CodeHighlighting/Operator/")
  READ_STYLE(m_styleCodeHighlightingEndOfLine, "Style/CodeHighlighting/EndOfLine/")
  READ_STYLE(m_styleSubsubsection, "Style/Subsubsection/")
  READ_STYLE(m_styleSubsection, "Style/Subsection/")
  READ_STYLE(m_styleSection, "Style/Section/")
  READ_STYLE(m_styleTitle, "Style/Title/")
#undef READ_STYLE

  m_config->Read(wxT("Style/fontname"), &m_styleDefault.font);

  m_config->Read(wxT("Style/Text/fontsize"),
               &m_styleText.fontSize);
  m_config->Read(wxT("Style/Text/fontname"),
               &m_styleText.font);

  m_config->Read(wxT("Style/Section/fontsize"),
               &m_styleSection.fontSize);
  m_config->Read(wxT("Style/Section/fontname"),
               &m_styleSection.font);

  m_config->Read(wxT("Style/Subsubsection/fontsize"),
               &m_styleSubsubsection.fontSize);
  m_config->Read(wxT("Style/Subsubsection/fontname"),
               &m_styleSubsubsection.font);

  m_config->Read(wxT("Style/Subsection/fontsize"),
               &m_styleSubsubsection.fontSize);
  m_config->Read(wxT("Style/Subsection/fontname"),
               &m_styleSubsubsection.font);

  m_config->Read(wxT("Style/Title/fontsize"),
               &m_styleTitle.fontSize);
  m_config->Read(wxT("Style/Title/fontname"),
               &m_styleTitle.font);
}


void Config::Write()
{
  m_config->Write(wxT("abortOnError"), m_abortOnError);
  m_config->Write(wxT("pollStdOut"), m_pollStdOut);
  m_config->Write(wxT("maxima"), m_mc);
  m_config->Write(wxT("parameters"), m_mp);
  m_config->Write(wxT("fontSize"), m_fontSize);
  m_config->Write(wxT("mathFontsize"), m_mathFontSize);
  m_config->Write(wxT("matchParens"), m_matchParens);
  m_config->Write(wxT("showLength"), m_showLength);
  m_config->Write(wxT("fixedFontTC"), m_fixedFontTC);
  m_config->Write(wxT("changeAsterisk"), m_changeAsterisk);
  m_config->Write(wxT("enterEvaluates"), m_enterEvaluates);
  m_config->Write(wxT("saveUntitled"), m_saveUntitled);
  m_config->Write(wxT("openHCaret"), m_openHCaret);
  m_config->Write(wxT("insertAns"), m_insertAns);
  m_config->Write(wxT("labelWidth"), m_labelWidth);
  m_config->Write(wxT("undoLimit"), m_undoLimit);
  m_config->Write(wxT("bitmapScale"), m_bitmapScale);
  m_config->Write(wxT("fixReorderedIndices"), m_fixReorderedIndices);
  m_config->Write(wxT("showUserDefinedLabels"), m_showUserDefinedLabels);
  m_config->Write(wxT("defaultPort"), m_defaultPort);
  #ifdef __WXMSW__
  m_config->Write(wxT("wxcd"), m_wxcd);
  #endif
  m_config->Write(wxT("AUI/savePanes"), m_savePanes);
  m_config->Write(wxT("usepngCairo"), m_usepngCairo);
  m_config->Write(wxT("OptimizeForVersionControl"), m_uncompressedWXMX);
  m_config->Write(wxT("DefaultFramerate"), m_defaultFramerate);
  m_config->Write(wxT("defaultPlotWidth"), m_defaultPlotWidth);
  m_config->Write(wxT("defaultPlotHeight"), m_defaultPlotHeight);
  m_config->Write(wxT("displayedDigits"), m_displayedDigits);
  m_config->Write(wxT("AnimateLaTeX"), m_AnimateLaTeX);
  m_config->Write(wxT("TeXExponentsAfterSubscript"), m_TeXExponentsAfterSubscript);
  m_config->Write(wxT("flowedTextRequested"), m_flowedTextRequested);
  m_config->Write(wxT("exportInput"), m_exportInput);
  m_config->Write(wxT("exportContainsWXMX"), m_exportContainsWXMX);
  m_config->Write(wxT("exportWithMathJAX"), m_exportWithMathJAX);
  m_config->Write(wxT("usejsmath"), m_usejsmath);
  m_config->Write(wxT("keepPercent"), m_keepPercent);
  m_config->Write(wxT("texPreamble"), m_texPreamble);
  m_config->Write(wxT("autoSaveInterval"), m_autoSaveInterval);
  m_config->Write(wxT("documentclass"), m_documentclass);
  m_config->Write(wxT("pos-restore"), m_rs);

  m_config->Write(wxT("language"), m_language);

  m_config->Write(wxT("Style/Background/color"),
                m_styleBackground.color.GetAsString(wxC2S_CSS_SYNTAX));
  m_config->Write(wxT("Style/Highlight/color"),
                m_styleHighlight.color.GetAsString(wxC2S_CSS_SYNTAX));
  m_config->Write(wxT("Style/TextBackground/color"),
                m_styleTextBackground.color.GetAsString(wxC2S_CSS_SYNTAX));
  m_config->Write(wxT("Style/CellBracket/color"),
                m_styleCellBracket.color.GetAsString(wxC2S_CSS_SYNTAX));
  m_config->Write(wxT("Style/ActiveCellBracket/color"),
                m_styleActiveCellBracket.color.GetAsString(wxC2S_CSS_SYNTAX));
  m_config->Write(wxT("Style/Cursor/color"),
                m_styleCursor.color.GetAsString(wxC2S_CSS_SYNTAX));
  m_config->Write(wxT("Style/Selection/color"),
                m_styleSelection.color.GetAsString(wxC2S_CSS_SYNTAX));
  m_config->Write(wxT("Style/EqualsSelection/color"),
                m_styleEqualsSelection.color.GetAsString(wxC2S_CSS_SYNTAX));
  m_config->Write(wxT("Style/Outdated/color"),
                m_styleOutdated.color.GetAsString(wxC2S_CSS_SYNTAX));

  m_config->Write(wxT("Style/fontname"), m_styleDefault.font);
  m_config->Write(wxT("fontEncoding"), m_fontEncoding);

  m_config->Write(wxT("Style/Math/fontname"), m_mathFontName);

#define WRITE_STYLE(style, where)                                       \
  m_config->Write(wxT(where "color"), style.color.GetAsString(wxC2S_CSS_SYNTAX)); \
  m_config->Write(wxT(where "bold"), style.bold);                         \
  m_config->Write(wxT(where "italic"), style.italic);                     \
  m_config->Write(wxT(where "underlined"), style.underlined);

  // Normal text
  WRITE_STYLE(m_styleDefault, "Style/NormalText/")

  // Main prompt
  WRITE_STYLE(m_styleMainPrompt, "Style/MainPrompt/")

  // Other prompt
  WRITE_STYLE(m_styleOtherPrompt, "Style/OtherPrompt/")

  // Label
  WRITE_STYLE(m_styleLabel, "Style/Label/")

  // Label
  WRITE_STYLE(m_styleUserDefinedLabel, "Style/UserDefinedLabel/")

  // Special
  WRITE_STYLE(m_styleSpecial, "Style/Special/")

  // Input
  WRITE_STYLE(m_styleInput, "Style/Input/")

  // Number
  WRITE_STYLE(m_styleNumber, "Style/Number/")

  // Greek
  WRITE_STYLE(m_styleGreek, "Style/Greek/")

  // String
  WRITE_STYLE(m_styleString, "Style/String/")

  // Variable
  WRITE_STYLE(m_styleVariable, "Style/Variable/")

  // Text
  m_config->Write(wxT("Style/Text/fontname"), m_styleText.font);
  m_config->Write(wxT("Style/Text/fontsize"), m_styleText.fontSize);
  WRITE_STYLE(m_styleText, "Style/Text/")

  // Syntax highlighting
  WRITE_STYLE(m_styleCodeHighlightingVariable, "Style/CodeHighlighting/Variable/")
  WRITE_STYLE(m_styleCodeHighlightingFunction, "Style/CodeHighlighting/Function/")
  WRITE_STYLE(m_styleCodeHighlightingComment,  "Style/CodeHighlighting/Comment/")
  WRITE_STYLE(m_styleCodeHighlightingNumber,   "Style/CodeHighlighting/Number/")
  WRITE_STYLE(m_styleCodeHighlightingString,   "Style/CodeHighlighting/String/")
  WRITE_STYLE(m_styleCodeHighlightingOperator, "Style/CodeHighlighting/Operator/")
  WRITE_STYLE(m_styleCodeHighlightingEndOfLine,"Style/CodeHighlighting/EndOfLine/")

  // Subsubsection
  m_config->Write(wxT("Style/Subsubsection/fontname"), m_styleSubsubsection.font);
  m_config->Write(wxT("Style/Subsubsection/fontsize"), m_styleSubsubsection.fontSize);
  WRITE_STYLE(m_styleSubsubsection, "Style/Subsubsection/")

  // Subsection
  m_config->Write(wxT("Style/Subsection/fontname"), m_styleSubsection.font);
  m_config->Write(wxT("Style/Subsection/fontsize"), m_styleSubsection.fontSize);
  WRITE_STYLE(m_styleSubsection, "Style/Subsection/")

  // Section
  m_config->Write(wxT("Style/Section/fontname"), m_styleSection.font);
  m_config->Write(wxT("Style/Section/fontsize"), m_styleSection.fontSize);
  WRITE_STYLE(m_styleSection, "Style/Section/")

  // Title
  m_config->Write(wxT("Style/Title/fontname"), m_styleTitle.font);
  m_config->Write(wxT("Style/Title/fontsize"), m_styleTitle.fontSize);
  WRITE_STYLE(m_styleTitle, "Style/Title/")

  // Function names
  WRITE_STYLE(m_styleFunction, "Style/Function/")
  m_config->Flush();
}


Config *Config::m_configInstance;

