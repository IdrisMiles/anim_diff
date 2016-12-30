#include "revisionUtils.h"
#include "revisionNode.h"
#include <iostream>

RevisionUtils::RevisionUtils()
: DiffFunctions()
{

}

RevisionUtils::~RevisionUtils()
{

}

RevisionDiff RevisionUtils::getRevisionDiff(std::shared_ptr<RevisionNode> _master, std::shared_ptr<RevisionNode> _branch)
{   
    RevisionDiff diff(_master, _branch);

    if(!_master && !_branch) throw std::string("null pointer: revision node");

    if(!_master->m_model && !_branch->m_model) throw std::string("null pointer: no model loaded");
    
    if(_master->m_model->m_animExists && _branch->m_model->m_animExists)
    {   

        // public members, so so bad and dirty raw pointers
        ModelRig masterRig = *(_master->m_model->m_rig);
        ModelRig branchRig = *(_branch->m_model->m_rig);

        //TODO check to see if rigs and bones match


        //create a DiffRig
        DiffRig diffRig;

        //just look at the first animation for now
        double masterTicks = masterRig.m_ticks;
        double masterDuration = masterRig.m_duration;

        double branchDuration = branchRig.m_duration;

        // use master tick speed, and duration is the larger of the two 
        diffRig.m_ticks = masterTicks;
        diffRig.m_duration = masterDuration > branchDuration ? masterDuration : branchDuration;

        // Then diff the animation info on a per tick basis for their durations
        getAnimDiff(masterRig, branchRig, diffRig);

        //return 
        diff.setDiffRig(diffRig);
        return diff;
    }
    else
    {
        throw std::string("no animation present");
    }

    return diff;
}

RevisionNode RevisionUtils::getRevisionNodeForDiff(std::shared_ptr<RevisionDiff> _diff)
{
    RevisionNode node;

    node.m_model = std::make_shared<Model>(getModelFromDiff(_diff));

    return node;
}

Model RevisionUtils::getModelFromDiff(std::shared_ptr<RevisionDiff> _diff)
{
    // this is bad
    Model newModel;
    Model masterModel = *(_diff->getMasterNode()->m_model.get());

    // copy across, deep copy yo!
    newModel.m_ticksPerSecond       = masterModel.m_ticksPerSecond;
    newModel.m_animationDuration    = masterModel.m_animationDuration;
    newModel.m_numAnimations        = masterModel.m_numAnimations;
    newModel.m_animationID          = masterModel.m_animationID;
    newModel.m_meshVerts            = masterModel.m_meshVerts;
    newModel.m_meshNorms            = masterModel.m_meshNorms;
    newModel.m_meshTris             = masterModel.m_meshTris;
    newModel.m_meshBoneWeights      = masterModel.m_meshBoneWeights;
    newModel.m_colour               = masterModel.m_colour;
    newModel.m_rigVerts             = masterModel.m_rigVerts;
    newModel.m_rigNorms             = masterModel.m_rigNorms;
    newModel.m_rigElements          = masterModel.m_rigElements;
    newModel.m_rigBoneWeights       = masterModel.m_rigBoneWeights;
    newModel.m_rigJointColours      = masterModel.m_rigJointColours;
    newModel.m_animExists           = masterModel.m_animExists;
    newModel.m_boneInfo             = masterModel.m_boneInfo;
    newModel.m_boneMapping          = masterModel.m_boneMapping;
    newModel.m_globalInverseTransform = masterModel.m_globalInverseTransform;

    /////////// Copy Rig and diff data
    newModel.m_rig->m_ticks = masterModel.m_rig->m_ticks;
    newModel.m_rig->m_duration = masterModel.m_rig->m_duration;

    // copy rig recursivley
    newModel.m_rig->m_rootBone = std::shared_ptr<Bone>(new Bone());
    copyRigStructure(newModel.m_rig, _diff->getDiffRig(), nullptr, newModel.m_rig->m_rootBone, masterModel.m_rig->m_rootBone);

    return newModel;
}

void RevisionUtils::copyRigStructure(std::shared_ptr<ModelRig> pRig, 
                                        DiffRig _diffRig, 
                                        std::shared_ptr<Bone> pParentBone, 
                                        std::shared_ptr<Bone> pNewBone, 
                                        std::shared_ptr<Bone> pOldBone)
{
    // copy data of bone
    pNewBone->m_parent = pParentBone;
    pNewBone->m_name = pOldBone->m_name;
    pNewBone->m_boneID = pOldBone->m_boneID;
    pNewBone->m_transform = pOldBone->m_transform;
    pNewBone->m_boneOffset = pOldBone->m_boneOffset;
    pNewBone->m_currentTransform = pOldBone->m_currentTransform;

    // make m_boneAnim
    BoneAnim boneAnim;
    boneAnim.m_name = pNewBone->m_name;

    // find matching bone
    auto boneDiff = _diffRig.m_boneAnimDiffs.find(boneAnim.m_name);

    if (boneDiff == _diffRig.m_boneAnimDiffs.end()) 
    {
        std::cout << "no diff bone found for: " << boneAnim.m_name << "\n"; 
        return;
    }
    // pos
    for(auto diffAnim : boneDiff.m_posAnimDeltas)
    {
        PosAnim tmp;

        tmp.time = diffAnim.time;
        tmp.pos = diffAnim.pos + ();

        boneAnim.m_posAnim.push_back(tmp);
    }

    // scale
    //for()
    //{
    //    boneAnim.m_scaleAnim.push_back();
    //}

    // rot
    //for()
    //{
    //    boneAnim.m_rotAnim.push_back();
    //}
    
    //add to map
    pNewBone->m_boneAnim = & boneAnim;
    pRig->m_boneAnims.insert({pNewBone->m_name, boneAnim});

    //do the same for the children
    for(unsigned int i =0; i < pOldBone->m_children.size(); ++i )
    {
        pNewBone->m_children.push_back(std::shared_ptr<Bone>(new Bone()));
        copyRigStructure( pRig, _diffRig, pNewBone, pNewBone->m_children[i], pOldBone->m_children[i]);
    }
}
