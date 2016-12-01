#ifndef REVISIONUTILS_H
#define REVISIONUTILS_H

#include <memory>
#include "diffFunctions.h"
#include "revisionDiff.h"

class RevisionNode;
class ModelRig;

class RevisionUtils : public DiffFunctions
{
public:
    RevisionUtils();
    ~RevisionUtils();

    // will not be void once structure is created
    static RevisionDiff getRevisionDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch);
};

#endif // REVISIONUTILS_H
