/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Renaud Detry
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef stmac_h
#define stmac_h

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <stdexcept>


#define DEBUG(x) cout << #x << ": " << x << endl
#define ensure(expression) if (!(expression)) \
          throw runtime_error(string(__FILE__) + ":" + rtoolbox::stringify(__LINE__) + \
                              ": failed to ensure `" + #expression + "'")

class BadConversion : public std::runtime_error {
public:
    BadConversion(const std::string& s) : std::runtime_error(s) { }
};

template<typename T>
inline std::string stringify(const T& x)
{
    std::ostringstream o;
    if (!(o << x))
        throw BadConversion(std::string("stringify(")
                            + typeid(x).name() + ")");
    return o.str();
}

template<typename T>
inline T numify(const std::string& s,
                bool failIfLeftoverChars = true)
{
    T x;
    std::istringstream i(s);
    char c;
    if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
        throw BadConversion(s);
    return x;
}



#endif

