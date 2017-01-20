#ifndef VIEWERUTILITIES_H
#define VIEWERUTILITIES_H

#include <vector>
#include <map>


#include "vertexBoneData.h"
#include "boneTransform.h"


// ASSIMP includes
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "include/modelRig.h"
#include "include/diffRig.h"
#include "include/mergeRig.h"
#include "include/bone.h"



namespace ViewerUtilities
{
    //-----------------------------------------------------------------------------------------------------------------------------
    // Assimp viewer stuff
    /// @brief Method to convert Assimps aiMatrix4x4 class to a glm::mat4 class. This is useful for sending assimps matrices to opengl.
    /// @param m : The matrix to convert.
    glm::mat4 ConvertToGlmMat(const aiMatrix4x4 &m);


    /// @brief Method to find the animation node with the name matching NodeName.
    /// @param _pAnimation : The animation we want to use.
    /// @param _nodeName : The name of the animated node in the animation we are trying to find.
    /// @return const aiNodeAnim* : returns the aiNodeAnim with the same name as _nodeName in _pAnimation if found, else returns NULL.
    const aiNodeAnim* FindNodeAnim(const aiAnimation* _pAnimation, const std::string _nodeName);


    const aiBone* GetBone(const aiScene *_aiScene, std::string _name);

    //-----------------------------------------------------------------------------------------------------------------------------
    // Revision viewer stuff
    void ReadNodeHierarchy(const std::map<std::string, unsigned int> &_boneMapping, std::vector<glm::mat4> &_boneInfo, const glm::mat4 _globalInverseTransform, const float _animationTime, std::shared_ptr<ModelRig> _pRig, std::shared_ptr<Bone> _pBone, const glm::mat4& _parentTransform);

    BoneAnim FindBoneAnim(ModelRig _pRig, std::string _nodeName);

    void CalcInterpolatedRotation(glm::quat& _out, const float _animationTime, const BoneAnim* _pBoneAnin);

    void CalcInterpolatedPosition(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim);

    void CalcInterpolatedScaling(glm::vec3& _out, const float _animationTime, const BoneAnim* _boneAnim);

    uint FindRotationKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim);

    uint FindPositionKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim);

    uint FindScalingKeyFrame(const float _animationTime, const BoneAnim* _pBoneAnim);

    BoneAnim TransferAnim(const aiNodeAnim *_pNodeAnim);

    void CopyRigStructure(const std::map<std::string, unsigned int> &_boneMapping, const aiScene *_aiScene, aiNode *_aiNode, std::shared_ptr<ModelRig> _rig, std::shared_ptr<Bone> _parentBone, const glm::mat4 &_parentTransform);


    //-----------------------------------------------------------------------------------------------------------------------------
    // Diff viewer stuff
    void ColourBoneDifferences(const std::map<std::string, unsigned int> &_boneMapping, const float _animationTime, const std::unordered_map<std::string, float> &_boneDeltas, std::shared_ptr<ModelRig> _pMasterRig, std::shared_ptr<Bone> _pMasterBone, DiffRig _pDiffRig, std::vector<glm::vec3> &_rigJointColour);

    void ReadNodeHierarchyDiff(const std::map<std::string, unsigned int> &_boneMapping, std::vector<glm::mat4> &_boneInfo, const glm::mat4 _globalInverseTransform, const float _animationTime, const std::unordered_map<std::string, float> &_boneDeltas, std::shared_ptr<ModelRig> _pMasterRig, std::shared_ptr<Bone> _pMasterBone, DiffRig _pDiffRig, const glm::mat4& _parentTransform);

    void CalcInterpolatedRotation(glm::quat& _out, const float _animationTime, const BoneAnimDiff* _pBoneAninDiff);

    void CalcInterpolatedPosition(glm::vec3& _out, const float _animationTime, const BoneAnimDiff* _boneAnimDiff);

    void CalcInterpolatedScaling(glm::vec3& _out, const float _animationTime, const BoneAnimDiff* _boneAnimDiff);

    uint FindRotationKeyFrame(const float _animationTime, const BoneAnimDiff* _pBoneAnimDiff);

    uint FindPositionKeyFrame(const float _animationTime, const BoneAnimDiff* _pBoneAnimDiff);

    uint FindScalingKeyFrame(const float _animationTime, const BoneAnimDiff* _pBoneAnimDiff);


    //-----------------------------------------------------------------------------------------------------------------------------
    // Merged viewer stuff
    void ColourBoneMerges(const std::map<std::string, unsigned int> &_boneMapping, const float _animationTime, const std::unordered_map<std::string, float> &_boneDeltas, std::shared_ptr<ModelRig> _pMasterRig, std::shared_ptr<Bone> _pMasterBone, MergeRig _pMergeRig, std::vector<glm::vec3> &_rigJointColour);

    void ReadNodeHierarchyMerge(const std::map<std::string, unsigned int> &_boneMapping, std::vector<glm::mat4> &_boneInfo, const glm::mat4 _globalInverseTransform, const float _animationTime, const std::unordered_map<std::string, float> &_boneDeltas, std::shared_ptr<ModelRig> _pMasterRig, std::shared_ptr<Bone> _pMasterBone, MergeRig _pMergeRig, const glm::mat4& _parentTransform);


    void GenerateScalingMatrix(glm::mat4 &scalingMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimDiff* pBoneAnimDiff);
    void GenerateRotationMatrix(glm::mat4 &rotationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimDiff* pBoneAnimDiff);
    void GenerateTranslationMatrix(glm::mat4 &translationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimDiff* pBoneAnimDiff);

    void GenerateScalingMatrix(glm::mat4 &scalingMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimMerge boneAnimMerge, bool &master, bool &branch);
    void GenerateRotationMatrix(glm::mat4 &rotationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimMerge boneAnimMerge, bool &master, bool &branch);
    void GenerateTranslationMatrix(glm::mat4 &translationMat, float _animationTime, float _delta, const BoneAnim *pMasterBoneAnim, const BoneAnimMerge boneAnimMerge, bool &master, bool &branch);
}

#endif // VIEWERUTILITIES_H
