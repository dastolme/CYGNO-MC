#ifndef CYGNOHit_h
#define CYGNOHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Types.hh"
#include "G4ThreeVector.hh"

// Forward declaration of the allocator class
template <class T>
class G4Allocator;

class CYGNOHit : public G4VHit
{
  public:

      CYGNOHit();
     ~CYGNOHit();
      CYGNOHit(const CYGNOHit&);
      const CYGNOHit& operator=(const CYGNOHit&);
      G4int operator==(const CYGNOHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
      void SetParentID    (G4int pid)      { parentID = pid; };
      void SetParticleID  (G4int partID)      { particleID = partID; };
      void SetTrackID     (G4int track)      { trackID = track; };
      void SetGlobalTime  (G4double gtime)      { globalTime = gtime; };
      void SetKineticEne  (G4double kene)      { kinEne = kene; };
      void SetProcessIni  (G4String prini)      { processIni = prini; };
      void SetProcessFin  (G4String prfin)      { processFin = prfin; };
      void SetEdep        (G4double de)      { edep = de; };
      void SetPos         (G4ThreeVector xyz){ pos = xyz; };
      void SetLength      (G4double len){ trackLen = len; };
      void SetMom         (G4ThreeVector pxpypz){ momentum = pxpypz; };
 
      G4int GetParentID() { return parentID; };
      G4int GetParticleID() { return particleID; };
      G4double GetGlobalTime() { return globalTime; };
      G4double GetKineticEne() { return kinEne; };
      G4String GetProcessIni() { return processIni; };
      G4String GetProcessFin() { return processFin; };
      G4int GetTrackID()    { return trackID; };
      G4double GetEdep()    { return edep; };      
      G4ThreeVector GetPos(){ return pos; };
      G4double GetLength(){ return trackLen; };
      G4ThreeVector GetMom(){ return momentum; };
       
  private:
 
      G4int         parentID;
      G4int         particleID;
      G4int         trackID;
      G4double      globalTime;
      G4double      kinEne;
      G4String      processIni;
      G4String      processFin;
      G4double      edep;
      G4ThreeVector pos;
      G4double      trackLen;
      G4ThreeVector momentum;
 };

typedef G4THitsCollection<CYGNOHit> CYGNOHitsCollection;

extern G4ThreadLocal G4Allocator<CYGNOHit>* CYGNOHitAllocator;

inline void* CYGNOHit::operator new(size_t)
{
  if (!CYGNOHitAllocator) CYGNOHitAllocator = new G4Allocator<CYGNOHit>;
  void* aHit = (void*)CYGNOHitAllocator->MallocSingle();
  return aHit;
}

inline void CYGNOHit::operator delete(void *aHit)
{
  CYGNOHitAllocator->FreeSingle((CYGNOHit*) aHit);
}

#endif
