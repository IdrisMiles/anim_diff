#include <gtest/gtest.h>

// our includes
#include "diffFunctions.h"
#include "revisionUtils.h"
#include "revisionNode.h"


//standard includes
#include <memory>

TEST(RevisionUtilsTest, RevisionNodeNullCheck) { 
    testing::internal::CaptureStdout();

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1;
    std::shared_ptr<RevisionNode> test2;

    testing::internal::GetCapturedStdout();
    try 
    {
        RevisionUtils::getRevisionDiff(test1, test2);
        FAIL() << "Expected null pointer exception";
    }
    catch(const std::string& ex) 
    {
        EXPECT_EQ(std::string("null pointer: revision node"), ex);
    }
}

TEST(RevisionUtilsTest, aiSceneNullCheck) { 
    testing::internal::CaptureStdout();
    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());

    testing::internal::GetCapturedStdout();
    try 
    {
        RevisionUtils::getRevisionDiff(test1, test2);
        FAIL() << "Expected null pointer exception";
    }
    catch(const std::string& ex) 
    {
        EXPECT_EQ(std::string("null pointer: no model loaded"), ex);
    }
}

TEST(RevisionUtilsTest, noAnimationCheck) { 
    testing::internal::CaptureStdout();
    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());

    // load files
    test1->LoadModel("../bin/pighead.obj");
    test2->LoadModel("../bin/GiantTeapot.obj");

    testing::internal::GetCapturedStdout();
    try 
    {
        RevisionUtils::getRevisionDiff(test1, test2);
        FAIL() << "No animation exception expected";
    }
    catch(const std::string& ex) 
    {
        EXPECT_EQ(std::string("no animation present"), ex);
    }
}

TEST(RevisionUtilsTest, loading2Anims) { 
    testing::internal::CaptureStdout();
    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());

    // load files
    test1->LoadModel("../bin/bony2.dae");
    test2->LoadModel("../bin/bony3.dae");

    testing::internal::GetCapturedStdout();
    try 
    {
        RevisionUtils::getRevisionDiff(test1, test2);
        SUCCEED() << "Loaded 2 animation files";
    }
    catch(const std::string& ex) 
    {
        FAIL() << "failed to load animation files"; 
    }
}

TEST(RevisionUtilsTest, checkNodes) { 
    testing::internal::CaptureStdout();
    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());

    // load files
    test1->LoadModel("../bin/bony2.dae");
    test2->LoadModel("../bin/bony3.dae");

    testing::internal::GetCapturedStdout();

    RevisionDiff diff = RevisionUtils::getRevisionDiff(test1, test2);
    
    // check master node is correct
    EXPECT_EQ(test1.get(), diff.getMasterNode().get());

    // now branch
    EXPECT_EQ(test2.get(), diff.getBranchNode().get());
}

// POSITON TESTS //////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(DiffFunctionTest, positionDiffEmpty) { 

    // test data
    std::vector<PosAnim> test1;
    std::vector<PosAnim> test2;

    std::vector<PosAnim> result = DiffFunctions::getPositionDiffs(test1, test2);

    EXPECT_EQ(0, result.size());
}

TEST(DiffFunctionTest, positionDiffTest) { 

    std::vector<PosAnim> master;
    std::vector<PosAnim> branch;

    master.push_back(PosAnim(0.0f, glm::vec3(0,0,0)));
    branch.push_back(PosAnim(0.0f, glm::vec3(1,1,1)));

    std::vector<PosAnim> result = DiffFunctions::getPositionDiffs(master, branch);
    
    EXPECT_EQ(1, result.size());

    EXPECT_EQ(1.0, result[0].pos.x);
    EXPECT_EQ(1.0, result[0].pos.y);
    EXPECT_EQ(1.0, result[0].pos.z);
}

TEST(DiffFunctionTest, positionDiffNotMatchingTimes) { 

    std::vector<PosAnim> master;
    std::vector<PosAnim> branch;

    master.push_back(PosAnim(0.0f, glm::vec3(0,0,0)));
    master.push_back(PosAnim(0.5f, glm::vec3(0,0,0)));
    master.push_back(PosAnim(1.0f, glm::vec3(0,0,0)));

    branch.push_back(PosAnim(0.0f, glm::vec3(0,0,0)));
    branch.push_back(PosAnim(1.0f, glm::vec3(0,0,0)));
    branch.push_back(PosAnim(2.0f, glm::vec3(0,0,0)));

    std::vector<PosAnim> result = DiffFunctions::getPositionDiffs(master, branch);
    
    EXPECT_EQ(4, result.size());
}

TEST(DiffFunctionTest, positionDiffInterpolate) { 

    std::vector<PosAnim> master;
    std::vector<PosAnim> branch;

    master.push_back(PosAnim(0.0f, glm::vec3(0,0,0)));
    master.push_back(PosAnim(2.0f, glm::vec3(1.0,1.0,1.0)));

    branch.push_back(PosAnim(0.0f, glm::vec3(0,0,0)));
    branch.push_back(PosAnim(1.0f, glm::vec3(1.0,1.0,1.0)));
    branch.push_back(PosAnim(2.0f, glm::vec3(1.0,1.0,1.0)));

    std::vector<PosAnim> result = DiffFunctions::getPositionDiffs(master, branch);
    
    EXPECT_EQ(3, result.size());

    EXPECT_EQ(0.5, result[1].pos.x);
    EXPECT_EQ(0.5, result[1].pos.y);
    EXPECT_EQ(0.5, result[1].pos.z);
}

// SCALE TESTS //////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(DiffFunctionTest, scaleDiffEmpty) { 

    // test data
    std::vector<ScaleAnim> test1;
    std::vector<ScaleAnim> test2;

    std::vector<ScaleAnim> result = DiffFunctions::getScaleDiffs(test1, test2);

    EXPECT_EQ(0, result.size());
}

TEST(DiffFunctionTest, scaleDiffTest) { 

    std::vector<ScaleAnim> master;
    std::vector<ScaleAnim> branch;

    master.push_back(ScaleAnim(0.0f, glm::vec3(0,0,0)));
    branch.push_back(ScaleAnim(0.0f, glm::vec3(1,1,1)));

    std::vector<ScaleAnim> result = DiffFunctions::getScaleDiffs(master, branch);
    
    EXPECT_EQ(1, result.size());

    EXPECT_EQ(1.0, result[0].scale.x);
    EXPECT_EQ(1.0, result[0].scale.y);
    EXPECT_EQ(1.0, result[0].scale.z);
}

TEST(DiffFunctionTest, scaleDiffNotMatchingTimes) { 

    std::vector<ScaleAnim> master;
    std::vector<ScaleAnim> branch;

    master.push_back(ScaleAnim(0.0f, glm::vec3(0,0,0)));
    master.push_back(ScaleAnim(0.5f, glm::vec3(0,0,0)));
    master.push_back(ScaleAnim(1.0f, glm::vec3(0,0,0)));

    branch.push_back(ScaleAnim(0.0f, glm::vec3(0,0,0)));
    branch.push_back(ScaleAnim(1.0f, glm::vec3(0,0,0)));
    branch.push_back(ScaleAnim(2.0f, glm::vec3(0,0,0)));

    std::vector<ScaleAnim> result = DiffFunctions::getScaleDiffs(master, branch);
    
    EXPECT_EQ(4, result.size());
}

TEST(DiffFunctionTest, scaleDiffInterpolate) { 

    std::vector<ScaleAnim> master;
    std::vector<ScaleAnim> branch;

    master.push_back(ScaleAnim(0.0f, glm::vec3(0,0,0)));
    master.push_back(ScaleAnim(2.0f, glm::vec3(1.0,1.0,1.0)));

    branch.push_back(ScaleAnim(0.0f, glm::vec3(0,0,0)));
    branch.push_back(ScaleAnim(1.0f, glm::vec3(1.0,1.0,1.0)));
    branch.push_back(ScaleAnim(2.0f, glm::vec3(1.0,1.0,1.0)));

    std::vector<ScaleAnim> result = DiffFunctions::getScaleDiffs(master, branch);
    
    EXPECT_EQ(3, result.size());

    EXPECT_EQ(0.5, result[1].scale.x);
    EXPECT_EQ(0.5, result[1].scale.y);
    EXPECT_EQ(0.5, result[1].scale.z);
}

// ROTATION TESTS //////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST(DiffFunctionTest, rotationDiffEmpty) { 

    // test data
    std::vector<RotAnim> test1;
    std::vector<RotAnim> test2;

    std::vector<RotAnim> result = DiffFunctions::getRotationDiffs(test1, test2);

    EXPECT_EQ(0, result.size());
}

TEST(DiffFunctionTest, rotationDiffTest) { 

    std::vector<RotAnim> master;
    std::vector<RotAnim> branch;

    master.push_back(RotAnim(0.0f, glm::quat(0,1,0, 0)));
    branch.push_back(RotAnim(0.0f, glm::quat(0,1,0, 1)));

    std::vector<RotAnim> result = DiffFunctions::getRotationDiffs(master, branch);
    
    EXPECT_EQ(1, result.size());

    EXPECT_EQ(0.0, result[0].rot.x);
    EXPECT_EQ(1.0, result[0].rot.y);
    EXPECT_EQ(0.0, result[0].rot.z);
    EXPECT_EQ(1.0, result[0].rot.z);
}

TEST(DiffFunctionTest, rotationDiffNotMatchingTimes) { 

    std::vector<RotAnim> master;
    std::vector<RotAnim> branch;

    master.push_back(RotAnim(0.0f, glm::quat(0,0,0,0)));
    master.push_back(RotAnim(0.5f, glm::quat(0,0,0,0)));
    master.push_back(RotAnim(1.0f, glm::quat(0,0,0,0)));

    branch.push_back(RotAnim(0.0f, glm::quat(0,0,0,0)));
    branch.push_back(RotAnim(1.0f, glm::quat(0,0,0,0)));
    branch.push_back(RotAnim(2.0f, glm::quat(0,0,0,0)));

    std::vector<RotAnim> result = DiffFunctions::getRotationDiffs(master, branch);
    
    EXPECT_EQ(4, result.size());
}

TEST(DiffFunctionTest, rotationDiffInterpolate) { 

    std::vector<RotAnim> master;
    std::vector<RotAnim> branch;

    master.push_back(RotAnim(0.0f, glm::quat(0,0,0,0)));
    master.push_back(RotAnim(2.0f, glm::quat(1.0,0.0,0.0,1.0)));

    branch.push_back(RotAnim(0.0f, glm::quat(0,0,0,0)));
    branch.push_back(RotAnim(1.0f, glm::quat(0.5,0.0,0.0,0.5)));
    branch.push_back(RotAnim(2.0f, glm::quat(1.0,0.0,0.0,0.0)));

    std::vector<RotAnim> result = DiffFunctions::getRotationDiffs(master, branch);
    
    EXPECT_EQ(3, result.size());

    EXPECT_EQ(0.0, result[1].rot.x);
    EXPECT_EQ(0.0, result[1].rot.y);
    EXPECT_EQ(0.0, result[1].rot.z);
    EXPECT_EQ(0.0, result[1].rot.w);
}

// MAIN ///////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
