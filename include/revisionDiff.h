#ifndef REVISIONDIFF_H
#define REVISIONDIFF_H

#include <vector>

#include "animNodeDiff.h"

class RevisionDiff
{
public:
    RevisionDiff();
    ~RevisionDiff();

private:

    std::vector<AnimNodeDiff> diffNodes;
};

#endif // REVISIONDIFF_H
