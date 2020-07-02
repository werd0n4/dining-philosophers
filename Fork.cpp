#include<atomic>

class Fork
{
    public:
    std::mutex mtx;
    std::condition_variable cv;
    bool free;
    // WINDOW* window;
};