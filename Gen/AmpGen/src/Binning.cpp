#include "AmpGen/Binning.h"

using namespace AmpGen;

bool AmpGen::operator<( const double& number, const AmpGen::Bin& bin )  { return number < bin.min(0) ; }
bool AmpGen::operator>( const double& number, const AmpGen::Bin& bin )  { return bin.min(0) > number ; } 
bool AmpGen::operator>( const AmpGen::Bin& bin , const double& number)  { return bin.min(0) > number ; }
bool AmpGen::operator<( const AmpGen::Bin& bin , const double& number)  { return bin.min(0) < number ; }

double deltaN( const Bin& bin0, const Bin& bin1 )  { return abs( bin0.nEvents() - bin1.nEvents() ); }

