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
        EXPECT_EQ(std::string("null pointer: scene"), ex);
    }
}

 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
