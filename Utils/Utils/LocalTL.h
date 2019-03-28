#pragma once

// Thread Local storage details are in this header file
#include <atomic>
#include <type_traits>
#include <vector>

template <typename T>
using IsFundamental = typename std::enable_if<std::is_fundamental<T>::value>::type;
template <typename T>
using IsPointer = typename std::enable_if<std::is_pointer<T>::value>::type;

// Primary template for the storage interface
template <typename T, typename Sfinae = void>
class LocalTLImpl
{
public:
  inline void initialize( unsigned int id )
  {
    if ( storage().size() <= id ) storage().resize( id + 1, nullptr );
    if ( !storage().at( id ) ) storage().at( id ) = new T{};
  }

  inline void destroy( unsigned int id )
  {
    if ( storage().size() > id && storage().at( id ) ) {
      delete storage().at( id );
      storage().at( id ) = nullptr;
    }
  }

  inline T& get( unsigned int id ) const { return *storage().at( id ); }

private:
  typedef std::vector<T*> storage_container;
  storage_container& storage() const
  {
    static thread_local storage_container _storage;
    return _storage;
  }
};

// Specialization for pointer types to avoid additional new
template <typename T>
class LocalTLImpl<T, IsPointer<T>>
{
public:
  inline void initialize( unsigned int id )
  {
    if ( storage().size() <= id ) storage().resize( id + 1, nullptr );
  };

  inline void destroy( unsigned int id )
  {
    if ( storage().size() > id && storage().at( id ) ) {
      storage().at( id ) = nullptr;
    }
  };

  inline T& get( unsigned int id ) const { return storage().at( id ); };

private:
  typedef std::vector<T> storage_container;
  storage_container& storage() const
  {
    static thread_local storage_container _storage;
    return _storage;
  }
};

// Specialization for fundamental types which are stored
// directly in the vector for improved performance
template <typename T>
class LocalTLImpl<T, IsFundamental<T>>
{
public:
  inline void initialize( unsigned int id )
  {
    if ( storage().size() <= id ) storage().resize( id + 1, T{} );
  };

  inline void destroy( unsigned int id )
  {
    if ( storage().size() > id && storage().at( id ) ) {
      storage().at( id ) = T{};
    }
  };

  inline T& get( unsigned int id ) const { return storage().at( id ); }

private:
  typedef std::vector<T> storage_container;
  storage_container& storage() const
  {
    static thread_local storage_container _storage;
    return _storage;
  }
};

// A templated cache to store a thread-private data of type VALTYPE.
template <class T>
class LocalTL
{
public:
  LocalTL();
  // Default constructor

  LocalTL( const T& v );
  // Construct cache object with initial value

  virtual ~LocalTL();
  // Default destructor

  inline T& get() const
  {
    impl.initialize( m_id );
    return impl.get( m_id );
  }
  inline T& operator()() const { return get(); }
  // Gets reference to cached value of this threads

  inline void put( const T& val ) const;
  // Sets this thread cached value to val

  LocalTL( const LocalTL& rhs );
  LocalTL& operator=( const LocalTL& rhs );
  T operator->() { return get(); }

private:
  int m_id;
  mutable LocalTLImpl<T> impl;
  static std::atomic_uint sm_inst_ctr;
  static std::atomic_uint sm_dstr_ctr;
};

// Initialize static members
template <class V>
std::atomic_uint LocalTL<V>::sm_inst_ctr{0};

template <class V>
std::atomic_uint LocalTL<V>::sm_dstr_ctr{0};

template <class V>
LocalTL<V>::LocalTL()
{
  // Assign the current global counter to this instance
  // The counter is defined as atomic so should be fine
  m_id = sm_inst_ctr++;
}

template <class V>
LocalTL<V>::LocalTL( const LocalTL<V>& rhs )
{
  if ( this == &rhs ) return;
  m_id = sm_inst_ctr++;
  // Force copy of cached data
  V aCopy = rhs.get();
  put( aCopy );
}

template <class V>
LocalTL<V>& LocalTL<V>::operator=( const LocalTL<V>& rhs )
{
  if ( this == &rhs ) return *this;
  // Force copy of cached data
  V aCopy = rhs.get();
  put( aCopy );
  return *this;
}

template <class V>
LocalTL<V>::LocalTL( const V& v )
{
  m_id = sm_inst_ctr++;
  put( v );
}

template <class V>
LocalTL<V>::~LocalTL()
{
  // Need to lock this as the two counters are accessed
  impl.destroy( m_id );
}

template <class V>
void LocalTL<V>::put( const V& val ) const
{
  get() = val;
}

template <typename T>
class GarbageBin
{
  static_assert( std::is_pointer<T>::value, "blub" );

public:
  static void Add( T obj ) { Instance().push_back( obj ); }
  ~GarbageBin()
  {
    for ( auto& obj : _store ) {
      delete _store;
    }
  }

private:
  static GarbageBin& Instance()
  {
    static GarbageBin _instance{};
    return _instance;
  }
  GarbageBin() = default;
  std::vector<T> _store;
};
