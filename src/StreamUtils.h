// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode:
// nil -*-
//
//  Copyright (C) 2020 Kuba Ober <kuba@mareimbrium.org>
//
//  Everyone is permitted to copy and distribute verbatim copies
//  of this licence document, but changing it is not allowed.
//
//                       WXWINDOWS LIBRARY LICENCE
//     TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
//
//  This library is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Library General Public Licence as published by
//  the Free Software Foundation; either version 2 of the Licence, or (at your
//  option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//  Licence for more details.
//
//  You should have received a copy of the GNU Library General Public Licence
//  along with this software, usually in a file named COPYING.LIB.  If not,
//  write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
//  Floor, Boston, MA 02110-1301 USA.
//
//  EXCEPTION NOTICE
//
//  1. As a special exception, the copyright holders of this library give
//  permission for additional uses of the text contained in this release of the
//  library as licenced under the wxWindows Library Licence, applying either
//  version 3.1 of the Licence, or (at your option) any later version of the
//  Licence as published by the copyright holders of version 3.1 of the Licence
//  document.
//
//  2. The exception is that you may use, copy, link, modify and distribute
//  under your own terms, binary object code versions of works based on the
//  Library.
//
//  3. If you copy code from files distributed under the terms of the GNU
//  General Public Licence or the GNU Library General Public Licence into a
//  copy of this library, as this licence permits, the exception does not apply
//  to the code that you add in this way.  To avoid misleading anyone as to the
//  status of such modified files, you must delete this exception notice from
//  such code and/or adjust the licensing conditions notice accordingly.
//
//  4. If you write modifications of your own for this library, it is your
//  choice whether to permit this exception to apply to your modifications.  If
//  you do not wish that, you must delete the exception notice from such code
//  and/or adjust the licensing conditions notice accordingly.
//
//  SPDX-License-Identifier: wxWindows

#ifndef WXMAXIMA_STREAMUTILS_H
#define WXMAXIMA_STREAMUTILS_H

/*! \file
 * Various utilities that implement stream functionality missing in wxWidgets
 * and/or otherwise needed by wxMaxima.
 */

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <wx/stream.h>
#include <wx/string.h>
#include <codecvt>
#include <vector>

//! A stateful decoder that can feed itself from an input stream and
//! append its output to a string. Useful in any situation where the
//! exact amount of data read and written must be controlled.
class UTF8Decoder {
#if defined(__WINDOWS__) && wxUSE_UNICODE
  // Note: The explicit little_endian mode is needed on MinGW builds, otherwise the output is
  // big-endian and the subsequent decoding and use of it fails. MSVC builds are OK with this
  // mode explicitly set, or without it.
  using Codec = std::codecvt_utf8<wxStringCharType, 0x10ffff, std::codecvt_mode::little_endian>;
  Codec m_codec;
#else
  std::locale m_locale;
  using Codec = std::codecvt<wxStringCharType, char, std::mbstate_t>;
  const Codec &m_codec;
#endif

public:
  struct DecodeResult
  {
    size_t bytesRead = {};
    size_t outputSize = {};
    const wxStringCharType *output = {};
    const wxStringCharType *outputEnd = {};
    bool ok = false;
  };

  class State
  {
    std::mbstate_t m_codecState = {};
    std::vector<char> m_inBuf;
    size_t m_inBufCount = {};
    std::vector<wxStringCharType> m_outBuf;
    bool m_hadError = false;
  public:
    DecodeResult Decode(const Codec &, wxInputStream &in, size_t maxRead, size_t maxWrite);
    bool hadError() const { return m_hadError; }
  };

  UTF8Decoder();
  DecodeResult Decode(State &state, wxInputStream &in, size_t maxRead, size_t maxWrite);
};

#endif
