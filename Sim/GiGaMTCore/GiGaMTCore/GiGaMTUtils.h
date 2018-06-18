#include <condition_variable>
#include <mutex>

class GiGaMTBarrier
{
private:
  std::mutex _mutex;
  std::condition_variable _cv;
  std::size_t m_n_waiting;

public:
  explicit GiGaMTBarrier( std::size_t count ) : m_n_waiting(count){}
  void wait()
  {
    std::unique_lock<std::mutex> lock{_mutex};
    if ( --m_n_waiting == 0 ) {
      _cv.notify_all();
    } else {
      _cv.wait( lock, [this] { return m_n_waiting == 0; } );
    }
  }
  GiGaMTBarrier()                       = delete;
  GiGaMTBarrier( const GiGaMTBarrier& ) = delete;
  GiGaMTBarrier( GiGaMTBarrier&& )      = delete;
};
