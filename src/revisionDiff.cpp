#include "revisionDiff.h"
#include "revisionNode.h"

RevisionDiff::RevisionDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch)
:
    m_masterNode(_master),
    m_branchNode(_branch)
{

}

RevisionDiff::~RevisionDiff()
{
    
}

void RevisionDiff::setDiffRig(DiffRig _rig)
{
    m_rig = _rig;
}
    
DiffRig RevisionDiff::getDiffRig()
{
    return m_rig;
}

void RevisionDiff::SetBoneDeltas(const std::vector<float> &_boneDeltas)
{
    m_boneDeltas = _boneDeltas;
}

std::vector<float> RevisionDiff::getBoneDeltas() const
{
    return m_boneDeltas;
}

std::shared_ptr<RevisionNode> RevisionDiff::getMasterNode()
{
    return m_masterNode;
}

std::shared_ptr<RevisionNode> RevisionDiff::getBranchNode()
{
    return m_branchNode;
}
