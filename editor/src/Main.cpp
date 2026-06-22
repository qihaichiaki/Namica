#include <namica/Test.h>
#include <thread>

int main()
{
    namica::printCore();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}