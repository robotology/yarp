#include "tftree.h"

TfTree::TfTree()
{
    root.setName("Root");
}

bool TfTree::evaluateTf(tf& Tf)
{
    for (size_t i = 0; i < frames.size(); i++)
    {
        if (frames[i].getName() == Tf.child_frame || frames[i].getParent())
        {
            return false;
        }
    }

    return true;
}