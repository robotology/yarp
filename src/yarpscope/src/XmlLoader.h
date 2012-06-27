/*
 *  This file is part of Yarp Port Scope
 *
 *  Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef YARPSCOPE_XMLLOADER_H
#define YARPSCOPE_XMLLOADER_H

namespace Glib {
class ustring;
}

namespace YarpScope
{
class XmlLoader
{
public:
    XmlLoader(const Glib::ustring &filename);
};

} // namespace YarpScope


#endif // YARPSCOPE_XMLLOADER_H
