#include "tfframe.h"

TfFrame::TfFrame()
{
    valid = false;
    parent = NULL;
}

TfFrame::~TfFrame()
{

}
bool TfFrame::addChild(TfFrame& frame)
{
    if (frame.getParent() && frame.getParent()->getParent())
    {
        return false;
    }
    children.push_back(&frame);
    frame.setParent(*this);
    return true;
}
bool TfFrame::setName(char* newName)
{
    if (!newName)
    {
        return false;
    }
    name = newName;
    return true;
}