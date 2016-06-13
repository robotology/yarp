#include "tfframe.h"

class TfTree
{
    TfFrame              root;
    std::vector<TfFrame> frames;
public:
         TfTree();
         ~TfTree();
    bool evaluateTf(tf& Tf);
    tf   getTfSum(const std::string& parent, const std::string& child);
    tf   getTfSum(const TfFrame& parent, const TfFrame& child);
};