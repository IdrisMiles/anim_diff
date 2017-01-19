#ifndef DIFFFUNCTIONS_H
#define DIFFFUNCTIONS_H

#include "revisionDiff.h"
#include "diffRig.h"
#include "modelRig.h"
#include "mergeRig.h"
#include "boneAnim.h"
#include "boneAnimDiff.h"

class DiffFunctions
{
public:
    DiffFunctions(){};
    ~DiffFunctions(){};

    static void getAnimDiff(ModelRig master, ModelRig branch, DiffRig &outRig);
    static BoneAnimDiff getBoneDiff(BoneAnim master, BoneAnim branch);

    static std::vector<PosAnim>     getPositionDiffs(std::vector<PosAnim>& master, std::vector<PosAnim>& branch);
    static std::vector<RotAnim>     getRotationDiffs(std::vector<RotAnim>& master, std::vector<RotAnim>& branch);
    static std::vector<ScaleAnim>   getScaleDiffs(std::vector<ScaleAnim>& master, std::vector<ScaleAnim>& branch);

    // pos helper methods
    static PosAnim getPosDiff(PosAnim posA, PosAnim posB);
    static PosAnim getPosDiff(PosAnim posA, PosAnim posB1, PosAnim posB2);

    static ScaleAnim getScaleDiff(ScaleAnim scaleA, ScaleAnim scaleB);
    static ScaleAnim getScaleDiff(ScaleAnim scaleA, ScaleAnim scaleB1, ScaleAnim scaleB2);

    static RotAnim getRotDiff(RotAnim rotA, RotAnim rotB);
    static RotAnim getRotDiff(RotAnim rotA, RotAnim rotB1, RotAnim rotB2);

    // merge methods
    static void getAnimMerge(DiffRig master, DiffRig branch, MergeRig &outRig);
    static BoneAnimMerge getBoneMerge(BoneAnimDiff master, BoneAnimDiff branch);

};

#endif // REVISIONUTILS_H