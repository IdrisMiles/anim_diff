#include <gtest/gtest.h>

// our includes
#include "revisionUtils.h"
#include "revisionNode.h"
#include "revisionDiff.h"

//standard includes
#include <memory>

TEST(DiffTest, RevisionNodeNullCheck) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1;
    std::shared_ptr<RevisionNode> test2;
    RevisionDiff diff;

    try 
    {
        diff = RevisionUtils::getDiff(test1, test2);
        FAIL() << "Expected null pointer exception";
    }
    catch(const std::string& ex) 
    {
        EXPECT_EQ(std::string("null pointer: revision node"), ex);
    }
}

TEST(DiffTest, aiSceneNullCheck) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());
    RevisionDiff diff;

    try 
    {
        diff = RevisionUtils::getDiff(test1, test2);
        FAIL() << "Expected null pointer exception";
    }
    catch(const std::string& ex) 
    {
        EXPECT_EQ(std::string("null pointer: no model loaded"), ex);
    }
}

TEST(DiffTest, noAnimationCheck) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());
    RevisionDiff diff;

    // load files
    test1->LoadModel("../bin/pighead.obj");
    test2->LoadModel("../bin/GiantTeapot.obj");

    try 
    {
        diff = RevisionUtils::getDiff(test1, test2);
        FAIL() << "No animation exception expected";
    }
    catch(const std::string& ex) 
    {
        EXPECT_EQ(std::string("no animation present"), ex);
    }
}

TEST(DiffTest, matchingTimesCheck) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());
    RevisionDiff diff;

    // load files
    test1->LoadModel("../bin/boblampclean.md5mesh");
    test2->LoadModel("../bin/BaseMesh_Anim.fbx");

    try 
    {
        diff = RevisionUtils::getDiff(test1, test2);
        FAIL() << "No matching ticks/duration exception expected";
    }
    catch(const std::string& ex) 
    {
        EXPECT_EQ(std::string("ticks/duration do not match"), ex);
    }
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
