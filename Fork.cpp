
struct Fork
{
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic_bool free;
};