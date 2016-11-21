#ifndef REVISIONUTILS_H
#define REVISIONUTILS_H

#include <memory>

#include "revisionDiff.h"
#include "diffRig.h"
#include "boneAnim.h"
#include "boneAnimDelta.h"
#include "boneAnimDiff.h"

class RevisionNode;
class ModelRig;

class RevisionUtils
{
public:
    RevisionUtils();
    ~RevisionUtils();

    // will not be void once structure is created
    static RevisionDiff getRevisionDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch);

private:
    static DiffRig getAnimDiff(std::shared_ptr<ModelRig>, std::shared_ptr<ModelRig> branch);
    static BoneAnimDiff getBoneDiff(BoneAnim master, BoneAnim branch);
    static BoneAnimDelta getBoneAnimDelta(BoneAnim master, BoneAnim branch);
};

#endif // REVISIONUTILS_H
