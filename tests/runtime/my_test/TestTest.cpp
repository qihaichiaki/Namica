#include "utils/TestUtils.h"
#include "namica/Test.h"

TEST_F(NamicaRuntimeTest, my_test)
{
    EXPECT_EQ(namica::printCore(), 0);
}