#include <gtest/gtest.h>

// our includes
#include "revisionUtils.h"
#include "revisionNode.h"

//standard includes
#include <memory>

 
TEST(DiffTest, HelloWorld) { 

    // testing the testing stuff
    std::shared_ptr<RevisionNode> test1;
    std::shared_ptr<RevisionNode> test2;

    RevisionUtils::getDiff(test1, test2);

    std::string test = "hello";
    ASSERT_EQ(std::string("hello"), test);
}

 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
