/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <iostream>
#include <stack>

void SectionHandler::insert(const Parameter& param)
{
    insertRecursive(rootnode, param.getListOfGroups(), param, 0);
}

void SectionHandler::insertRecursive(SectionNode& node, const std::deque<std::string>& groups, const Parameter& param, size_t levelOfNesting)
{
    // Case 1
    // This is a leaf. Insert the parameter at the beginning of the deque.
    // In this way the deque contains at the beginning all the parameters which do not belong to a further subsection
    if (groups.empty())
    {
        auto newNode = std::make_unique<SectionNode>();
        newNode->name = param.getParamOnly();
        newNode->param = param;
        newNode->nestingLevel = levelOfNesting;
        node.nextNode.push_front(std::move(newNode));
        return;
    }

    const std::string& currentGroup = groups.front();
    // Case 2
    // Search if it already exists a subsection with the same name then...
    for (const auto& next : node.nextNode)
    {
        if (next->name == currentGroup)
        {
            //Case 2a
            //...if it exists, go deeper in the tree
            insertRecursive(*next, std::deque<std::string>(groups.begin() + 1, groups.end()), param, levelOfNesting+1);
            return;
        }
    }

    //Case 2b
    //...if it does not exist, than create the new subsection at the end of the deque.
    // In this way the deque contains at the beginning all the parameters which do not belong to a further subsection
    auto newNode = std::make_unique<SectionNode>();
    newNode->name = currentGroup;
   // newNode->paramPointer = nullptr; //this is a group, not an actual parameter
    newNode->nestingLevel = levelOfNesting;
    node.nextNode.push_back(std::move(newNode));
    // then go deeper in the tree
    insertRecursive(*node.nextNode.back(), std::deque<std::string>(groups.begin() + 1, groups.end()), param, levelOfNesting+1);
}

SectionHandler::SectionNode* SectionHandler::iterator_start()
{
    computePreOrderTraversal();
    auto it = prePreOrderTraversal.begin();
    iterator_ptr = it;
    return *iterator_ptr;
}

void SectionHandler::computePreOrderTraversal()
{
    std::vector<SectionHandler::SectionNode*> result;

    std::stack<SectionHandler::SectionNode*> nodeStack;
    nodeStack.push(&rootnode);

    while (!nodeStack.empty()) {
        SectionHandler::SectionNode* node = nodeStack.top();
        nodeStack.pop();

        // Do not insert the root node, it is not a real parameter, it is just a container for all other parameters
        if (node->name!="")
        {   result.push_back(node); }

        // Insert children in reverse order to perform a pre-order traversal
        for (auto it = node->nextNode.rbegin(); it != node->nextNode.rend(); ++it)
        {
            nodeStack.push(it->get());
        }
    }

    prePreOrderTraversal = result;
}

SectionHandler::SectionNode* SectionHandler::iterator_next()
{
    if (iterator_ptr == prePreOrderTraversal.end()) { return nullptr; }

    iterator_ptr++;
    if (iterator_ptr == prePreOrderTraversal.end()) { return nullptr; }

    return *iterator_ptr;
}

SectionHandler::SectionNode* SectionHandler::iterator_get()
{
   return *iterator_ptr;
}

bool SectionHandler::SectionNode::isParameter()
{
    return nextNode.size() == 0;
}

bool SectionHandler::SectionNode::isGroupOfParameters()
{
    return nextNode.size() != 0;
}
