/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_CONF_STRING_H
#define YARP_CONF_STRING_H

#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace yarp {
namespace conf {
namespace string {

/**
 * Utility to split a string by a @c separator, into a vector of strings.
 *
 * @tparam ContainerT the type of the container to return
 * @param s the string to split
 * @param regex a regular expression that is used as separator to separate the elements
 * @return a vector of strings each containing one token
 */
template <typename ContainerT = std::vector<std::string>>
inline ContainerT split(const typename ContainerT::value_type& s, std::basic_regex<typename ContainerT::value_type::value_type> regex)
{
    using string_type = typename ContainerT::value_type;
    using const_iterator_type = typename string_type::const_iterator;
    if (s.empty()) {
        return {};
    }

    std::vector<string_type> ret;
    std::copy(std::regex_token_iterator<const_iterator_type>{s.begin(), s.end(), regex, -1},
              std::regex_token_iterator<const_iterator_type>{},
              std::back_inserter(ret));
    return ret;
}

/**
 * Utility to split a string by a @c separator, into a vector of strings.
 *
 * @tparam ContainerT the type of the container to return
 * @param s the string to split
 * @param separator a string of characters separating the elements
 * @return a vector of strings each containing one token
 */
template <typename ContainerT = std::vector<std::string>>
inline ContainerT split(const typename ContainerT::value_type& s, typename ContainerT::value_type separator)
{
    using string_type = typename ContainerT::value_type;
    using char_type = typename string_type::value_type;

    // Escape all characters in the string
    string_type sep_esc(separator.size()*2, char_type{'\\'});
    for (size_t i = 0; i < s.size(); ++i) {
        sep_esc[i*2 + 1] = separator[i];
    }

    return split<ContainerT>(s, std::basic_regex<char_type>{string_type{"("} + sep_esc + string_type{")|$"}});
}

/**
 * Utility to split a string by a @c separator, into a vector of strings.
 *
 * @tparam ContainerT the type of the container to return
 * @param s the string to split
 * @param separator a character separating the elements
 * @return a vector of strings each containing one token
 */
template <typename ContainerT = std::vector<std::string>>
inline ContainerT split(const typename ContainerT::value_type& s, typename ContainerT::value_type::value_type separator)
{
    using string_type = typename ContainerT::value_type;
    using char_type = typename string_type::value_type;

    return split<ContainerT>(s, std::basic_regex<char_type>{string_type{"\\"} + string_type{separator} + string_type{"|$"}});
}

/**
 * Utility to join the elements in a container to a single string separated by
 * a @c separator.
 *
 * @tparam ContainerT the type of the container
 * @param begin an iterator to the first element of the container
 * @param end an iterator to the end of the container
 * @param separator a string to append between elements
 * @return a strings containing all the token separated by @c separator
 */
template <typename ContainerT = std::vector<std::string>>
inline typename ContainerT::value_type join(typename ContainerT::const_iterator begin,
                                            typename ContainerT::const_iterator end,
                                            const typename ContainerT::value_type& separator = ", ")
{
    using string_type = typename ContainerT::value_type;
    using char_type = typename string_type::value_type;
    using traits_type = typename string_type::traits_type;
    using allocator_type = typename string_type::allocator_type;

    if (begin == end) {
        return {};
    }

    std::basic_stringstream<char_type, traits_type, allocator_type> s;
    std::copy(begin, end - 1, std::ostream_iterator<string_type, char_type, traits_type>(s, separator.c_str()));
    s << *(end - 1);
    return s.str();
}

/**
 * Utility to join the elements in a container to a single string separated by
 * a @c separator.
 *
 * @tparam ContainerT the type of the container
 * @param begin an iterator to the first element of the container
 * @param end an iterator to the end of the container
 * @param separator a character to append between elements
 * @return a strings containing all the token separated by @c separator
 */
template <typename ContainerT = std::vector<std::string>>
inline typename ContainerT::value_type join(typename ContainerT::const_iterator begin,
                                            typename ContainerT::const_iterator end,
                                            const typename ContainerT::value_type::value_type& separator)
{
    return join<ContainerT>(begin, end, typename ContainerT::value_type{separator});
}

} // namespace string
} // namespace conf
} // namespace yarp

#endif // YARP_CONF_STRING_H
