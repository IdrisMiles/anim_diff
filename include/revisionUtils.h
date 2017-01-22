#ifndef REVISIONUTILS_H
#define REVISIONUTILS_H

#include <memory>
#include "diffFunctions.h"
#include "revisionNode.h"
#include "revisionDiff.h"
#include "revisionMerge.h"

class ModelRig;

class RevisionUtils : public DiffFunctions
{
public:
    RevisionUtils();
    ~RevisionUtils();

    // will not be void once structure is created
    static RevisionDiff getRevisionDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch);

    static RevisionMerge getRevisionMerge(RevisionDiff _diffA, 
                                            RevisionDiff _diffB, 
                                            std::shared_ptr<RevisionNode> _parentNode);
                                            
    static RevisionNode getRevisionNodeForDiff(std::shared_ptr<RevisionDiff> _diff);

private:

    static void copyRigStructure(std::shared_ptr<ModelRig> pRig, 
                                        DiffRig _diffRig, 
                                        std::shared_ptr<Bone> pParentBone, 
                                        std::shared_ptr<Bone> pNewBone, 
                                        std::shared_ptr<Bone> pOldBone);

    static Model getModelFromDiff(std::shared_ptr<RevisionDiff> _diff);
};

#endif // REVISIONUTILS_H
