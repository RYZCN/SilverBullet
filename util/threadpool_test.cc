#include <threadpool.h>
#include "gtest/gtest.h"
namespace SB
{

    TEST(ThreadPoolTest, Simple)
    {
        int num = 5;
        int inc = 0;
        std::vector<int> record(num, 0);
        ThreadPool pool("test", 5, 1000);
        auto func = [&record](const int inc) { ASSERT_EQ(record[inc], 0);record[inc]=1; };
        while (inc < num)
        {
            pool.run(std::bind(func, inc));
            inc++;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
        for (int i = 0; i < record.size(); ++i)
        {
            ASSERT_EQ(record[i], 1) << i;
        }
        
    }

} // namespace SB

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
