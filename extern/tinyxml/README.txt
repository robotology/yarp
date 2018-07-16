TinyXML
-------

TinyXML is a simple, small, minimal, C++ XML parser that can be easily
integrating into other programs. It reads XML and creates C++ objects
representing the XML document. The objects can be manipulated, changed,
and saved again as XML.

Homepage: http://www.grinninglizard.com/tinyxml/index.html
          https://sourceforge.net/projects/tinyxml/

Copyright: Original code by Lee Thomason (www.grinninglizard.com)

License:
 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any
 damages arising from the use of this software.
 .
 Permission is granted to anyone to use this software for any
 purpose, including commercial applications, and to alter it and
 redistribute it freely, subject to the following restrictions:
 .
 1. The origin of this software must not be misrepresented; you must
 not claim that you wrote the original software. If you use this
 software in a product, an acknowledgment in the product documentation
 would be appreciated but is not required.
 .
 2. Altered source versions must be plainly marked as such, and
 must not be misrepresented as being the original software.
 .
 3. This notice may not be removed or altered from any source
 distribution.

Version: 2.6.2

Patches:
 * enforce-use-stl.patch: (origin debian package 2.6.2-1) TinyXml is
   built with TIXML_USE_STL, so we have to enforce it when the library
   is used.

 * entity-encoding.patch: (origin debian package 2.6.2-1) TinyXML
   incorrectly encodes text element containing an ampersand followed by
   either x or #.

 * 0001-Return-TIXML_WRONG_TYPE-for-negative-values-in-Query.patch:
   Return TIXML_WRONG_TYPE for negative values in
   QueryUnsignedAttribute
   https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
