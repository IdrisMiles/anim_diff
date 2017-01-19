#ifndef REVISIONMERGE_H
#define REVISIONMERGE_H

#include <vector>
#include "diffRevision.h"

class RevisionNode;

class RevisionMerge : public RevisionDiff
{
public:
    
    RevisionMerge(std::shared_ptr<RevisionDiff> _masterDiff, std::shared_ptr<RevisionDiff> _branchDiff);
    ~RevisionMerge();

};

#endif // REVISIONMERGE_H
