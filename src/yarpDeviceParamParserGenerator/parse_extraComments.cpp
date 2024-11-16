/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>

bool ParamsFilesGenerator::parseExtraComments(const std::string inputfilename)
{
    std::string line;

    std::ifstream inputfile(inputfilename);

    bool b = inputfile.is_open();
    if (!b)
    {
        std::cerr << "Unable to open file: " << inputfilename << std::endl;
        return false;
    }

    m_extra_comments.assign(std::istreambuf_iterator<char>(inputfile),
                            std::istreambuf_iterator<char>());

    inputfile.close();
    return true;
}
