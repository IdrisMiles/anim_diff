#include <gtest/gtest.h>

// our includes
#include "revisionUtils.h"
#include "revisionNode.h"

//standard includes
#include <memory>

TEST(DiffTest, RevisionNodeNullCheck) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1;
    std::shared_ptr<RevisionNode> test2;

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

TEST(DiffTest, aiSceneNullCheck) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());

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

TEST(DiffTest, noAnimationCheck) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());

    // load files
    test1->LoadModel("../bin/pighead.obj");
    test2->LoadModel("../bin/GiantTeapot.obj");

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

TEST(DiffTest, matchingTimesCheck) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());

    // load files
    test1->LoadModel("../bin/boblampclean.md5mesh");
    test2->LoadModel("../bin/bony2.dae");

    try 
    {
        RevisionUtils::getRevisionDiff(test1, test2);
        FAIL() << "No matching ticks/duration exception expected";
    }
    catch(const std::string& ex) 
    {
        EXPECT_EQ(std::string("ticks/duration do not match"), ex);
    }
}

TEST(DiffTest, loading2Anims) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1(new RevisionNode());
    std::shared_ptr<RevisionNode> test2(new RevisionNode());

    // load files
    test1->LoadModel("../bin/bony2.dae");
    test2->LoadModel("../bin/bony3.dae");

    try 
    {
        RevisionUtils::getRevisionDiff(test1, test2);
        SUCCEED() << "Loaded 2 animation file";
    }
    catch(const std::string& ex) 
    {
        FAIL() << "failed to load animation files"; 
    }
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
