#include <mutex>
#include <condition_variable>


class GiGaMTBarrier
{
private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::size_t _count;
public:
    explicit GiGaMTBarrier(std::size_t count) : _count{count} { }
    void wait()
    {
        std::unique_lock<std::mutex> lock{_mutex};
        if (--_count == 0) {
            _cv.notify_all();
        } else {
            _cv.wait(lock, [this] { return _count == 0; });
        }
    }
    GiGaMTBarrier() = delete;
    GiGaMTBarrier(const GiGaMTBarrier &) = delete;
    GiGaMTBarrier(GiGaMTBarrier &&) = delete;
};
