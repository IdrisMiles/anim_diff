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

    void SetBoneDeltas(const std::unordered_map<std::string, float> &_boneDeltas);
    std::unordered_map<std::string, float> getBoneDeltas() const;

    std::shared_ptr<RevisionNode> getMasterNode();
    std::shared_ptr<RevisionNode> getBranchNode();

private:
    std::shared_ptr<RevisionNode> m_masterNode;
    std::shared_ptr<RevisionNode> m_branchNode;

    DiffRig m_rig;
    std::unordered_map<std::string, float> m_boneDeltas;
};

#endif // REVISIONDIFF_H
