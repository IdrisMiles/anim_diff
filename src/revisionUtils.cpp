#include "revisionUtils.h"

#include "revisionNode.h"
#include "revisionDiff.h"
#include <assimp/scene.h>

#include <iostream>

RevisionUtils::RevisionUtils()
{

}

RevisionUtils::~RevisionUtils()
{

}

RevisionDiff RevisionUtils::getDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch)
{   
    if(!_master && !_branch) throw std::string("null pointer: revision node");

    // public members, so so bad and dirty raw pointers
    const aiScene *pMasterScene = _master->m_model->m_scene;
    const aiScene *pBranchScene = _branch->m_model->m_scene;

    //create our return 
    RevisionDiff revDiff;

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
        throw std::string("null pointer: scene");
    }

    return revDiff;
}