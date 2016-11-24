#ifndef DIFFFUNCTIONS_H
#define DIFFFUNCTIONS_H

#include "revisionDiff.h"
#include "diffRig.h"
#include "boneAnim.h"
#include "boneAnimDiff.h"

class DiffFunctions
{
public:
    RevisionUtils(){};
    ~RevisionUtils(){};

    static void getAnimDiff(ModelRig master, ModelRig branch, DiffRig &outRig);
    static BoneAnimDiff getBoneDiff(BoneAnim master, BoneAnim branch);

    static std::vector<PosAnim>     getPositionDiffs(std::vector<PosAnim>& master, std::vector<PosAnim>& branch);
    static std::vector<RotAnim>     getRotationDiffs(std::vector<RotAnim>& master, std::vector<RotAnim>& branch);
    static std::vector<ScaleAnim>   getScaleDiffs(std::vector<ScaleAnim>& master, std::vector<ScaleAnim>& branch);
};

#endif // REVISIONUTILS_H
