#ifndef REVISIONDIFF_H
#define REVISIONDIFF_H

#include <vector>

#include "diffRig.h"

class RevisionNode;

class RevisionDiff
{
public:
    RevisionDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch);
    ~RevisionDiff();

    void setDiffRig(DiffRig _rig);
    DiffRig getDiffRig();

    void SetBoneDeltas(const std::vector<float> &_boneDeltas);
    std::vector<float> getBoneDeltas() const;

    std::shared_ptr<RevisionNode> getMasterNode();
    std::shared_ptr<RevisionNode> getBranchNode();

private:
    std::shared_ptr<RevisionNode> m_masterNode;
    std::shared_ptr<RevisionNode> m_branchNode;

    DiffRig m_rig;
    std::vector<float> m_boneDeltas;
};

#endif // REVISIONDIFF_H
