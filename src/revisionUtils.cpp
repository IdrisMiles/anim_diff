#include "revisionUtils.h"

#include "revisionNode.h"
#include <assimp/scene.h>

#include <iostream>

RevisionUtils::RevisionUtils()
{

}

RevisionUtils::~RevisionUtils()
{

}

RevisionDiff RevisionUtils::getRevisionDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch)
{   
    if(!_master && !_branch) throw std::string("null pointer: revision node");

    if(!_master->m_model && !_master->m_model) throw std::string("null pointer: no model loaded");
    // public members, so so bad and dirty raw pointers
    const aiScene *pMasterScene = _master->m_model->m_scene ? _master->m_model->m_scene : nullptr;
    const aiScene *pBranchScene = _branch->m_model->m_scene ? _branch->m_model->m_scene : nullptr;

    if(pMasterScene != nullptr && pBranchScene != nullptr)
    {
        if(pMasterScene->HasAnimations() && pBranchScene->HasAnimations())
        {   
            //just look at the first animation for now
            double master_ticksPerSecond = pMasterScene->mAnimations[0]->mTicksPerSecond;
            double master_animationDuration = pMasterScene->mAnimations[0]->mDuration;

            double branch_ticksPerSecond = pBranchScene->mAnimations[0]->mTicksPerSecond;
            double branch_animationDuration = pBranchScene->mAnimations[0]->mDuration;

            // compare animation times.
            if((master_ticksPerSecond == branch_ticksPerSecond) && (master_animationDuration == branch_animationDuration))
            {
                // whoooooo
                // we have something that we can actually compare
                //diff(pMasterScene, pBranchScene);
            }
            else
            {
                throw std::string("ticks/duration do not match");
            }
        }
        else
        {
            throw std::string("no animation present");
        }
    }
    else
    {
        // don't think it'll ever get here.. supposedly
        throw std::string("null pointer: rig");
    }

    return RevisionDiff();
}

DiffRig RevisionUtils::diff(std::shared_ptr<ModelRig> master, std::shared_ptr<ModelRig> branch)
{
    return DiffRig();
}