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

#include "ConfigDialogue.h"
#include "MathCell.h"

#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/font.h>
#include <wx/fontdlg.h>
#include <wx/wfstream.h>
#include <wx/sstream.h>
#include <wx/colordlg.h>
#include <wx/settings.h>
#include "Dirstructure.h"

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

ConfigDialogue::ConfigDialogue(wxWindow* parent)
{
#if defined __WXMAC__
  SetSheetStyle(wxPROPSHEET_BUTTONTOOLBOOK | wxPROPSHEET_SHRINKTOFIT);
#else
  SetSheetStyle(wxPROPSHEET_LISTBOOK);
#endif
  SetSheetInnerBorder(3);
  SetSheetOuterBorder(3);

  Dirstructure dirstruct;
    
#define IMAGE(img) wxImage(dirstruct.ConfigArtDir() + wxT(img))

  wxSize imageSize(32, 32);
  m_imageList = new wxImageList(32, 32);
  m_imageList->Add(IMAGE("editing.png"));
  m_imageList->Add(IMAGE("maxima.png"));
  m_imageList->Add(IMAGE("styles.png"));
  m_imageList->Add(IMAGE("Document-export.png"));
  m_imageList->Add(IMAGE("options.png"));

  Create(parent, wxID_ANY, _("wxMaxima configuration"),
         wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

  m_notebook = GetBookCtrl();

  m_notebook->SetImageList(m_imageList);

  m_notebook->AddPage(CreateWorksheetPanel(), _("Worksheet"), true, 0);
  m_notebook->AddPage(CreateMaximaPanel(), _("Maxima"), false, 1);
  m_notebook->AddPage(CreateStylePanel(), _("Style"), false, 2);
  m_notebook->AddPage(CreateExportPanel(), _("Export"), false, 3);
  m_notebook->AddPage(CreateOptionsPanel(), _("Options"), false, 4);
#ifndef __WXMAC__
  CreateButtons(wxOK | wxCANCEL);
#endif

  LayoutDialog();

  SetProperties();
  UpdateExample();
}

ConfigDialogue::~ConfigDialogue()
{
  if (m_imageList != NULL)
    delete m_imageList;
}

void ConfigDialogue::SetProperties()
{
  SetTitle(_("wxMaxima configuration"));

  m_showUserDefinedLabels->SetToolTip(_("If a command begins with a label followed by a : wxMaxima will show this label instead of the \%o style label maxima has automatically assigned to the same output cell."));
  m_abortOnError->SetToolTip(_("If multiple cells are evaluated in one go: Abort evaluation if wxMaxima detects that maxima has encountered any error."));
  m_pollStdOut->SetToolTip(_("Once the local network link between maxima and wxMaxima has been established maxima has no reason to send any messages using the system's stdout stream so all this stream transport should be a greeting message; The lisp running maxima will send eventual error messages using the system's stderr stream instead. If this box is checked we will nonetheless watch maxima's stdout stream for messages."));
  m_maximaProgram->SetToolTip(_("Enter the path to the Maxima executable."));
  m_additionalParameters->SetToolTip(_("Additional parameters for Maxima"
                                       " (e.g. -l clisp)."));
  m_saveSize->SetToolTip(_("Save wxMaxima window size/position between sessions."));
  m_texPreamble->SetToolTip(_("Additional commands to be added to the preamble of LaTeX output for pdftex."));
  m_autoSaveInterval->SetToolTip(_("If this number of minutes has elapsed after the last save of the file, the file has been given a name (by opening or saving it) and the keyboard has been inactive for > 10 seconds the file is saved. If this number is zero the file isn't saved automatically at all."));
  m_uncomressedWXMX->SetToolTip(_("Don't compress the maxima input text and compress images individually: This enables version control systems like git and svn to effectively spot the differences."));
  m_defaultFramerate->SetToolTip(_("Define the default speed (in frames per second) animations are played back with."));
  m_defaultPlotWidth->SetToolTip(_("The default width for embedded plots. Can be read out or overridden by the maxima variable wxplot_size"));
  m_defaultPlotHeight->SetToolTip(_("The default height for embedded plots. Can be read out or overridden by the maxima variable wxplot_size."));
  m_displayedDigits->SetToolTip(_("If numbers are getting longer than this number of digits they will be displayed abbreviated by an ellipsis."));
  m_AnimateLaTeX->SetToolTip(_("Some PDF viewers are able to display moving images and wxMaxima is able to output them. If this option is selected additional LaTeX packages might be needed in order to compile the output, though."));
  m_TeXExponentsAfterSubscript->SetToolTip(_("In the LaTeX output: Put exponents after an eventual subscript instead of above it. Might increase readability for some fonts and short subscripts."));
  m_flowedTextRequested->SetToolTip(_("While text cells in LaTeX are broken into lines by TeX the text displayed on the screen is broken into lines manually. This option, if set tells that lines in HTML output will be broken where they are broken in the worksheet. If this option isn't set manual linebreaks can still be introduced by introducing an empty line."));
  m_bitmapScale->SetToolTip(_("Normally html expects images to be rather low-res but space saving. These images tend to look rather blurry when viewed on modern screens. Therefore this setting was introduces that selects the factor by which the HTML export increases the resolution in respect to the default value."));
  m_exportInput->SetToolTip(_("Normally we export the whole worksheet to TeX or HTML. But sometimes the maxima input does scare the user. This option turns off exporting of maxima's input."));
  m_exportContainsWXMX->SetToolTip(_("If this option is set the .wxmx source of the current file is copied to a place a link to is put into the result of an export."));
  m_exportWithMathJAX->SetToolTip(_("Use MathJAX instead of images in HTML exports to display maxima output. The advantage of MathJAX is that it allows one to copy the displayed equations as if they were text, to choose if they should be copied as TeX or MathML instead and displays them in a scaleable format that is really nice to look at. The disadvantage of MathJAX is that it will need JavaScript and a little bit of time in order to typeset an equation."));
  m_savePanes->SetToolTip(_("Save panes layout between sessions."));
  m_usepngCairo->SetToolTip(_("The pngCairo terminal offers much better graphics quality (antialiassing and additional line styles). But it will only produce plots if the gnuplot installed on the current system actually supports it."));
  m_matchParens->SetToolTip(_("Write matching parenthesis in text controls."));
  m_showLength->SetToolTip(_("Show long expressions in wxMaxima document."));
  m_language->SetToolTip(_("Language used for wxMaxima GUI."));
  m_documentclass->SetToolTip(_("The document class LaTeX is instructed to use for our documents."));
  m_fixedFontInTC->SetToolTip(_("Set fixed font in text controls."));
  m_getFont->SetToolTip(_("Font used for display in document."));
  m_getMathFont->SetToolTip(_("Font used for displaying math characters in document."));
  m_changeAsterisk->SetToolTip(_("Use centered dot character for multiplication"));
  m_defaultPort->SetToolTip(_("The default port used for communication between Maxima and wxMaxima."));
  m_undoLimit->SetToolTip(_("Save only this number of actions in the undo buffer. 0 means: save an infinite number of actions."));

  #ifdef __WXMSW__
  m_wxcd->SetToolTip(_("Automatically change maxima's working directory to the one the current document is in: "
                       "This is necessary if the document uses File I/O relative to the current directory "
                       "but will make maxima 5.35 fail to find its own installation path when the current "
                       "document resides on a different drive than the maxima installation."));
  #endif
  unsigned int i = 0;
  for (i = 0; i < LANGUAGE_NUMBER; i++)
    if (langs[i] == config->m_language)
      break;
  if (i < LANGUAGE_NUMBER)
    m_language->SetSelection(i);
  else
    m_language->SetSelection(0);

  m_documentclass->SetValue(config->m_documentclass);
  m_texPreamble->SetValue(config->m_texPreamble);
  m_autoSaveInterval->SetValue(config->m_autoSaveInterval);

  Dirstructure dirstruct;

  if (wxFileExists(dirstruct.MaximaDefaultName()))
  {
    m_maximaProgram->SetValue(config->m_dirstruct.MaximaDefaultName());
    m_maximaProgram->Enable(false);
    m_mpBrowse->Enable(false);
  }
  else
  {
    if (mp.Length())
      m_maximaProgram->SetValue(config->m_mp);
    else
      m_maximaProgram->SetValue(config->m_dirstruct.MaximaDefaultName());
  }
  
  m_additionalParameters->SetValue(config->m_mc);
  if (rs == 1)
    m_saveSize->SetValue(config->m_true);
  else
    m_saveSize->SetValue(config->m_false);
  m_savePanes->SetValue(config->m_savePanes);
  m_usepngCairo->SetValue(config->m_usepngCairo);

  m_uncomressedWXMX->SetValue(config->m_UncompressedWXMX);
  m_AnimateLaTeX->SetValue(config->m_AnimateLaTeX);
  m_TeXExponentsAfterSubscript->SetValue(config->m_TeXExponentsAfterSubscript);
  m_flowedTextRequested->SetValue(config->m_flowedTextRequested);
  m_exportInput->SetValue(config->m_exportInput);
  m_exportContainsWXMX->SetValue(config->m_exportContainsWXMX);
  m_exportWithMathJAX->SetValue(config->m_exportWithMathJAX);
  m_matchParens->SetValue(config->m_match);
  m_showLength->SetSelection(showLength);
  m_changeAsterisk->SetValue(config->m_changeAsterisk);
  m_enterEvaluates->SetValue(config->m_enterEvaluates);
  m_saveUntitled->SetValue(config->m_saveUntitled);
  m_openHCaret->SetValue(config->m_openHCaret);
  m_insertAns->SetValue(config->m_insertAns);
  m_labelWidth->SetValue(config->m_labelWidth);
  m_undoLimit->SetValue(config->m_undoLimit);
  m_bitmapScale->SetValue(config->m_bitmapScale);
  m_fixReorderedIndices->SetValue(config->m_fixReorderedIndices);
  m_showUserDefinedLabels->SetValue(config->m_showUserDefinedLabels);
  m_fixedFontInTC->SetValue(config->m_fixedFontTC);
  m_useJSMath->SetValue(config->m_usejsmath);
  m_keepPercentWithSpecials->SetValue(config->m_keepPercent);
  m_abortOnError->SetValue(config->m_abortOnError);
  m_pollStdOut->SetValue(config->m_pollStdOut);
  m_defaultFramerate->SetValue(config->m_defaultFramerate);
  m_defaultPlotWidth->SetValue(config->m_defaultPlotWidth);
  m_defaultPlotHeight->SetValue(config->m_defaultPlotHeight);
  m_displayedDigits->SetValue(config->m_displayedDigits);

  m_getStyleFont->Enable(false);

  if (!wxFontEnumerator::IsValidFacename(wxT("jsMath-cmex10")) ||
      !wxFontEnumerator::IsValidFacename(wxT("jsMath-cmsy10")) ||
      !wxFontEnumerator::IsValidFacename(wxT("jsMath-cmr10")) ||
      !wxFontEnumerator::IsValidFacename(wxT("jsMath-cmmi10")) ||
      !wxFontEnumerator::IsValidFacename(wxT("jsMath-cmti10")))
    m_useJSMath->Enable(false);

  ReadStyles();
}

wxPanel* ConfigDialogue::CreateWorksheetPanel()
{
  wxPanel *panel = new wxPanel(m_notebook, -1);

  wxArrayString showLengths;

  wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(6, 2, 5, 5);
  wxFlexGridSizer* vsizer = new wxFlexGridSizer(16,1,5,5);
  
  wxStaticText* pw = new wxStaticText(panel, -1, _("Default plot size for new maxima sessions"));
  wxBoxSizer *PlotWidthHbox=new wxBoxSizer(wxHORIZONTAL);
  m_defaultPlotWidth=new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 100, 16384);
  PlotWidthHbox->Add(m_defaultPlotWidth,0,wxEXPAND, 0);
  wxStaticText* xx = new wxStaticText(panel, -1, _("x"));
  PlotWidthHbox->Add(xx,0,wxALIGN_CENTER_VERTICAL, 0);
  m_defaultPlotHeight=new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 100, 16384);
  PlotWidthHbox->Add(m_defaultPlotHeight,0,wxEXPAND, 0);
  //  plotWidth->SetSizerAndFit(PlotWidthHbox);
  grid_sizer->Add(pw, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer->Add(PlotWidthHbox,0,wxALL | wxALIGN_CENTER_VERTICAL, 5);

  wxStaticText* dd = new wxStaticText(panel, -1, _("Maximum displayed number of digits:"));
  m_displayedDigits = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 20, INT_MAX);
  grid_sizer->Add(dd, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer->Add(m_displayedDigits,0,wxALL | wxALIGN_CENTER_VERTICAL, 5);

  wxStaticText* sl = new wxStaticText(panel, -1, _("Show long expressions"));
  grid_sizer->Add(sl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  showLengths.Add(_("No"));
  showLengths.Add(_("If not very long"));
  showLengths.Add(_("If not extremely long"));
  showLengths.Add(_("Yes"));
  m_showLength = new wxChoice(panel,-1,wxDefaultPosition,wxDefaultSize,showLengths);
  grid_sizer->Add(m_showLength, 0, wxALL, 5);

  wxStaticText* lw = new wxStaticText(panel, -1, _("Label width"));
  grid_sizer->Add(lw, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  m_labelWidth = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 3, 10);
  grid_sizer->Add(m_labelWidth, 0, wxALL, 5);


  vsizer->Add(grid_sizer, 1, wxEXPAND, 5);

  m_showUserDefinedLabels = new wxCheckBox(panel, -1, _("Show user-defined labels instead of (\%oxx)"));
  vsizer->Add(m_showUserDefinedLabels, 0, wxALL, 5);

  m_matchParens = new wxCheckBox(panel, -1, _("Match parenthesis in text controls"));
  vsizer->Add(m_matchParens, 0, wxALL, 5);

  m_fixedFontInTC = new wxCheckBox(panel, -1, _("Fixed font in text controls"));
  vsizer->Add(m_fixedFontInTC, 0, wxALL, 5);

  m_changeAsterisk = new wxCheckBox(panel, -1, _("Use centered dot character for multiplication"));
  vsizer->Add(m_changeAsterisk, 0, wxALL, 5);

  m_keepPercentWithSpecials = new wxCheckBox(panel, -1, _("Keep percent sign with special symbols: \%e, \%i, etc."));
  vsizer->Add(m_keepPercentWithSpecials, 0, wxALL, 5);

  m_enterEvaluates = new wxCheckBox(panel, -1, _("Enter evaluates cells"));
  vsizer->Add(m_enterEvaluates, 0, wxALL, 5);

  m_openHCaret = new wxCheckBox(panel, -1, _("Open a cell when Maxima expects input"));
  vsizer->Add(m_openHCaret, 0, wxALL, 5);

  m_insertAns = new wxCheckBox(panel, -1, _("Insert \% before an operator at the beginning of a cell"));
  vsizer->Add(m_insertAns, 0, wxALL, 5);

  vsizer->AddGrowableRow(10);
  panel->SetSizer(vsizer);
  vsizer->Fit(panel);

  return panel;
}
  
wxPanel* ConfigDialogue::CreateExportPanel()
{
  wxPanel *panel = new wxPanel(m_notebook, -1);

  wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(4, 2, 5, 5);
  wxFlexGridSizer* vsizer = new wxFlexGridSizer(17,1,5,5);

  wxStaticText *dc = new wxStaticText(panel, -1, _("Documentclass for TeX export:"));
  m_documentclass = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(250, wxDefaultSize.GetY()));
  grid_sizer->Add(dc, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer->Add(m_documentclass, 0, wxALL, 5);

  wxStaticText *tp = new wxStaticText(panel, -1, _("Additional lines for the TeX preamble:"));
  m_texPreamble = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(250, 100), wxTE_MULTILINE | wxHSCROLL);
  grid_sizer->Add(tp, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer->Add(m_texPreamble, 0, wxALL, 5);
  vsizer->Add(grid_sizer, 1, wxEXPAND, 5);

  wxStaticText* bs = new wxStaticText(panel, -1, _("Bitmap scale for export"));
  m_bitmapScale = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 1, 3);
  grid_sizer->Add(bs, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer->Add(m_bitmapScale, 0, wxALL, 5);  

  m_AnimateLaTeX = new wxCheckBox(panel, -1, _("Export animations to TeX (Images only move if the PDF viewer supports this)"));
  vsizer->Add(m_AnimateLaTeX, 0, wxALL, 5);

  m_TeXExponentsAfterSubscript = new wxCheckBox(panel, -1, _("LaTeX: Place exponents after, instead above subscripts"));
  vsizer->Add(m_TeXExponentsAfterSubscript, 0, wxALL, 5);

  m_flowedTextRequested = new wxCheckBox(panel, -1, _("HTML/Text Cells: Export all linebreaks"));
  vsizer->Add(m_flowedTextRequested, 0, wxALL, 5);

  m_exportInput = new wxCheckBox(panel, -1, _("Include input cells in the export of a worksheet"));
  vsizer->Add(m_exportInput, 0, wxALL, 5);

  m_exportContainsWXMX = new wxCheckBox(panel, -1, _("Add the .wxmx file to the HTML export"));
  vsizer->Add(m_exportContainsWXMX, 0, wxALL, 5);

  m_exportWithMathJAX = new wxCheckBox(panel, -1, _("Use MathJAX in HTML export"));
  vsizer->Add(m_exportWithMathJAX, 0, wxALL, 5);
  
  vsizer->AddGrowableRow(10);
  panel->SetSizer(vsizer);
  vsizer->Fit(panel);

  return panel;
}

wxPanel* ConfigDialogue::CreateOptionsPanel()
{
  wxPanel *panel = new wxPanel(m_notebook, -1);

  wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(4, 2, 5, 5);
  wxFlexGridSizer* vsizer = new wxFlexGridSizer(17,1,5,5);

  wxStaticText *lang = new wxStaticText(panel, -1, _("Language:"));
  const wxString m_language_choices[] =
    {
      _("(Use default language)"),
      _("Catalan"),
      _("Chinese Simplified"),
      _("Chinese traditional"),
      _("Czech"),
      _("Danish"),
      _("English"),
      _("French"),
      _("Galician"),
      _("German"),
      _("Greek"),
      _("Hungarian"),
      _("Italian"),
      _("Japanese"),
      _("Norwegian"),
      _("Polish"),
      _("Portuguese (Brazilian)"),
      _("Russian"),
      _("Spanish"),
      _("Turkish"),
      _("Ukrainian")
    };
  m_language = new wxChoice(panel, language_id, wxDefaultPosition, wxSize(230, -1), LANGUAGE_NUMBER, m_language_choices);
  grid_sizer->Add(lang, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer->Add(m_language, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  
  wxStaticText *as = new wxStaticText(panel, -1, _("Autosave interval (minutes, 0 means: off)"));
  m_autoSaveInterval = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(230, -1), wxSP_ARROW_KEYS, 0, 30);
  grid_sizer->Add(as, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer->Add(m_autoSaveInterval, 0, wxALL, 5);

  wxStaticText* ul = new wxStaticText(panel, -1, _("Undo limit (0 for none)"));
  m_undoLimit = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 10000);
  grid_sizer->Add(ul, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer->Add(m_undoLimit, 0, wxALL, 5);

  wxStaticText* df = new wxStaticText(panel, -1, _("Default animation framerate:"));
  m_defaultFramerate = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 1, 200);
  grid_sizer->Add(df, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer->Add(m_defaultFramerate,0,wxALL | wxALIGN_CENTER_VERTICAL, 5);
  vsizer->Add(grid_sizer, 1, wxEXPAND, 5);


  m_saveSize = new wxCheckBox(panel, -1, _("Save wxMaxima window size/position"));
  vsizer->Add(m_saveSize, 0, wxALL, 5);

  m_savePanes = new wxCheckBox(panel, -1, _("Save panes layout"));
  vsizer->Add(m_savePanes, 0, wxALL, 5);

  m_usepngCairo = new wxCheckBox(panel, -1, _("Use cairo to improve plot quality."));
  vsizer->Add(m_usepngCairo, 0, wxALL, 5);

  m_uncomressedWXMX = new wxCheckBox(panel, -1, _("Optimize wxmx files for version control"));
  vsizer->Add(m_uncomressedWXMX, 0, wxALL, 5);

  m_saveUntitled = new wxCheckBox(panel, -1, _("Ask to save untitled documents"));
  vsizer->Add(m_saveUntitled, 0, wxALL, 5);

  m_fixReorderedIndices = new wxCheckBox(panel, -1, _("Fix reordered reference indices (of \%i, \%o) before saving"));
  vsizer->Add(m_fixReorderedIndices, 0, wxALL, 5);

  vsizer->AddGrowableRow(10);
  panel->SetSizer(vsizer);
  vsizer->Fit(panel);

  return panel;
}

wxPanel* ConfigDialogue::CreateMaximaPanel()
{
  wxPanel* panel = new wxPanel(m_notebook, -1);

  wxFlexGridSizer* sizer = new wxFlexGridSizer(10, 2, 0, 0);  

  wxStaticText *mp = new wxStaticText(panel, -1, _("Maxima program:"));
  m_maximaProgram = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(250, -1), wxTE_RICH);
  m_mpBrowse = new wxButton(panel, wxID_OPEN, _("Open"));
  sizer->Add(mp, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  sizer->Add(10, 10);
  sizer->Add(m_maximaProgram, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  sizer->Add(m_mpBrowse, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  wxStaticText *ap = new wxStaticText(panel, -1, _("Additional parameters:"));
  m_additionalParameters = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(250, -1), wxTE_RICH);
  sizer->Add(10, 10);
  sizer->Add(10, 10);
  sizer->Add(ap, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  sizer->Add(10, 10);
  sizer->Add(m_additionalParameters, 0, wxALL, 5);

  int defaultPort = 4010;
  wxConfig::Get()->Read(wxT("defaultPort"), &defaultPort);
  m_defaultPort = new wxSpinCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(230, -1), wxSP_ARROW_KEYS, 50, 5000, defaultPort);
  m_defaultPort->SetValue(config->m_defaultPort);
  wxStaticText* dp = new wxStaticText(panel, -1, _("Default port for communication with wxMaxima:"));
  sizer->Add(10, 10);
  sizer->Add(dp, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  sizer->Add(10, 10);
  sizer->Add(m_defaultPort, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  sizer->Add(10, 10);

  #ifdef __WXMSW__
  bool wxcd = false;  
  wxConfig::Get()->Read(wxT("wxcd"), &wxcd);
  m_wxcd = new wxCheckBox(panel, -1, _("maxima's pwd is path to document"));
  m_wxcd-> SetValue(config->m_wxcd);
  sizer->Add(m_wxcd, 0, wxALL, 5);
  sizer->Add(10, 10);
  #endif

  m_abortOnError = new wxCheckBox(panel, -1, _("Abort evaluation on error"));
  sizer->Add(m_abortOnError,0,wxALL, 5);
  sizer->Add(10,10);
  
  m_pollStdOut = new wxCheckBox(panel, -1, _("Debug: Watch maxima's stdout stream"));
  sizer->Add(m_pollStdOut,0,wxALL, 5);
  sizer->Add(10,10);
  
  panel->SetSizer(sizer);
  sizer->Fit(panel);

  return panel;
}

wxPanel* ConfigDialogue::CreateStylePanel()
{
  wxPanel *panel = new wxPanel(m_notebook, -1);

  wxStaticBox* fonts = new wxStaticBox(panel, -1, _("Fonts"));
  wxStaticBox* styles = new wxStaticBox(panel, -1, _("Styles"));

  wxFlexGridSizer* vsizer = new wxFlexGridSizer(3,1,5,5);
  wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer(3, 2, 2, 2);
  wxStaticBoxSizer* sb_sizer_1 = new wxStaticBoxSizer(fonts, wxVERTICAL);
  wxStaticBoxSizer* sb_sizer_2 = new wxStaticBoxSizer(styles, wxVERTICAL);
  wxBoxSizer* hbox_sizer_1 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* hbox_sizer_2 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* hbox_sizer_3 = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* vbox_sizer = new wxBoxSizer(wxVERTICAL);

  wxStaticText* df = new wxStaticText(panel, -1, _("Default font:"));
  m_getFont = new wxButton(panel, font_family, _("Choose font"), wxDefaultPosition, wxSize(250, -1));
  m_mathFont = new wxStaticText(panel, -1, _("Math font:"));
  m_getMathFont = new wxButton(panel, button_mathFont, _("Choose font"), wxDefaultPosition, wxSize(250, -1));
  m_useJSMath = new wxCheckBox(panel, -1, _("Use jsMath fonts"));
  const wxString m_styleFor_choices[] =
    {
      _("Default"),
      _("Variables"),
      _("Numbers"),
      _("Function names"),
      _("Special constants"),
      _("Greek constants"),
      _("Strings"),
      _("Maxima input"),
      _("Input labels"),
      _("Maxima questions"),
      _("Output labels"),
      _("User-defined labels"),
      _("Highlight (dpart)"),
      _("Text cell"),
      _("Subsubsection cell"),
      _("Subsection cell"),
      _("Section cell"),
      _("Title cell"),
      _("Text cell background"),
      _("Document background"),
      _("Cell bracket"),
      _("Active cell bracket"),
      _("Cursor"),
      _("Selection"),
      _("Text equal to selection"),
      _("Outdated cells"),
      _("Code highlighting: Variables"),
      _("Code highlighting: Functions"),
      _("Code highlighting: Comments"),
      _("Code highlighting: Numbers"),
      _("Code highlighting: Strings"),
      _("Code highlighting: Operators"),
      _("Code highlighting: End of line")
    };

  m_styleFor = new wxListBox(panel, listbox_styleFor, wxDefaultPosition, wxSize(250, -1), 33, m_styleFor_choices, wxLB_SINGLE);
  m_getStyleFont = new wxButton(panel, style_font_family, _("Choose font"), wxDefaultPosition, wxSize(150, -1));
#ifndef __WXMSW__
  m_styleColor = new ColorPanel(this, panel, color_id, wxDefaultPosition, wxSize(150, 30), wxSUNKEN_BORDER | wxFULL_REPAINT_ON_RESIZE);
#else
  m_styleColor = new wxButton(panel, color_id, wxEmptyString, wxDefaultPosition, wxSize(150, -1));
#endif
  m_boldCB = new wxCheckBox(panel, checkbox_bold, _("Bold"));
  m_italicCB = new wxCheckBox(panel, checkbox_italic, _("Italic"));
  m_underlinedCB = new wxCheckBox(panel, checkbox_underlined, _("Underlined"));
  m_examplePanel = new ExamplePanel(panel, -1, wxDefaultPosition, wxSize(250, 60));
  m_loadStyle = new wxButton(panel, load_id, _("Load"));
  m_saveStyle = new wxButton(panel, save_id, _("Save"));

  grid_sizer_1->Add(df, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer_1->Add(m_getFont, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer_1->Add(m_mathFont, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer_1->Add(m_getMathFont, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  grid_sizer_1->Add(10, 10);
  grid_sizer_1->Add(m_useJSMath, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  sb_sizer_1->Add(grid_sizer_1, 1, wxALL | wxEXPAND, 0);
  vsizer->Add(sb_sizer_1, 1, wxALL | wxEXPAND, 3);

  vbox_sizer->Add(m_styleColor, 0, wxALL | wxALIGN_CENTER, 5);
  vbox_sizer->Add(m_getStyleFont, 0, wxALL | wxALIGN_CENTER, 5);
  hbox_sizer_1->Add(m_boldCB, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  hbox_sizer_1->Add(m_italicCB, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  hbox_sizer_1->Add(m_underlinedCB, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  vbox_sizer->Add(hbox_sizer_1, 1, wxALL | wxEXPAND, 0);
  vbox_sizer->Add(m_examplePanel, 0, wxALL | wxEXPAND, 5);
  hbox_sizer_2->Add(m_styleFor, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  hbox_sizer_2->Add(vbox_sizer, 1, wxALL | wxEXPAND, 0);
  sb_sizer_2->Add(hbox_sizer_2, 0, wxALL | wxEXPAND, 0);

  vsizer->Add(sb_sizer_2, 1, wxALL | wxEXPAND, 3);

  // load save buttons
  hbox_sizer_3->Add(m_loadStyle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  hbox_sizer_3->Add(m_saveStyle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  vsizer->Add(hbox_sizer_3, 1, wxALIGN_RIGHT, 3);

  panel->SetSizer(vsizer);
  vsizer->Fit(panel);

  return panel;
}

void ConfigDialogue::OnClose(wxCloseEvent& event)
{
#if defined __WXMAC__
  EndModal(wxID_OK);
#else
  EndModal(wxID_CANCEL);
#endif
}

void ConfigDialogue::WriteSettings()
{
  int i = 0;
  wxString search = wxT("maxima-htmldir");
  wxArrayString out;
  Config *config = Config::Get();
  config->m_abortOnError = m_abortOnError->GetValue());
  config->m_pollStdOut = m_pollStdOut->GetValue());
  config->m_maxima = m_maximaProgram->GetValue());
  config->m_parameters = m_additionalParameters->GetValue());
  config->m_fontSize = m_fontSize);
  config->m_mathFontsize = m_mathFontSize);
  config->m_matchParens = m_matchParens->GetValue());
  config->m_showLength = m_showLength->GetSelection());
  config->m_fixedFontTC = m_fixedFontInTC->GetValue());
  config->m_changeAsterisk = m_changeAsterisk->GetValue());
  config->m_enterEvaluates = m_enterEvaluates->GetValue());
  config->m_saveUntitled = m_saveUntitled->GetValue());
  config->m_openHCaret = m_openHCaret->GetValue());
  config->m_insertAns = m_insertAns->GetValue());
  config->m_labelWidth = m_labelWidth->GetValue());
  config->m_undoLimit = m_undoLimit->GetValue());
  config->m_bitmapScale = m_bitmapScale->GetValue());
  config->m_fixReorderedIndices = m_fixReorderedIndices->GetValue());
  config->m_showUserDefinedLabels = m_showUserDefinedLabels->GetValue());
  config->m_defaultPort = m_defaultPort->GetValue());
  #ifdef __WXMSW__
  config->m_wxcd = m_wxcd->GetValue());
  #endif
  config->m_AUI/savePanes = m_savePanes->GetValue());
  config->m_usepngCairo = m_usepngCairo->GetValue());
  config->m_OptimizeForVersionControl = m_uncomressedWXMX->GetValue());
  config->m_DefaultFramerate = m_defaultFramerate->GetValue());
  config->m_defaultPlotWidth = m_defaultPlotWidth->GetValue());
  config->m_defaultPlotHeight = m_defaultPlotHeight->GetValue());
  config->m_displayedDigits = m_displayedDigits->GetValue());
  config->m_AnimateLaTeX = m_AnimateLaTeX->GetValue());
  config->m_TeXExponentsAfterSubscript = m_TeXExponentsAfterSubscript->GetValue());
  config->m_flowedTextRequested = m_flowedTextRequested->GetValue());
  config->m_exportInput = m_exportInput->GetValue());
  config->m_exportContainsWXMX = m_exportContainsWXMX->GetValue());
  config->m_exportWithMathJAX = m_exportWithMathJAX->GetValue());
  config->m_usejsmath = m_useJSMath->GetValue());
  config->m_keepPercent = m_keepPercentWithSpecials->GetValue());
  config->m_texPreamble = m_texPreamble->GetValue());
  config->m_autoSaveInterval = m_autoSaveInterval->GetValue());
  config->m_documentclass = m_documentclass->GetValue());
  if (m_saveSize->GetValue())
    config->m_saveSize = 1;
  else
    config->m_saveSize = 0;
  i = m_language->GetSelection();
  if (i > -1 && i < LANGUAGE_NUMBER)
    config->Write(wxT("language"), langs[i]);

  WriteStyles();
}

void ConfigDialogue::OnMpBrowse(wxCommandEvent& event)
{
  wxConfig *config = (wxConfig *)wxConfig::Get();
  wxString dd;
  config->Read(wxT("maxima"), &dd);
#if defined __WXMSW__
  wxString file = wxFileSelector(_("Select Maxima program"),
                                 wxPathOnly(dd), wxFileNameFromPath(dd),
                                 wxEmptyString, _("Bat files (*.bat)|*.bat|All|*"),
                                 wxFD_OPEN);
#else
  wxString file = wxFileSelector(_("Select Maxima program"),
                                 wxPathOnly(dd), wxFileNameFromPath(dd),
                                 wxEmptyString, _("All|*"),
                                 wxFD_OPEN);
#endif

  if (file.Length())
  {
    if (file.Right(8) == wxT("wxmaxima") || file.Right(12) == wxT("wxmaxima.exe") ||
        file.Right(12) == wxT("wxMaxima.exe"))
      wxMessageBox(_("Invalid entry for Maxima program.\n\nPlease enter the path to Maxima program again."),
                   _("Error"),
                   wxOK|wxICON_ERROR);
    else
      m_maximaProgram->SetValue(config->m_file);
  }
}

void ConfigDialogue::OnMathBrowse(wxCommandEvent& event)
{
  wxFont math;
  math = wxGetFontFromUser(this, wxFont(m_mathFontSize, wxFONTFAMILY_DEFAULT,
                                        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
                                        false, m_mathFontName));

  if (math.Ok())
  {
    m_mathFontName = math.GetFaceName();
    m_mathFontSize = math.GetPointSize();
    m_getMathFont->SetLabel(m_mathFontName + wxString::Format(wxT(" (%d)"), m_mathFontSize));
    UpdateExample();
  }
}

void ConfigDialogue::OnChangeFontFamily(wxCommandEvent& event)
{
  wxFont font;
  int fontsize = m_fontSize;
  style *tmp = GetStylePointer();
  wxString fontName;

  if (tmp == &m_styleText || tmp == &m_styleTitle || tmp == &m_styleSubsubsection || tmp == &m_styleSubsection || tmp == &m_styleSection)
  {
    if (tmp->fontSize != 0)
      fontsize = tmp->fontSize;
    fontName = tmp->font;
  }
  else
    fontName = m_styleDefault.font;

  font = wxGetFontFromUser(this, wxFont(fontsize,
                                        wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                                        wxFONTWEIGHT_NORMAL,
                                        false, fontName,
                                        m_fontEncoding));
  if (font.Ok())
  {
    if (event.GetId() == font_family)
    {
      m_styleDefault.font = font.GetFaceName();
      m_fontEncoding = font.GetEncoding();
      m_fontSize = font.GetPointSize();
      m_fontSize = MIN(m_fontSize, MC_MAX_SIZE);
      m_fontSize = MAX(m_fontSize, MC_MIN_SIZE);
      m_getFont->SetLabel(m_styleDefault.font + wxString::Format(wxT(" (%d)"), m_fontSize));
    }
    else
    {
      tmp->font = font.GetFaceName();
      tmp->fontSize = font.GetPointSize();
      tmp->fontSize = MAX(tmp->fontSize, MC_MIN_SIZE);
    }
    UpdateExample();
  }
}

void ConfigDialogue::ReadStyles(wxString file)
{
  // Set values in dialog
  m_styleFor->SetSelection(0);
  m_styleColor->SetBackgroundColour(config->m_styleDefault.color); // color the panel, after the styles are loaded
  m_boldCB->SetValue(config->m_m_styleDefault.bold);
  m_italicCB->SetValue(config->m_m_styleDefault.italic);
  m_underlinedCB->SetValue(config->m_m_styleDefault.underlined);
}

void ConfigDialogue::WriteStyles(wxString file)
{
}

void ConfigDialogue::OnChangeColor()
{
  style* tmp = GetStylePointer();
  wxColour col = wxGetColourFromUser(this, tmp->color);
  if (col.IsOk())
  {
    tmp->color = col.GetAsString(wxC2S_CSS_SYNTAX);
    UpdateExample();
    m_styleColor->SetBackgroundColour(tmp->color);
  }
}

void ConfigDialogue::OnChangeStyle(wxCommandEvent& event)
{
  style* tmp = GetStylePointer();
  int st = m_styleFor->GetSelection();

  m_styleColor->SetBackgroundColour(tmp->color);

  if (st >= TS_TEXT_BACKGROUND && st <= TS_SUBSUBSECTION)
    m_getStyleFont->Enable(true);
  else
    m_getStyleFont->Enable(false);

  // Background color only
  if (st >= TS_SECTION)
  {
    m_boldCB->Enable(false);
    m_italicCB->Enable(false);
    m_underlinedCB->Enable(false);
  }
  else
  {
    if(st>TS_OUTDATED)
    {
      m_boldCB->Enable(false);
      m_italicCB->Enable(false);
      m_underlinedCB->Enable(false);
    } else
    {
      m_boldCB->Enable(true);
      m_italicCB->Enable(true);
      m_underlinedCB->Enable(true);
      m_boldCB->SetValue(tmp->bold);
      m_italicCB->SetValue(tmp->italic);
      m_underlinedCB->SetValue(tmp->underlined);
    }
  }
  UpdateExample();
}

void ConfigDialogue::OnCheckbox(wxCommandEvent& event)
{
  style* tmp = GetStylePointer();

  tmp->bold = m_boldCB->GetValue();
  tmp->italic = m_italicCB->GetValue();
  tmp->underlined = m_underlinedCB->GetValue();

  UpdateExample();
}

void ConfigDialogue::OnChangeWarning(wxCommandEvent &event)
{
  wxMessageBox(_("Please restart wxMaxima for changes to take effect!"),
               _("Configuration warning"),
               wxOK|wxICON_WARNING);
}

style* ConfigDialogue::GetStylePointer()
{
  style* tmp;
  switch (m_styleFor->GetSelection())
  {
  case 1:
    tmp = &m_styleVariable;
    break;
  case 2:
    tmp = &m_styleNumber;
    break;
  case 3:
    tmp = &m_styleFunction;
    break;
  case 4:
    tmp = &m_styleSpecial;
    break;
  case 5:
    tmp = &m_styleGreek;
    break;
  case 6:
    tmp = &m_styleString;
    break;
  case 7:
    tmp = &m_styleInput;
    break;
  case 8:
    tmp = &m_styleMainPrompt;
    break;
  case 9:
    tmp = &m_styleOtherPrompt;
    break;
  case 10:
    tmp = &m_styleLabel;
    break;
  case 11:
    tmp = &m_styleUserDefinedLabel;
    break;
  case 12:
    tmp = &m_styleHighlight;
    break;
  case 13:
    tmp = &m_styleText;
    break;
  case 14:
    tmp = &m_styleSubsubsection;
    break;
  case 15:
    tmp = &m_styleSubsection;
    break;
  case 16:
    tmp = &m_styleSection;
    break;
  case 17:
    tmp = &m_styleTitle;
    break;
  case 18:
    tmp = &m_styleTextBackground;
    break;
  case 19:
    tmp = &m_styleBackground;
    break;
  case 20:
    tmp = &m_styleCellBracket;
    break;
  case 21:
    tmp = &m_styleActiveCellBracket;
    break;
  case 22:
    tmp = &m_styleCursor;
    break;
  case 23:
    tmp = &m_styleSelection;
    break;
  case 24:
    tmp = &m_styleEqualsSelection;
    break;
  case 25:
    tmp = &m_styleOutdated;
    break;
  case 26:
    tmp = &m_styleCodeHighlightingVariable;
    break;
  case 27:
    tmp = &m_styleCodeHighlightingFunction;
    break;
  case 28:
    tmp = &m_styleCodeHighlightingComment;
    break;
  case 29:
    tmp = &m_styleCodeHighlightingNumber;
    break;
  case 30:
    tmp = &m_styleCodeHighlightingString;
    break;
  case 31:
    tmp = &m_styleCodeHighlightingOperator;
    break;
  case 32:
    tmp = &m_styleCodeHighlightingEndOfLine;
    break;
  default:
    tmp = &m_styleDefault;
  }
  return tmp;
}

void ConfigDialogue::UpdateExample()
{
  style *tmp = GetStylePointer();
  wxString example = _("Example text");
  wxColour color(tmp->color);
  wxString font(m_styleDefault.font);

  if (tmp == &m_styleBackground)
    color = m_styleInput.color;

  int fontsize = m_fontSize;
  if (tmp == &m_styleText || tmp == &m_styleSubsubsection || tmp == &m_styleSubsection || tmp == &m_styleSection || tmp == &m_styleTitle)
  {
    fontsize = tmp->fontSize;
    font = tmp->font;
    if (fontsize == 0)
      fontsize = m_fontSize;
  }
  else if (tmp == &m_styleVariable || tmp == &m_styleNumber || tmp == &m_styleFunction ||
           tmp == &m_styleSpecial)
  {
    fontsize = m_mathFontSize;
    font = m_mathFontName;
  }

  if (tmp == &m_styleTextBackground) {
    m_examplePanel->SetFontSize(m_styleText.fontSize);
    m_examplePanel->SetStyle(m_styleText.color, m_styleText.italic, m_styleText.bold, m_styleText.underlined, m_styleText.font);
  }
  else {
    m_examplePanel->SetFontSize(fontsize);
    m_examplePanel->SetStyle(color, tmp->italic, tmp->bold, tmp->underlined, font);
  }

  if (tmp == &m_styleTextBackground ||
      tmp == &m_styleText)
    m_examplePanel->SetBackgroundColour(m_styleTextBackground.color);
  else
    m_examplePanel->SetBackgroundColour(m_styleBackground.color);

  m_examplePanel->Refresh();
}

void ConfigDialogue::LoadSave(wxCommandEvent& event)
{
  if (event.GetId() == save_id)
  {
    wxString file = wxFileSelector(_("Save style to file"),
                                   wxEmptyString, wxT("style.ini"), wxT("ini"),
                                   _("Config file (*.ini)|*.ini"),
                                   wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (file != wxEmptyString)
      WriteStyles(file);
  }
  else {
    wxString file = wxFileSelector(_("Load style from file"),
                                   wxEmptyString, wxT("style.ini"), wxT("ini"),
                                   _("Config file (*.ini)|*.ini"),
                                   wxFD_OPEN);
    if (file != wxEmptyString)
    {
      ReadStyles(file);
      UpdateExample();
    }
  }
}

#if defined __WXMSW__
void ConfigDialogue::OnColorButton(wxCommandEvent &event)
{
  OnChangeColor();
}
#endif

BEGIN_EVENT_TABLE(ConfigDialogue, wxPropertySheetDialog)
EVT_BUTTON(wxID_OPEN, ConfigDialogue::OnMpBrowse)
EVT_BUTTON(button_mathFont, ConfigDialogue::OnMathBrowse)
EVT_BUTTON(font_family, ConfigDialogue::OnChangeFontFamily)
#if defined __WXMSW__
EVT_BUTTON(color_id, ConfigDialogue::OnColorButton)
#endif
EVT_LISTBOX(listbox_styleFor, ConfigDialogue::OnChangeStyle)
EVT_COMBOBOX(language_id, ConfigDialogue::OnChangeWarning)
EVT_CHECKBOX(checkbox_bold, ConfigDialogue::OnCheckbox)
EVT_CHECKBOX(checkbox_italic, ConfigDialogue::OnCheckbox)
EVT_CHECKBOX(checkbox_underlined, ConfigDialogue::OnCheckbox)
EVT_BUTTON(save_id, ConfigDialogue::LoadSave)
EVT_BUTTON(load_id, ConfigDialogue::LoadSave)
EVT_BUTTON(style_font_family, ConfigDialogue::OnChangeFontFamily)
EVT_CLOSE(ConfigDialogue::OnClose)
END_EVENT_TABLE()

  void ExamplePanel::OnPaint(wxPaintEvent& event)
{
  wxString example(_("Example text"));
  wxPaintDC dc(this);
  int panel_width, panel_height;
  int text_width, text_height;
  wxFontWeight bold = wxFONTWEIGHT_NORMAL;
  wxFontStyle italic = wxFONTSTYLE_NORMAL;
  bool underlined = false;

  GetClientSize(&panel_width, &panel_height);

  dc.SetTextForeground(m_fgColor);

  if (m_bold)
    bold = wxFONTWEIGHT_BOLD;
  if (m_italic)
    italic = wxFONTSTYLE_SLANT;
  if (m_underlined)
    underlined = true;
  dc.SetFont(wxFont(m_size, wxFONTFAMILY_MODERN, italic, bold, underlined, m_font));
  dc.GetTextExtent(example, &text_width, &text_height);

  dc.DrawText(example, (panel_width - text_width) / 2,
              (panel_height - text_height) / 2);
}

BEGIN_EVENT_TABLE(ExamplePanel, wxPanel)
EVT_PAINT(ExamplePanel::OnPaint)
END_EVENT_TABLE()

#ifndef __WXMSW__
BEGIN_EVENT_TABLE(ColorPanel, wxPanel)
EVT_LEFT_UP(ColorPanel::OnClick)
END_EVENT_TABLE()
#endif
