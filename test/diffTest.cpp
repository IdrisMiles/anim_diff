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

TEST(DiffFunctionTest, positionDiffEmpty) { 

    // test data
    std::vector<PosAnim> test1;
    std::vector<PosAnim> test2;

    std::vector<PosAnim> result = DiffFunctions::getPositionDiffs(test1, test2);

    EXPECT_EQ(0, result.size());
}


int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
