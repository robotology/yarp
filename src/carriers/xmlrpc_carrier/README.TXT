# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

XMLRPC to Bottle translation:
  xmlrpc / bottle
  int <-> int
  string <-> string
  double <-> double
  array <-> list
  struc <-> list

Both arrays and strucs in XMLRPC are mapped to Bottle lists.
Strucs are mapped to a list with they tag "dict" prepended,
and then sublists with key->value mappings.
  (dict (key1 val1) (key2 val2))

(The Bottle find/findGroup/check methods will work fine on
such lists, or you could populate a Property object if
you want fast look-up.)

If you want to convert a Bottle list that may start with the
string "dict" or "list", prepend the keyword "list".
  (list ...)
That keyword will be silently consumed, and will guarantee
that an XMLRPC array is produced.
