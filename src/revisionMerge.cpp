#include "revisionMerge.h"    
    
RevisionMerge::RevisionMerge(std::shared_ptr<RevisionDiff> _masterDiff, std::shared_ptr<RevisionDiff> _branchDiff)
: revisionDiff(_masterDiff.getMasterNode(), _masterDiff.getBranchNode())
{

}

~RevisionMerge::RevisionMerge()
{
}