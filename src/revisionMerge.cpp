#include "revisionMerge.h"    
#include "revisionNode.h"

RevisionMerge::RevisionMerge(std::shared_ptr<RevisionNode> _master, 
                                std::shared_ptr<RevisionNode> _branch, 
                                std::shared_ptr<RevisionNode> _parent)
:   m_masterNode(_master),
    m_branchNode(_branch),
    m_parentNode(_parent)
{
    for(auto bone : m_masterNode->m_model->m_rig->m_boneAnims)
    {
        m_boneDeltas[bone.first] = 0.0;
    }
}

void RevisionMerge::setMergeRig(MergeRig _rig)
{
    m_rig = _rig;
}
    
MergeRig RevisionMerge::getMergeRig()
{
    return m_rig;
}

void RevisionMerge::SetBoneDeltas(const std::unordered_map<std::string, float> &_boneDeltas)
{
    m_boneDeltas = _boneDeltas;
}

std::unordered_map<std::string, float> RevisionMerge::getBoneDeltas() const
{
    return m_boneDeltas;
}

std::shared_ptr<RevisionNode> RevisionMerge::getMasterNode()
{
    return m_masterNode;
}

std::shared_ptr<RevisionNode> RevisionMerge::getBranchNode()
{
    return m_branchNode;
}
