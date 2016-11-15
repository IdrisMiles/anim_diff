#ifndef REVISIONUTILS_H
#define REVISIONUTILS_H

#include <memory>
#include "revisionDiff.h"

class RevisionNode;

class RevisionUtils
{
public:
    RevisionUtils();
    ~RevisionUtils();

    // will not be void once structure is created
    static RevisionDiff getDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch);
};

#endif // REVISIONUTILS_H
