// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2007-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2014-2016 Gunter Königsmann <wxMaxima@physikbuch.de>
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
  This file declares the class SlideShowCell

  SlideShowCell is the Cell type that represents animations.
*/

#ifndef SLIDESHOWCELL_H
#define SLIDESHOWCELL_H

#include "Cell.h"
#include "Image.h"
#include <wx/image.h>
#include <wx/timer.h>

#include <wx/filesys.h>
#include <wx/fs_arc.h>
#include <wx/mstream.h>
#include <wx/wfstream.h>

#include <vector>

class SlideShow final : public Cell
{
public:
  /*! The constructor

    \param framerate The individual frame rate that has to be set for this cell only. 
    If the default frame rate from the config is to be used instead this parameter 
    has to be set to -1.
    \param config A pointer to the pointer to the configuration storage of the 
                  worksheet this cell belongs to.
    \param filesystem The filesystem the contents of this slideshow can be found in.
                      NULL = the operating system's filesystem
    \param parent     The parent GroupCell this cell belongs to.
   */
  SlideShow(GroupCell *parent, Configuration **config, std::shared_ptr<wxFileSystem> filesystem, int framerate = -1);
  SlideShow(GroupCell *parent, Configuration **config, int framerate = -1);
  SlideShow(const SlideShow &cell);
  //! A constructor that loads the compressed file from a wxMemoryBuffer
  SlideShow(GroupCell *parent, Configuration **config, const wxMemoryBuffer &image, const wxString &type);
  SlideShow(GroupCell *parent, Configuration **config, const wxString &image, bool remove);

  std::unique_ptr<Cell> Copy() const override;
  const CellTypeInfo &GetInfo() override;
  ~SlideShow();
  void LoadImages(wxMemoryBuffer imageData);
  void LoadImages(wxString imageFile);

  //! A class that publishes wxm data to the clipboard
  static wxDataFormat m_gifFormat;

  //! Can the current image be exported in SVG format?
  bool CanExportSVG() const {return (m_images[m_displayed] != NULL) && m_images[m_displayed]->CanExportSVG();}

  //! A Gif object for the clipboard
  class GifDataObject : public wxCustomDataObject
  {
  public:
    explicit GifDataObject(const wxMemoryOutputStream &str);

    GifDataObject();

  private:
    wxCharBuffer m_databuf;
  };

  bool IsOk() const {return (m_size>0) && (m_images[m_displayed]->IsOk());}
  
  const wxString &GetToolTip(wxPoint point) const override;

  /*! Remove all cached scaled images from memory

    To be called when the slideshow is outside of the displayed portion 
    of the screen; The bitmaps will be re-generated when needed.
   */
  void ClearCache() override;

  void LoadImages(wxArrayString images, bool deleteRead);

  int GetDisplayedIndex() const { return m_displayed; }

  wxImage GetBitmap(int n) const
  { return m_images[n]->GetUnscaledBitmap().ConvertToImage(); }

  void SetDisplayedIndex(int ind);

  int Length() const { return m_size; }

  //! Exports the image the slideshow currently displays
  wxSize ToImageFile(wxString file);

  //! Exports the whole animation as animated gif
  wxSize ToGif(wxString file);

  bool CopyToClipboard() const override;
  
  //! Put the animation on the clipboard.
  bool CopyAnimationToClipboard();

  /*! Get the frame rate of this SlideShow [in Hz].

    Returns either the frame rate set for this slide show cell individually or 
    the default frame rate chosen in the config.
   */
  int GetFrameRate() const;

  /*! Reload the animation timer starting and instantiating and registering it if necessary.

    If the timer is already running, the request to reload it is ignored.
   */
  void ReloadTimer();

  /*! Stops the timer
    
    Also deletes the timer as on MSW there aren't many timers available.
   */
  void StopTimer();

  /*! Set the frame rate of this SlideShow [in Hz].
    
    \param Freq The requested frequency [in Hz] or -1 for: Use the default value.
    \return The frame rate that was actually set.
   */
  int SetFrameRate(int Freq);

  bool AnimationRunning() const { return m_animationRunning; }
  void AnimationRunning(bool run);
  bool CanPopOut() const override
  { return (!m_images[m_displayed]->GnuplotSource().empty()); }

  void GnuplotSource(int image, wxString gnuplotFilename, wxString dataFilename, std::shared_ptr<wxFileSystem> filesystem)
  { m_images[image]->GnuplotSource(gnuplotFilename, dataFilename, filesystem); }

  wxString GnuplotSource() const override
  {
    if (!m_images[m_displayed])
      return wxEmptyString;
    else
      return m_images[m_displayed]->GnuplotSource();
  }

  void SetNextToDraw(Cell *next) override { m_nextToDraw = next; }
  Cell *GetNextToDraw() const override {return m_nextToDraw;}

private:
  CellPointers *const m_cellPointers = GetCellPointers(); // must come before m_timer (!)
  wxTimer m_timer;
  std::vector<std::shared_ptr<Image>> m_images;
  std::shared_ptr<wxFileSystem> m_fileSystem;
  CellPtr<Cell> m_nextToDraw;

  /*! The framerate of this cell.

    Can contain a frame rate [in Hz] or a -1, which means: Use the default frame rate.
  */
  int m_framerate = -1;
  int m_size = 0;
  int m_displayed = 0;
  int m_imageBorderWidth = 0;

//** Bitfield objects (1 bytes)
//**
  void InitBitFields()
  { // Keep the initailization order below same as the order
    // of bit fields in this class!
    m_animationRunning = true;
    m_drawBoundingBox = false;
  }

  bool m_animationRunning : 1 /* InitBitFields */;
  bool m_drawBoundingBox : 1 /* InitBitFields */;


  int GetImageBorderWidth() const override { return m_imageBorderWidth; }

  void Recalculate(AFontSize fontsize) override;

  void Draw(wxPoint point) override;

  wxString ToMatlab() const override;
  wxString ToRTF() const override;
  wxString ToString() const override;
  wxString ToTeX() const override;
  wxString ToXML() const override;

  void DrawBoundingBox(wxDC &WXUNUSED(dc), bool WXUNUSED(all) = false)  override
  { m_drawBoundingBox = true; }
};

#endif // SLIDESHOWCELL_H
