// class UserLimits  implementation
// (based on CMS code by P.Arce)
// 
#include "GiGaMTCore/GaussinoG4UserLimits.h"
#include <map>

namespace Gaussino {

UserLimits::UserLimits(const G4String& type) 
:fType(type)
{}

bool UserLimits::GetUserLimitSet( const G4int particleCode, ULtype ultype ) const
{
  bool isSet = false;
  ::std::pair< mmiul::const_iterator, mmiul::const_iterator> mmpite = fUserLimitSet.equal_range( particleCode );
  mmiul::const_iterator mmite;
  for(mmite = mmpite.first; mmite != mmpite.second; mmite++ ) {
    if( (*mmite).second == ultype ) {
      isSet = true;
      break;
    }
  }

  return isSet;
}


void UserLimits::SetUserMinEkine(G4double uekinMin, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMinEkine ) );
  fMinEkine[particleCode] = uekinMin;
}

void UserLimits::SetUserMaxTime(G4double utimeMax, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMaxTime ) );
  fMaxTime[particleCode] =utimeMax;
}

void UserLimits::SetMaxAllowedStep(G4double ustepMax, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMaxStepLength ) );
  fMaxStepLength[particleCode] =ustepMax;
}

void UserLimits::SetUserMaxTrackLength(G4double utrakMax, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMaxTrackLength ) );
  fMaxTrackLength[particleCode] = utrakMax;
}

void UserLimits::SetUserMinRange(G4double urangMin, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMinRange ) );
  fMinRange[particleCode] = urangMin;
}

G4double UserLimits::GetUserMinEkine(const G4Track& aTrack)
{
  return GetCut( aTrack, fMinEkine, true );
}

G4double UserLimits::GetUserMaxTime(const G4Track& aTrack)
{
  return GetCut( aTrack, fMaxTime, false );
}

G4double UserLimits::GetMaxAllowedStep(const G4Track& aTrack)
{
  return GetCut( aTrack, fMaxStepLength, false );
}

G4double UserLimits::GetUserMaxTrackLength(const G4Track& aTrack)
{
  return GetCut( aTrack, fMaxTrackLength, false );
}

G4double UserLimits::GetUserMinRange(const G4Track& aTrack)
{
  return GetCut( aTrack, fMinRange, true );
}

G4double UserLimits::GetCut(const G4Track& aTrack, const mid& partCutMap, const bool cutBelow )
{
  double cutval;
  G4int partCode = aTrack.GetDefinition()->GetPDGEncoding();
  //---- Look if cut exists for this particle
  mid::const_iterator ite = partCutMap.find( partCode );
  if( ite != partCutMap.end() ) 
    {
      cutval = (*ite).second;
    } 
  else 
    {
      //---- Look if cut exists for all particles (pid=0) 
      ite = partCutMap.find(0);    
      
      if( ite != partCutMap.end() ) {
        cutval = (*ite).second;
      } 
      else 
        {
          //if cut does not exists, return maximum value
          if( cutBelow ) {
            cutval = 0.;
          } 
          else 
            { 
              cutval = kInfinity;
            }
        }
    }
  return cutval;
}

/** comparison operator for UserLimits class 
 *  @param ul1 the first UL to be compared 
 *  @param ul2 the second UL to be compared 
 *  @return the result of coomaprison operation
 */
bool operator== ( const UserLimits& ul1 , 
                  const UserLimits& ul2 )
{       
  // the same object 
  if ( &ul1                == &ul2                ) { return true  ; } 
  if (  ul1.fMaxStepLength !=  ul2.fMaxStepLength ) { return false ; }
  if (  ul1.fMaxTime       !=  ul2.fMaxTime       ) { return false ; }
  if (  ul1.fMinEkine      !=  ul2.fMinEkine      ) { return false ; }
  if (  ul1.fMinRange      !=  ul2.fMinRange      ) { return false ; }
  if (  ul1.fUserLimitSet  !=  ul2.fUserLimitSet  ) { return false ; }
  if (  ul1.fType          !=  ul2.fType          ) { return false ; }
  return true ;
}
}
