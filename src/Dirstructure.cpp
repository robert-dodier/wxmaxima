﻿// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2015      Gunter Königsmann <wxMaxima@physikbuch.de>
//            (C) 2008-2009 Ziga Lenarcic <zigalenarcic@users.sourceforge.net>
//            (C) 2011-2011 cw.ahbong <cw.ahbong@gmail.com>
//            (C) 2012-2013 Doug Ilijev <doug.ilijev@gmail.com>
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


/*! \file
  This file defines the class DirStructure

  DirStructure informs about what directory is to be found where on the current system.
*/


#include "Dirstructure.h"
#include <wx/filename.h>

wxString Dirstructure::ResourcesDir()
{
  // Our ressources dir is somewhere near to the dir the binary can be found.
  wxFileName exe(wxStandardPaths::Get().GetExecutablePath());
  
  // We only need the drive and the directory part of the path to the binary
  exe.ClearExt();
  exe.SetName(wxEmptyString);
  
  // If the binary is in a source or bin folder the resources dir is one level above
  wxArrayString dirs = exe.GetDirs();
  if((dirs.Last().Upper() == wxT("SRC")) || (dirs.Last().Upper() == wxT("BIN")))
  {
    exe.RemoveLastDir();
    dirs = exe.GetDirs();
  }
  
  // If the binary is in the wxMaxima folder the resources dir is two levels above as we
  // are in MacOS/wxmaxima
  if((dirs.Last().Upper() == wxT("WXMAXIMA")) && (dirs[dirs.GetCount()-1].Upper() == wxT("MACOS")))
  {
    exe.RemoveLastDir();
    exe.RemoveLastDir();
    dirs = exe.GetDirs();
  }
  
  // If there is a Resources folder the ressources are there
  if(wxDirExists(exe.GetPath() + wxT("/Resources")))
  {
    exe.SetPath(exe.GetPath() + wxT("/Resources"));
    dirs = exe.GetDirs();
  }
  
  // If there is a share folder the ressources are there
  if(wxDirExists(exe.GetPath() + wxT("/share")))
  {
    exe.SetPath(exe.GetPath() + wxT("/share"));
    dirs = exe.GetDirs();
  }
  
  // If there is a data folder the ressources are there
  if(wxDirExists(exe.GetPath() + wxT("/data")))
  {
    exe.SetPath(exe.GetPath() + wxT("/data"));
    dirs = exe.GetDirs();
  }

  return exe.GetPath();
}

wxString Dirstructure::Prefix()
{
#ifndef PREFIX
#define PREFIX "/usr"
#endif
  return wxT(PREFIX);
}

wxString Dirstructure::GetwxMaximaLocation()
{
#if defined __WXMAC__
  wxString applicationPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep();

  if (applicationPath.EndsWith(wxT("/Contents/MacOS/")))
  {
    wxString bundle_nonAbsolute;
    wxFileName bundle(applicationPath + wxT("../../"));
    bundle.MakeAbsolute();
    if (bundle.GetFullPath().EndsWith(wxT(".app")))
      return bundle.GetFullPath();
  }


  if (wxFileExists(applicationPath))
  {
    wxFileName applicationPathName = wxFileName(applicationPath);
    applicationPathName.MakeAbsolute();
    return applicationPathName.GetFullPath();
  }

  if (wxFileExists("/Applications/wxMaxima.app"))
    return wxT("/Applications/wxMaxima.app");
  if (wxFileExists("/Applications/wxmaxima.app"))
    return wxT("/Applications/wxmaxima.app");
  return (wxT("wxmaxima"));
#else

  return wxStandardPaths::Get().GetExecutablePath();
#endif

}

wxString Dirstructure::UserConfDir()
{
  return wxGetHomeDir() + wxT("/");
}

wxString Dirstructure::MaximaDefaultLocation()
{
#if defined __WXMSW__
  wxString maxima = wxGetCwd();
  if (maxima.Right(8) == wxT("wxMaxima"))
    maxima.Replace(wxT("wxMaxima"), wxT("bin\\maxima.bat"));
  else
    maxima.Append("\\maxima.bat");
  if (!wxFileExists(maxima))
  {
    wxFileName exe = wxStandardPaths::Get().GetExecutablePath();
    exe.MakeAbsolute();
    wxString exeDir = exe.GetPathWithSep();
    wxString maximapath = exeDir + wxT("..") + exe.GetPathSeparator() +
      wxT("bin")  + exe.GetPathSeparator() + wxT("maxima.bat");
  }
  wxFileName maximapath(maxima);
  maximapath.MakeAbsolute();
  return maximapath.GetFullPath();
#elif defined __WXMAC__
  wxString command;
  if (wxFileExists("/Applications/Maxima.app"))
    command = wxT("/Applications/Maxima.app");
  if (wxFileExists("/Applications/maxima.app"))
    command = wxT("/Applications/maxima.app");
  else if (wxFileExists("/usr/local/bin/maxima"))
    command = wxT("/usr/local/bin/maxima");
  else if (wxFileExists("/usr/bin/maxima"))
    command = wxT("/usr/bin/maxima");
  else
    command = wxT("maxima");
  return command;
#else
  return wxT("maxima");
#endif
}
