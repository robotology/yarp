/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  UTILS_H
#define  UTILS_H

#include <string>

//adds the symbol "*" at the beginning of each line, as requested by doxygen format
inline std::string doxygenize_string (std::string s)
{
    for (size_t i = 0; i < s.length(); ++i) {
        if (s[i] == '\n') {
            s.insert(i + 1, 1, '*');
            ++i;
        }
    }
    return s;
}

//generate a string containing the current date and time
inline std::string current_time()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::string currentTimeString = std::ctime(&currentTime);
    return std::string ("// Generated on: ") + currentTimeString + std::string("\n");
}

//return true if the string does not contain any valid character
inline bool containsOnlySymbols(const std::string& str) {
    for (char ch : str) {
        if (std::isalnum(static_cast<unsigned char>(ch))) {
            return false; // If any alphanumeric character is found, return false
        }
    }
    return true; // If all characters are symbols, return true
}

//remove all spaces from a string
inline std::string trimSpaces(const std::string& str) {
    size_t firstNonSpace = str.find_first_not_of(" \t");
    size_t lastNonSpace = str.find_last_not_of(" \t");

    if (firstNonSpace != std::string::npos && lastNonSpace != std::string::npos) {
        return str.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
    }
    else {
        return ""; // If string contains only spaces or is empty, return an empty string
    }
}

#endif
