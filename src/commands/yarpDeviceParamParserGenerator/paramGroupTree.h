/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  PARAMGROUPTREE_H
#define  PARAMGROUPTREE_H

#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <ctime>
#include <memory>

#include "parameter.h"

class SectionHandler
{
    private:
    class SectionNode
    {
        public:
        SectionNode() = default;
        virtual ~SectionNode() = default;

        std::deque<std::unique_ptr<SectionNode>>  nextNode;
        std::string                               name;
        size_t                                    nestingLevel=0;
        Parameter                                 param;

        bool isParameter();
        bool isGroupOfParameters();
    };

    private:
    void computePreOrderTraversal();

    std::vector<SectionNode*>::iterator iterator_ptr;
    std::vector<SectionNode*> prePreOrderTraversal;

    SectionNode rootnode;
    void insertRecursive(SectionNode& node, const std::deque<std::string>& groups, const Parameter& param, size_t levelOfNesting);
    bool traverseNodes();

    public:
    void insert(const Parameter& param);
    SectionNode* iterator_start();
    SectionNode* iterator_next();
    SectionNode* iterator_get();
};

#endif
