

class Fork
{
    std::mutex mtx;
    std::condition_variable cv;
};