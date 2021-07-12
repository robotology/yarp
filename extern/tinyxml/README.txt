TinyXML
-------

TinyXML is a simple, small, minimal, C++ XML parser that can be easily
integrating into other programs. It reads XML and creates C++ objects
representing the XML document. The objects can be manipulated, changed,
and saved again as XML.

Homepage: http://www.grinninglizard.com/tinyxml/index.html
          https://sourceforge.net/projects/tinyxml/

Copyright: Original code by Lee Thomason (www.grinninglizard.com)

License: Zlib

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
