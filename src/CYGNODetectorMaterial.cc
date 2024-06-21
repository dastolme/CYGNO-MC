#include "CYGNODetectorMaterial.hh"
#include "CYGNODetectorMaterialMessenger.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

using namespace std;

CYGNODetectorMaterial* CYGNODetectorMaterial::fCYGNODetectorMaterial = NULL;

CYGNODetectorMaterial::CYGNODetectorMaterial():
	gasHeFrac(0.6), 
	gasCF4Frac(0.4)
{
    fMessenger = new CYGNODetectorMaterialMessenger(this);
    ConstructMaterials();
}


void CYGNODetectorMaterial::ConstructMaterials(){

    //**********************************************************************
    //   DEFINITION OF MATERIALS
    //**********************************************************************

    man = G4NistManager::Instance();
    G4double a;  // atomic mass
    G4double z;  // atomic number
    G4String name, symbol;
    G4int ncomponents, natoms;
    G4double fractionmass;
    G4double massOfMole = 1.008*g/mole;
    G4double temperature = 2.73*kelvin;
    G4double pressure = 3.e-18*pascal;
    G4double density;

    Air = new G4Material("AirGas", z= 7., a= 14.01*g/mole, density= 1.2*kg/m3);
    
    G4Element* elC  = man->FindOrBuildElement("C");
    G4Element* elH = man->FindOrBuildElement("H");
    G4Element* elHe = man->FindOrBuildElement("He");
    G4Element* elO = man->FindOrBuildElement("O");
    G4Element* elK = man->FindOrBuildElement("K");
    G4Element* elS = man->FindOrBuildElement("S"); 
    G4Element* elF = man->FindOrBuildElement("F"); 
    G4Element* elI = man->FindOrBuildElement("I");
    G4Element* elNa = man->FindOrBuildElement("Na");
    G4Element* elN = man->FindOrBuildElement("N");
    G4Element* elAl = man->FindOrBuildElement("Al");
    G4Element* elMn = man->FindOrBuildElement("Mn");
    G4Element* elSi = man->FindOrBuildElement("Si");
    G4Element* elCa = man->FindOrBuildElement("Ca");
    G4Element* elB = man->FindOrBuildElement("B");
    G4Element* elCu = man->FindOrBuildElement("Cu");
    
    O = man->FindOrBuildMaterial("G4_O");
    Na = man->FindOrBuildMaterial("G4_Na");
    K = man->FindOrBuildMaterial("G4_K");
    B = man->FindOrBuildMaterial("G4_B");
    Al = man->FindOrBuildMaterial("G4_Al");
    Cu = man->FindOrBuildMaterial("G4_Cu");
    Fe = man->FindOrBuildMaterial("G4_Fe");
    Pb = man->FindOrBuildMaterial("G4_Pb");
    Co = man->FindOrBuildMaterial("G4_Co");
    Ni = man->FindOrBuildMaterial("G4_Ni");
    Si = man->FindOrBuildMaterial("G4_Si");
    In = man->FindOrBuildMaterial("G4_In");
    
    Teflon  = man->FindOrBuildMaterial("G4_TEFLON");
    

    //PyrexGlass  = man->FindOrBuildMaterial("G4_Pyrex_Glass");
    //Modified Pyrex glass for Vessel PMTs
    //fraction masses for Pyrex Glass taken from Geant manual 
    //http://geant4.web.cern.ch/geant4/workAreaUserDocKA/Backup/Docbook_UsersGuides_beta/ForApplicationDeveloper/html/apas08.html
    PyrexGlass = new G4Material("PyrexGlass", density = 0.3389*g/cm3 , ncomponents = 6);
    PyrexGlass->AddMaterial (B, fractionmass = 0.0400639);
    PyrexGlass->AddMaterial (O, fractionmass = 0.539561);
    PyrexGlass->AddMaterial (Na, fractionmass = 0.0281909);
    PyrexGlass->AddMaterial (Al, fractionmass = 0.011644);
    PyrexGlass->AddMaterial (Si, fractionmass = 0.377219);
    PyrexGlass->AddMaterial (K, fractionmass = 0.00332099);


    //Hamamatsu R5912 uses Borosilicate glass.
    //Cribbed properties from:
    //http://www.schott.com/d/tubing/9a0f5126-6e35-43bd-bf2a-349912caf9f2/schott-algae-brochure-borosilicate.pdf 
    BSglass = new G4Material("BorosilicateGlass",
        density=2.23*g/cm3, ncomponents=6);
    BSglass->AddElement(elSi, natoms = 81);
    BSglass->AddElement(elO, natoms = 81*2+13*3+2*3+4+4);
    BSglass->AddElement(elB, natoms = 13*2);
    BSglass->AddElement(elAl, natoms = 2*2);
    BSglass->AddElement(elNa, natoms = 4*2);
    BSglass->AddElement(elK, natoms = 4*2);


    Quartz = new G4Material ("Quartz", 2.200 * g/cm3, ncomponents = 2);
    Quartz->AddElement (elSi, natoms = 1);
    Quartz->AddElement (elO, natoms = 2);


    //Cameras --> effective material, at the moment pyrex glass
    //FIXME
    G4cout << "========== Warning ============" << G4endl;
    G4cout << "Cameras and PMTs made of quartz, effective density in order to have the correct total mass" << G4endl;
    G4cout << "===============================" << G4endl;

    //
    //Camera = FindOrBuildMaterial("G4_Pyrex_Glass");
    //Camera = FindOrBuildMaterial("G4_GLASS_PLATE");
    Camera = new G4Material ("Camera", 3.498 * g/cm3, ncomponents = 2);
    Camera->AddElement (elSi, natoms = 1);
    Camera->AddElement (elO, natoms = 2);

    PMT = new G4Material ("PMT", 1.170 * g/cm3, ncomponents = 2);
    PMT->AddElement (elSi, natoms = 1);
    PMT->AddElement (elO, natoms = 2);
    
    //lngs rock material definition
    density = 2.71*g/cm3;
    lngsRock = new G4Material(name="LNGSRock",density,ncomponents=7);
    lngsRock->AddElement(elC,fractionmass=11.88*perCent);
    lngsRock->AddElement(elO,fractionmass=48.92*perCent);
    lngsRock->AddElement(elMn,fractionmass=5.58*perCent);
    lngsRock->AddElement(elAl,fractionmass=1.03*perCent);
    lngsRock->AddElement(elSi,fractionmass=1.27*perCent);
    lngsRock->AddElement(elK,fractionmass=1.03*perCent);
    lngsRock->AddElement(elCa,fractionmass=30.29*perCent);
   

    // Perspex (Acrylic)
    density = 1.180*g/cm3; //1180kg/m^3
    Perspex = new G4Material(name="Perspex", density, ncomponents=3);
    Perspex->AddElement(elH, natoms=8);
    Perspex->AddElement(elC, natoms=5);
    Perspex->AddElement(elO, natoms=2);


    // SF6_gas 
    density = 156.14*g/m3; //sf6 20 torr
    pressure = 0.0263158*atmosphere; //sf6 20 torr
    SF6_gas = new G4Material(name="SF6_gas", density, ncomponents=2, kStateGas, temperature, pressure);
    SF6_gas->AddElement(elS, natoms =1);
    SF6_gas->AddElement(elF, natoms =6);

    G4double He_frac = gasHeFrac;
    G4double CF4_frac = gasCF4Frac;

    // LNGS pressure correction
    G4double LNGS_pressure = 0.888231 * atmosphere;

    //He_gas
    density = 162.488 * He_frac * g/m3;
    pressure = LNGS_pressure * He_frac;
    He_gas = new G4Material(name="He_gas", density, ncomponents=1, kStateGas, temperature, pressure);
    He_gas->AddElement(elHe, natoms=1);

    //CF4_gas
    density = 3574.736 * CF4_frac * g/m3;
    pressure = LNGS_pressure * CF4_frac;
    CF4_gas = new G4Material(name="CF4_gas", density, ncomponents=2, kStateGas, temperature, pressure);
    CF4_gas->AddElement(elC, natoms=1);
    CF4_gas->AddElement(elF, natoms=4);

    //CYGNO_gas
    density = He_gas->GetDensity() + CF4_gas->GetDensity();
    pressure = He_gas->GetPressure() + CF4_gas->GetPressure();
    CYGNO_gas = new G4Material(name="CYGNO_gas", density, ncomponents=2, kStateGas, temperature, pressure);
    CYGNO_gas->AddMaterial(He_gas, fractionmass = He_gas->GetDensity()/density*100*perCent);
    CYGNO_gas->AddMaterial(CF4_gas, fractionmass = CF4_gas->GetDensity()/density*100*perCent);

    G4cout << "\n" << "GAS INFO:" << G4endl;
    G4cout << "CF4" << "\t" <<
      "Pressure: " << CYGNO_gas->GetMaterial("CF4_gas")->GetPressure()/(atmosphere) << " atm " << "\t" <<
      "Density: "  << CYGNO_gas->GetMaterial("CF4_gas")->GetDensity()/(g/m3) << " g/m3" <<  G4endl;
    G4cout << "He " << "\t" <<  
      "Pressure: " << CYGNO_gas->GetMaterial("He_gas")->GetPressure()/(atmosphere) << " atm " << "\t" <<
      "Density: "  << CYGNO_gas->GetMaterial("He_gas")->GetDensity()/(g/m3) << " g/m3" <<  G4endl; 
    G4cout << "Mix" << "\t" << 
      "Pressure: " << CYGNO_gas->GetPressure()/(atmosphere) << " atm" << "\t" << "\t"
      "Density: "  << CYGNO_gas->GetDensity()/(g/m3) << " g/m3" << "\n" << G4endl;


    //kapton

    density = 1.42*g/cm3;
    Kapton = new G4Material("Kapton", density, ncomponents=4);
    Kapton->AddElement(elC, natoms=22);
    Kapton->AddElement(elH, natoms=10);
    Kapton->AddElement(elN, natoms=2);
    Kapton->AddElement(elO, natoms=5);
    //---------
   
    // GEM effective material of 5 um copper + 50 um kapton + 5 um copper
    density = 2.14*g/cm3; //consider the composite material 1/6 copper + 5/6 kapton (volume), and subtract the holes with 70um diameter and 140 um pitch
    double fracMass;
    GEM = new G4Material("GEM", density, ncomponents=2);
    GEM->AddMaterial(Kapton, fracMass=0.44);
    GEM->AddMaterial(Cu, fracMass=0.56);


    
    Vacuum = new G4Material("Vacuum",1.,massOfMole, density= 1.e-25*g/cm3,kStateGas,temperature, pressure);
    Water  = man->FindOrBuildMaterial("G4_WATER");
    Steel  = man->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    PE  = man->FindOrBuildMaterial("G4_POLYETHYLENE");
    Concrete  = man->FindOrBuildMaterial("G4_CONCRETE");
	
    
    //**********************************************************************
    //   DEFINITION OF VISUALIZATION ATTRIBUTES
    //**********************************************************************
    PEVis = new G4VisAttributes(G4Color(0.8,0.83,0.8)); //Polyethilene
    PbVis = new G4VisAttributes(G4Color(0.,0.83,0.8)); //Pb
    WaterVis = new G4VisAttributes(G4Color(0.08,0.,1)); //Water
    AirVis = new G4VisAttributes(G4Color(1.,1.,0.4)); //Air
    VacuumVis = new G4VisAttributes(G4Color(1.,1.,0.4));
    CopperVis = new G4VisAttributes(G4Colour(1.,0.,0.));
    CameraVis = new G4VisAttributes(G4Colour(1.,0.,1.));
    PerspexVis = new G4VisAttributes(G4Colour(0.,1.,1.));
    CYGNOGasVis = new G4VisAttributes(G4Colour(0.,1.,0.));
    //CopperVis->SetForceWireframe(true);
}

CYGNODetectorMaterial::~CYGNODetectorMaterial()
{
    delete fMessenger;
}


CYGNODetectorMaterial* CYGNODetectorMaterial::GetInstance()
{
  if (fCYGNODetectorMaterial == NULL) {
    fCYGNODetectorMaterial = new CYGNODetectorMaterial();
  }
  return fCYGNODetectorMaterial;
}

void CYGNODetectorMaterial::Refresh(){
    ConstructMaterials();
}

G4Material* CYGNODetectorMaterial::Material(G4String what)
{ 
  G4Material* material = 0;
  if(what == "Vacuum")            material = Vacuum;
  if(what == "Air")               material = Air;
  if(what == "CYGNO_gas")         material = CYGNO_gas;
  if(what == "O")                 material = O;
  if(what == "Na")                material = Na;
  if(what == "K")                 material = K;
  if(what == "B")                 material = B;
  if(what == "Al")                material = Al;
  if(what == "Cu")                material = Cu;
  if(what == "Fe")                material = Fe;
  if(what == "Pb")                material = Pb;
  if(what == "Co")                material = Co;
  if(what == "Ni")                material = Ni;
  if(what == "Si")                material = Si;
  if(what == "In")                material = In;
  if(what == "Teflon")            material = Teflon;
  if(what == "PyrexGlass")        material = PyrexGlass;
  if(what == "BSglass")           material = BSglass;
  if(what == "VetoPMTglass")      material = VetoPMTglass;
  if(what == "Quartz")            material = Quartz;
  if(what == "Ceramic")           material = Ceramic;
  if(what == "Kovar")             material = Kovar;
  if(what == "LNGSRock")          material = lngsRock;
  if(what == "Water")             material = Water;
  if(what == "Steel")             material = Steel;
  if(what == "PE")                material = PE;
  if(what == "Concrete")          material = Concrete;
  if(what == "CYGNO_gas")         material = CYGNO_gas;
  if(what == "Perspex")           material = Perspex;
  if(what == "Camera")            material = Camera;
  if(what == "PMT")               material = PMT;
  if(what == "Kapton")            material = Kapton;
  if(what == "GEM")               material = GEM;
 
  return material;
}

G4VisAttributes* CYGNODetectorMaterial::VisAttributes(G4String what)
{
  G4VisAttributes* vis = 0;
  if(what == "PE")            vis = PEVis;
  if(what == "Pb")            vis = PbVis;
  if(what == "Water")         vis = WaterVis;
  if(what == "Air")           vis = AirVis;
  if(what == "Vacuum")        vis = VacuumVis;
  if(what == "Cu")            vis = CopperVis;
  if(what == "Perspex")       vis = PerspexVis;
  if(what == "LNGSRock")      vis = LNGSRockVis;
  if(what == "Concrete")      vis = ConcreteVis;
  if(what == "Camera")        vis = CameraVis;
  if(what == "PMT")        vis = CameraVis;
  if(what == "CYGNO_gas")     vis = CYGNOGasVis;
  return vis;
}
				  
G4Material* CYGNODetectorMaterial::FindOrBuildMaterial(const G4String& name, G4bool isotopes, G4bool warning)
{
  return man->FindOrBuildMaterial(name, isotopes, warning);  
}

