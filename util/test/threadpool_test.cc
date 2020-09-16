#include "gtest/gtest.h"
#include <threadpool.h>
namespace SB
{
    namespace util
    {
        /**
     * @brief Construct a new TEST object
     * 
     */
        int num = 1000;
        int inc = 0;
        std::vector<int> record(num, 0);
        void func(int index)
        {
            record[index]++;
        }
        TEST(ThreadPoolTest, Simple)
        {

            {
                ThreadPool pool("test", 5, 1000);
                pool.start();
                while (inc < num)
                {
                    Task c = std::bind(func, inc);
                    pool.put(c);
                    inc++;
                }
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }

            for (int i = 0; i < record.size(); ++i)
            {
                ASSERT_EQ(record[i], 1) << i;
            }
        }

    } // namespace util

} // namespace SB
using namespace SB::util;
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}