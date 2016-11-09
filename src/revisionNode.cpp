#include "include/revisionNode.h"

RevisionNode::RevisionNode()
{

}


RevisionNode::~RevisionNode()
{

}


void RevisionNode::LoadModel(const std::string &_modelFile)
{
    m_model.reset(new Model());
    m_model->LoadModel(_modelFile);
}
