#include "revisionUtils.h"

#include "revisionNode.h"
#include <assimp/scene.h>

RevisionUtils::RevisionUtils()
{

}

RevisionUtils::~RevisionUtils()
{

}

void RevisionUtils::getDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch)
{   
    // public members, so so bad and dirty raw pointers
    const aiScene *masterScene = _master->m_model->m_scene;
    const aiScene *branchScene = _branch->m_model->m_scene;
}