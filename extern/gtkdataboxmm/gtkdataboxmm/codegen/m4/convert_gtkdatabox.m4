dnl Copyright (c) 2012  iCub Facility, Istituto Italiano di Tecnologia
dnl   @author Daniele E. Domenichelli <daniele.domenichelli@iit.it>
dnl This file is part of gtkdataboxmm.

_EQUAL(gint16,short)

_EQUAL(`gfloat',`float')
_EQUAL(`gfloat*',`float*')

_CONVERSION(`float&',`gfloat*',`&($3)')
_CONVERSION(`gfloat*',`const gfloat*',`const_cast<const gfloat*>($3)',`$3')
_CONVERSION(`const float*',`gfloat*',`const_cast<gfloat*>($3)',`$3')

_CONVERSION(`Databox&',`GtkDatabox*',__FR2P)
_CONVERSION(`const Databox&',`GtkDatabox*',__FCR2P))
_CONVERSION(`Databox&',`GtkDatabox*',($3).gobj())
_CONVERSION(`GtkDatabox*',`Databox*',`&Glib::wrap($3)')
_CONVERSION(`GtkDatabox*',`Databox&',`Glib::wrap($3)')
_CONVERSION(`const Databox*',`const GtkDatabox*',($3).gobj())
_CONVERSION(`const Databox&',`const GtkDatabox*',($3).gobj())

_CONVERSION(`Ruler&',`GtkDataboxRuler*',__FR2P)
_CONVERSION(`const Ruler&',`GtkDataboxRuler*',__FCR2P))
_CONVERSION(`Ruler&',`GtkDataboxRuler*',($3).gobj())
_CONVERSION(`GtkDataboxRuler*',`Ruler*',`&Glib::wrap($3)')
_CONVERSION(`GtkDataboxRuler*',`Ruler&',`Glib::wrap($3)')
_CONVERSION(`const Ruler*',`const GtkDataboxRuler*',($3).gobj())
_CONVERSION(`const Ruler&',`const GtkDataboxRuler*',($3).gobj())


_CONVERSION(`GtkDataboxGraph*',`Glib::RefPtr<Graph>',`Glib::wrap($3)')
_CONVERSION(`const Glib::RefPtr<Graph>&',`GtkDataboxGraph*',__CONVERT_REFPTR_TO_P)
_CONVERSION(`Glib::RefPtr<Graph>',`GtkDataboxGraph*',__CONVERT_REFPTR_TO_P)

_CONV_ENUM(ScaleType,GtkDataboxScaleType)
_CONVERSION(MarkersPosition,GtkDataboxMarkersPosition,static_cast<GtkDataboxMarkersPosition>($3))
_CONVERSION(MarkersTextPosition,GtkDataboxMarkersTextPosition,static_cast<GtkDataboxMarkersTextPosition>($3))
_CONVERSION(MarkersType,GtkDataboxMarkersType,static_cast<GtkDataboxMarkersType>($3))

dnl _CONV_ENUM(MarkersPosition,GtkDataboxMarkersPosition)
dnl _CONV_ENUM(MarkersTextPosition,GtkDataboxMarkersTextPosition)
dnl _CONV_ENUM(MarkersType,GtkDataboxMarkersType)
