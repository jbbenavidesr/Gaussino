#include "CLHEP/Random/MixMaxRng.h"
#include "NewRnd/RndGlobal.h"
#include "TH1D.h"
#include <future>
#include <iostream>
#include <thread>
#include <vector>

#define NTRHEADS 50
#define NNumbers 50000

std::vector<int>* refvalues        = nullptr;
std::vector<double>* refhistvalues = nullptr;
TH1D* hist                         = nullptr;

bool essentiallyEqual( float a, float b, float epsilon = 0.01 )
{
  return fabs( a - b ) <= ( ( fabs( a ) > fabs( b ) ? fabs( b ) : fabs( a ) ) * epsilon );
}

void run_thread( std::promise<bool>* prom )
{
  gRandom->SetSeed( 42 );
  for ( int i = 0; i < NNumbers; i++ ) {
    if ( refvalues->at( i ) != gRandom->Poisson( 10 ) ) {
      prom->set_value( false );
      return;
    }
  }
  prom->set_value( true );
}

void run_thread_local( std::promise<bool>* prom )
{
  CLHEP::MixMaxRng engine;
  engine.setSeed( 42 );
  ThreadLocalgRandom::Guard guard( engine );
  for ( int i = 0; i < NNumbers; i++ ) {
    if ( refvalues->at( i ) != gRandom->Poisson( 10 ) ) {
      prom->set_value( false );
      return;
    }
  }
  prom->set_value( true );
}

void hist_thread( std::promise<bool>* prom )
{
  gRandom->SetSeed( 42 );
  for ( int i = 0; i < NNumbers; i++ ) {
    if ( !essentiallyEqual( refhistvalues->at( i ), hist->GetRandom() ) ) {
      prom->set_value( false );
      return;
    }
  }
  prom->set_value( true );
}

void hist_thread_local( std::promise<bool>* prom )
{
  CLHEP::MixMaxRng engine;
  engine.setSeed( 42 );
  ThreadLocalgRandom::Guard guard( engine );
  for ( int i = 0; i < NNumbers; i++ ) {
    if ( !essentiallyEqual( refhistvalues->at( i ), hist->GetRandom() ) ) {
      prom->set_value( false );
      return;
    }
  }
  prom->set_value( true );
}

int main()
{
  // Reference test using standard gRandom setup
  hist = new TH1D( "", "", 100, -5, 5 );
  for ( int i = 0; i < 100000; i++ ) {
    hist->Fill( gRandom->Gaus() );
  }
  refvalues = new std::vector<int>{};
  gRandom->SetSeed( 42 );
  for ( int i = 0; i < NNumbers; i++ ) {
    refvalues->push_back( gRandom->Poisson( 10 ) );
  }
  refhistvalues = new std::vector<double>{};
  gRandom->SetSeed( 42 );
  for ( int i = 0; i < NNumbers; i++ ) {
    refhistvalues->push_back( hist->GetRandom() );
  }
  std::vector<std::future<bool>> futures;
  std::vector<std::thread> threads;
  for ( int i = 0; i < NTRHEADS; i++ ) {
    auto prom = new std::promise<bool>{};
    futures.push_back( prom->get_future() );
    threads.emplace_back( run_thread, prom );
  }
  bool ret = true;
  for ( auto& fut : futures ) {
    ret &= fut.get();
  }
  for ( auto& t : threads ) t.join();
  if ( ret ) {
    std::cout << "ERROR: Sequences are unexpectly equal in normal gRandom test." << std::endl;
  }
  futures.clear();
  threads.clear();

  for ( int i = 0; i < NTRHEADS; i++ ) {
    auto prom = new std::promise<bool>{};
    futures.push_back( prom->get_future() );
    threads.emplace_back( hist_thread, prom );
  }
  ret = true;
  for ( auto& fut : futures ) {
    ret &= fut.get();
  }
  for ( auto& t : threads ) t.join();
  if ( ret ) {
    std::cout << "ERROR: Sequences are unexpectly equal in normal gRandom histogram test." << std::endl;
  }
  delete refvalues;
  delete refhistvalues;
  futures.clear();
  threads.clear();

  // threadlocal gRandom test
  {
    CLHEP::MixMaxRng engine;
    engine.setSeed( 42 );
    ThreadLocalgRandom::Guard guard( engine );

    // Reference test using standard gRandom setup
    refvalues = new std::vector<int>{};
    for ( int i = 0; i < NNumbers; i++ ) {
      refvalues->push_back( gRandom->Poisson( 10 ) );
    }
    for ( int i = 0; i < NTRHEADS; i++ ) {
      auto prom = new std::promise<bool>{};
      futures.push_back( prom->get_future() );
      threads.emplace_back( run_thread_local, prom );
    }
    ret = true;
    for ( auto& fut : futures ) {
      ret &= fut.get();
    }
    for ( auto& t : threads ) t.join();
    if ( !ret ) {
      std::cout << "FAILURE: Sequences not equal in normal thread-local gRandom test" << std::endl;
    } else {
      std::cout << "SUCCESS: Sequences are equal in normal thread-local gRandom test" << std::endl;
    }
    delete refvalues;
    futures.clear();
    threads.clear();
  }

  {
    CLHEP::MixMaxRng engine;
    engine.setSeed( 42 );
    ThreadLocalgRandom::Guard guard( engine );
    refhistvalues = new std::vector<double>{};
    for ( int i = 0; i < NNumbers; i++ ) {
      refhistvalues->push_back( hist->GetRandom() );
    }
    for ( int i = 0; i < NTRHEADS; i++ ) {
      auto prom = new std::promise<bool>{};
      futures.push_back( prom->get_future() );
      threads.emplace_back( hist_thread_local, prom );
    }
    ret = true;
    for ( auto& fut : futures ) {
      ret &= fut.get();
    }
    for ( auto& t : threads ) t.join();
    if ( !ret ) {
      std::cout << "FAILURE: Sequences not equal in normal thread-local gRandom histogram test" << std::endl;
    } else {
      std::cout << "SUCCESS: Sequences are equal in normal thread-local gRandom histogram test" << std::endl;
    }
    delete refhistvalues;
    futures.clear();
    threads.clear();
  }

  return 0;
}
