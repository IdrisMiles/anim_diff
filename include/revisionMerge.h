#ifndef REVISIONMERGE_H
#define REVISIONMERGE_H

#include <vector>
#include "mergeRig.h"

class RevisionNode;

class RevisionMerge 
{
public:
    
    RevisionMerge(std::shared_ptr<RevisionNode> _master, 
                    std::shared_ptr<RevisionNode> _branch,
                    std::shared_ptr<RevisionNode> _parent);
    
    ~RevisionMerge(){};

    void setMergeRig(MergeRig _rig);
    MergeRig getMergeRig();

    void SetBoneDeltas(const std::unordered_map<std::string, float> &_boneDeltas);
    std::unordered_map<std::string, float> getBoneDeltas() const;

    std::shared_ptr<RevisionNode> getMasterNode();
    std::shared_ptr<RevisionNode> getBranchNode();

private:
    std::shared_ptr<RevisionNode> m_masterNode;
    std::shared_ptr<RevisionNode> m_branchNode;
    std::shared_ptr<RevisionNode> m_parentNode;

    MergeRig m_rig;
    std::unordered_map<std::string, float> m_boneDeltas;
};

#endif // REVISIONMERGE_H
