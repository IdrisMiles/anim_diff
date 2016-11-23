#ifndef REVISIONUTILS_H
#define REVISIONUTILS_H

#include <memory>

#include "revisionDiff.h"
#include "diffRig.h"
#include "boneAnim.h"
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
    static void getAnimDiff(ModelRig master, ModelRig branch, DiffRig &outRig);
    static BoneAnimDiff getBoneDiff(BoneAnim master, BoneAnim branch);

    static std::vector<PosAnim>     getPositionDiffs(std::vector<PosAnim>& master, std::vector<PosAnim>& branch);
    static std::vector<RotAnim>     getRotationDiffs(std::vector<RotAnim>& master, std::vector<RotAnim>& branch);
    static std::vector<ScaleAnim>   getScaleDiffs(std::vector<ScaleAnim>& master, std::vector<ScaleAnim>& branch);
};

#endif // REVISIONUTILS_H
