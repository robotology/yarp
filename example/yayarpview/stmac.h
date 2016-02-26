/*
 * Copyright: (C) 2010 Renaud Detry
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

