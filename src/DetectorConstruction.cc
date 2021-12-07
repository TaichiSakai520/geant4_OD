
// ===========================================================================
//                         DetectorConstruction.cc
// ===========================================================================
#include "DetectorConstruction.hh"
//#include "OBDCalorimeterSD.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4Sphere.hh"
#include "G4NistManager.hh"

#include "G4Colour.hh"
#include "G4SDManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"

//#define OpticalMode
// ===========================================================================
//                            class description
// ===========================================================================

/////////////////////////////////////////////
DetectorConstruction::DetectorConstruction()
/////////////////////////////////////////////
{
  fin = new std::ifstream("PMTPosition.dat");
}

/////////////////////////////////////////////
DetectorConstruction::~DetectorConstruction()
/////////////////////////////////////////////
{
}
/////////////////////////////////////////////
void DetectorConstruction::DefineMaterials()
/////////////////////////////////////////////
{
  G4String symbol;
  G4double a, z, density; // a = means mass of a moll, z = means # of protons

  G4int ncomponents, natoms;
  G4double fractionmass;
  G4double /*density_0,*/ density_1, density_2;

  // define Elements
  G4Element *H = new G4Element("Hydrogen", symbol = "H", z = 1., a = 1.01 * g / mole);

  G4Element *C = new G4Element("Carbon", symbol = "C", z = 6., a = 12.01 * g / mole);

  G4Element *N = new G4Element("Nitrogen", symbol = "N", z = 7., a = 14.01 * g / mole);
  G4Element *O = new G4Element("Oxygen", symbol = "O", z = 8., a = 16.00 * g / mole);
  G4Element *Fe = new G4Element("Iron", symbol = "Fe", z = 26., a = 55.85 * g / mole);

  //Get pointer to 'Material Manager'
  G4NistManager *materi_Man = G4NistManager::Instance();
  //Get stainless_steel
  //G4Material *stainless_steel = materi_Man->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  G4Material *stainless_steel = materi_Man->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  G4Material *water_n = materi_Man->FindOrBuildMaterial("G4_WATER");
  G4Material *glass_n = materi_Man->FindOrBuildMaterial("G4_GLASS_PLATE");
  G4Material *Cu_NIST = materi_Man->FindOrBuildMaterial("G4_Cu");
  G4Material *air = materi_Man->FindOrBuildMaterial("G4_AIR");
  //std::cout << stainless_steel << std::endl;
  // for enrichment
  G4Material *stainless = new G4Material("stainless", density = 8.0 * g / cm3, ncomponents = 1);
  stainless->AddElement(Fe, natoms = 1);

  // define a material from elements.   case 1: chemical molecule

  // Pseudo-cumene -- (C9 H12) also called 1,2,4-Trimethybenzene

  /*
  G4Material *stainless = new G4Material("stainless", 7.938 * g / cm3, ncomponents = 2, kStateSolid, 276 * kelvin, 201 * atm);
  stainless->AddMaterial(stainless_steel, fractionmass = 0.99);
  stainless->AddMaterial(C_M, fractionmass = 0.01);
*/
  G4Material *Copper = new G4Material("Copper", 8.96 * g / cm3, ncomponents = 1, kStateSolid);
  Copper->AddMaterial(Cu_NIST, fractionmass = 1);

  G4Material *glass = new G4Material("glass", 2.4 * g / cm3, ncomponents = 1, kStateSolid);
  glass->AddMaterial(glass_n, 1);
  G4Material *N2 = new G4Material("N2", 0.001251 * g / cm3, ncomponents = 1, kStateSolid);
  N2->AddElement(N, 1);
  G4Material *water = new G4Material("water", 1 * g / cm3, ncomponents = 1, kStateLiquid);
  water->AddMaterial(water_n, 1);
  double LAB_density = 0.86 * g / cm3;
  G4Material *LAB = new G4Material("LAB", density = LAB_density, ncomponents = 2);
  LAB->AddElement(C, natoms = 18);
  LAB->AddElement(H, natoms = 30);
  G4Material *PPO = new G4Material("PPO", density = 1.00 * g / cm3, ncomponents = 4);
  PPO->AddElement(C, natoms = 15);
  PPO->AddElement(H, natoms = 11);
  PPO->AddElement(N, natoms = 1);
  PPO->AddElement(O, natoms = 1);

  G4double PPO_fraction = 3.0 * g / (1e3 * cm3 * LAB_density);
  density_1 = (0.86 * 1e3 + 2.0) / 1000 * g / cm3;
  G4Material *LS = new G4Material("LS", density_1, ncomponents = 2, kStateLiquid);
  LS->AddMaterial(LAB, fractionmass = 1 / (1 + PPO_fraction));
  LS->AddMaterial(PPO, fractionmass = PPO_fraction / (1 + PPO_fraction));

  G4Material *paraffin = new G4Material("paraffin", 0.76 * g / cm3, ncomponents = 2, kStateLiquid);
  paraffin->AddElement(C, natoms = 25);
  paraffin->AddElement(H, natoms = 52);

  //acrylic resin
  G4Material *acrylic = new G4Material("acrylic", 1.18 * g / cm3, ncomponents = 3, kStateSolid);
  acrylic->AddElement(C, natoms = 5);
  acrylic->AddElement(O, natoms = 2);
  acrylic->AddElement(H, natoms = 8);
}

////////////////////////////////////////////////////
G4VPhysicalVolume *DetectorConstruction::Construct()
////////////////////////////////////////////////////
{
  // define materials
  DefineMaterials();

  G4Material *mate;
  G4VisAttributes *va;
  G4Material *materi_stainless;
  materi_stainless = G4Material::GetMaterial("stainless");

  G4Material *materi_glass;
  materi_glass = G4Material::GetMaterial("glass");

  // ==============================================================
  //                          world volume
  // ==============================================================

  double radius_world = 80 * m;
  double height_world = 160 * m;
  G4Tubs *solid_world = new G4Tubs("solid_world", 0., radius_world, height_world / 2.0, 0., 360. * deg);
  G4Material *materi_world;
  materi_world = G4Material::GetMaterial("water");
  G4LogicalVolume *logVol_world = new G4LogicalVolume(solid_world, materi_world, "logVol_world");
  G4PVPlacement *world = new G4PVPlacement(0, G4ThreeVector(), "physVol_world", logVol_world, 0, false, 0);
  logVol_world->SetVisAttributes(new G4VisAttributes(false, G4Colour(0, 0, 1)));

  // ==============================================================
  //                             hemisphere
  // ==============================================================
  /*
  double radius_hemi = 40 * m;
  G4Sphere *solid_sphere = new G4Sphere("solid_sphere", radius_hemi - 0.3 * m, radius_hemi, 0, 360 * deg, 0, 90 * deg);
  G4LogicalVolume *logVol_sphere = new G4LogicalVolume(solid_sphere, materi_world, "logVol_sphere");
  G4PVPlacement *sphere = new G4PVPlacement(0, G4ThreeVector(0 * m, 0 * m, (-height_world / 2 + 20.032 / 2)), "physVol_sphere", logVol_sphere, world, false, 0);
  logVol_sphere->SetVisAttributes(new G4VisAttributes(true, G4Colour(1, 0, 1)));
*/
  // ==============================================================
  //                             Detector
  // ==============================================================

  G4double radius_De = 9.016 * m;
  G4double height_De = 20.032 * m;
  G4Tubs *Solid_De = new G4Tubs("Solid_De", 0., radius_De, height_De / 2.0, 0., 360. * deg);

  // Define logical volume
  G4Material *materi_De;
  materi_De = G4Material::GetMaterial("stainless");
  G4LogicalVolume *logVol_De = new G4LogicalVolume(Solid_De, materi_De, "logVol_De");

  G4PVPlacement *De_p = new G4PVPlacement(0, G4ThreeVector(0 * m, 0 * m, (-height_world / 2 + height_De / 2)), "PhysVol_De", logVol_De, world, false, 1000);
  logVol_De->SetVisAttributes(new G4VisAttributes(true, G4Colour(0, 1, 0)));

#ifndef OpticalMode
  G4double radius_para = 9 * m;
  G4double leng_Z_para = 20 * m;
  G4Tubs *Solid_para = new G4Tubs("Solid_para", 0.0, radius_para, leng_Z_para / 2.0, 0.0, 360.0 * deg);
  G4Material *materi_para = G4Material::GetMaterial("paraffin");
  G4LogicalVolume *logVol_para = new G4LogicalVolume(Solid_para, materi_para, "logVol_para");
  G4int copyNum_LogV_para = 3000;
  G4PVPlacement *para_p = new G4PVPlacement(0, G4ThreeVector(0 * m, 0 * m, 0 * m), "PhysVol_para", logVol_para, De_p, false, copyNum_LogV_para);
  //logVol_para->SetVisAttributes(G4VisAttributes::Invisible);

  //make acrylic vessel
  G4double radius_ac = 6.05 * m;
  G4double leng_Z_ac = 14.1 * m;
  G4Tubs *Solid_ac = new G4Tubs("Solid_ac", 0.0, radius_ac, leng_Z_ac / 2.0, 0.0, 360.0 * deg);
  G4Material *materi_ac = G4Material::GetMaterial("acrylic");
  G4LogicalVolume *logVol_ac = new G4LogicalVolume(Solid_ac, materi_ac, "logVol_ac");
  G4int copyNum_LogV_ac = 4000;
  G4PVPlacement *ac_p = new G4PVPlacement(0, G4ThreeVector(0 * m, 0 * m, 0 * m), "PhysVol_ac", logVol_ac, para_p, false, copyNum_LogV_ac);

  //fill the ditector with LS
  G4double radius_LS = 6. * m;
  G4double leng_Z_LS = 14. * m;
  G4Tubs *Solid_LS = new G4Tubs("Solid_LS", 0.0, radius_LS, leng_Z_LS / 2.0, 0.0, 360.0 * deg);
  G4Material *materi_LS = G4Material::GetMaterial("LS");
  G4LogicalVolume *logVol_LS = new G4LogicalVolume(Solid_LS, materi_LS, "logVol_LS");
  G4int copyNum_LogV_LS = 5000;
  G4PVPlacement *LS_p = new G4PVPlacement(0, G4ThreeVector(0 * m, 0 * m, 0 * m), "PhysVol_LS", logVol_LS, ac_p, false, copyNum_LogV_LS);
  logVol_LS->SetVisAttributes(new G4VisAttributes(true, G4Colour(1, 1, 0)));
#endif

  // ==============================================================
  //                              OD(side)
  // ==============================================================
  G4Sphere *PMTSolid = new G4Sphere("PMTSolid", 0 * cm, 33.02 * cm, 0 * deg, 360 * deg, 0 * deg, 180 * deg);

  G4LogicalVolume *PMTLV = new G4LogicalVolume(PMTSolid, materi_glass, "PMTLV");

  char Buffer[256];
  char name[256];
  G4double PMT_x, PMT_y, PMT_z;
  G4int PMTNum;
  G4VPhysicalVolume *PMT_p[400];
  while (fin->getline(Buffer, sizeof(Buffer)))
  {
    std::stringstream strin(Buffer);
    strin >> PMTNum >> PMT_x >> PMT_y >> PMT_z;
    sprintf(name, "PhysVol_PMT%d", PMTNum);
    PMT_p[PMTNum] = new G4PVPlacement(0, G4ThreeVector(PMT_x * mm, PMT_y * mm, PMT_z * mm + (-height_world / 2 + height_De / 2)), name, PMTLV, world, false, 2000 + PMTNum);
  }

  // ==============================================================
  //                          optical feature
  // ==============================================================
#ifdef OpticalMode
  G4double photonEnergy[] =
      {2.034 * eV, 2.068 * eV, 2.103 * eV, 2.139 * eV,
       2.177 * eV, 2.216 * eV, 2.256 * eV, 2.298 * eV,
       2.341 * eV, 2.386 * eV, 2.433 * eV, 2.481 * eV,
       2.532 * eV, 2.585 * eV, 2.640 * eV, 2.697 * eV,
       2.757 * eV, 2.820 * eV, 2.885 * eV, 2.954 * eV,
       3.026 * eV, 3.102 * eV, 3.181 * eV, 3.265 * eV,
       3.353 * eV, 3.446 * eV, 3.545 * eV, 3.649 * eV,
       3.760 * eV, 3.877 * eV, 4.002 * eV, 4.136 * eV};

  const G4int nEntries = sizeof(photonEnergy) / sizeof(G4double);

  G4double refractiveIndex1[] =
      {1.33288, 1.33322, 1.33358, 1.33395,
       1.33434, 1.33474, 1.33515, 1.33559,
       1.33603, 1.33649, 1.33697, 1.33746,
       1.33798, 1.33852, 1.33907, 1.33963,
       1.34021, 1.34081, 1.34141, 1.34203,
       1.34266, 1.34329, 1.34391, 1.34453,
       1.34512, 1.34568, 1.34619, 1.34663,
       1.34698, 1.34718, 1.34721, 1.347}; // at 4 degree celcius

  assert(sizeof(refractiveIndex1) == sizeof(photonEnergy));

  G4double absorption[] =
      {3.448 * m, 4.082 * m, 6.329 * m, 9.174 * m, 12.346 * m, 13.889 * m,
       15.152 * m, 17.241 * m, 18.868 * m, 20.000 * m, 26.316 * m, 35.714 * m,
       45.455 * m, 47.619 * m, 52.632 * m, 52.632 * m, 55.556 * m, 52.632 * m,
       52.632 * m, 47.619 * m, 45.455 * m, 41.667 * m, 37.037 * m, 33.333 * m,
       30.500 * m, 28.500 * m, 27.000 * m, 24.500 * m, 22.000 * m, 19.500 * m,
       17.500 * m, 14.500 * m}; //at normal temperature

  assert(sizeof(absorption) == sizeof(photonEnergy));

  G4double scintilFast[] =
      {1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
       1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
       1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
       1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
       1.00, 1.00, 1.00, 1.00};

  assert(sizeof(scintilFast) == sizeof(photonEnergy));

  G4double scintilSlow[] =
      {0.01, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00,
       7.00, 8.00, 9.00, 8.00, 7.00, 6.00, 4.00,
       3.00, 2.00, 1.00, 0.01, 1.00, 2.00, 3.00,
       4.00, 5.00, 6.00, 7.00, 8.00, 9.00, 8.00,
       7.00, 6.00, 5.00, 4.00};

  assert(sizeof(scintilSlow) == sizeof(photonEnergy));

  G4MaterialPropertiesTable *myMPT1 = new G4MaterialPropertiesTable();

  myMPT1->AddProperty("RINDEX", photonEnergy, refractiveIndex1, nEntries)
      ->SetSpline(true);
  myMPT1->AddProperty("ABSLENGTH", photonEnergy, absorption, nEntries)
      ->SetSpline(true);
  myMPT1->AddProperty("FASTCOMPONENT", photonEnergy, scintilFast, nEntries)
      ->SetSpline(true);
  myMPT1->AddProperty("SLOWCOMPONENT", photonEnergy, scintilSlow, nEntries)
      ->SetSpline(true);

  myMPT1->AddConstProperty("SCINTILLATIONYIELD", 0 / MeV);
  myMPT1->AddConstProperty("RESOLUTIONSCALE", 1.0);
  myMPT1->AddConstProperty("FASTTIMECONSTANT", 1. * ns);
  myMPT1->AddConstProperty("SLOWTIMECONSTANT", 10. * ns);
  myMPT1->AddConstProperty("YIELDRATIO", 0.8);

  // G4double energy_water[] = {
  //     1.56962 * eV, 1.58974 * eV, 1.61039 * eV, 1.63157 * eV,
  //     1.65333 * eV, 1.67567 * eV, 1.69863 * eV, 1.72222 * eV,
  //     1.74647 * eV, 1.77142 * eV, 1.7971 * eV, 1.82352 * eV,
  //     1.85074 * eV, 1.87878 * eV, 1.90769 * eV, 1.93749 * eV,
  //     1.96825 * eV, 1.99999 * eV, 2.03278 * eV, 2.06666 * eV,
  //     2.10169 * eV, 2.13793 * eV, 2.17543 * eV, 2.21428 * eV,
  //     2.25454 * eV, 2.29629 * eV, 2.33962 * eV, 2.38461 * eV,
  //     2.43137 * eV, 2.47999 * eV, 2.53061 * eV, 2.58333 * eV,
  //     2.63829 * eV, 2.69565 * eV, 2.75555 * eV, 2.81817 * eV,
  //     2.88371 * eV, 2.95237 * eV, 3.02438 * eV, 3.09999 * eV,
  //     3.17948 * eV, 3.26315 * eV, 3.35134 * eV, 3.44444 * eV,
  //     3.54285 * eV, 3.64705 * eV, 3.75757 * eV, 3.87499 * eV,
  //     3.99999 * eV, 4.13332 * eV, 4.27585 * eV, 4.42856 * eV,
  //     4.59258 * eV, 4.76922 * eV, 4.95999 * eV, 5.16665 * eV,
  //     5.39129 * eV, 5.63635 * eV, 5.90475 * eV, 6.19998 * eV};

  //const G4int numentries_water = sizeof(energy_water) / sizeof(G4double);

  //assume 100 times larger than the rayleigh scattering for now.
  // G4double mie_water[] = {
  //     1670.244 * 100 * m, 1587.267 * 100 * m, 1507.42 * 100 * m,
  //     1430.625 * 100 * m, 1356.802 * 100 * m, 1285.874 * 100 * m,
  //     1217.763 * 100 * m, 1152.395 * 100 * m, 1089.695 * 100 * m,
  //     1029.588 * 100 * m, 972.0035 * 100 * m, 916.8686 * 100 * m,
  //     864.1133 * 100 * m, 813.6679 * 100 * m, 765.4642 * 100 * m,
  //     719.4346 * 100 * m, 675.5129 * 100 * m, 633.6336 * 100 * m,
  //     593.7325 * 100 * m, 555.7461 * 100 * m, 519.6124 * 100 * m,
  //     485.2700 * 100 * m, 452.6587 * 100 * m, 421.7194 * 100 * m,
  //     392.3939 * 100 * m, 364.6250 * 100 * m, 338.3568 * 100 * m,
  //     313.5341 * 100 * m, 290.1030 * 100 * m, 268.0103 * 100 * m,
  //     247.2042 * 100 * m, 227.6336 * 100 * m, 209.2488 * 100 * m,
  //     192.0007 * 100 * m, 175.8416 * 100 * m, 160.7245 * 100 * m,
  //     146.6038 * 100 * m, 133.4346 * 100 * m, 121.1733 * 100 * m,
  //     109.7770 * 100 * m, 99.20416 * 100 * m, 89.41407 * 100 * m,
  //     80.36711 * 100 * m, 72.02470 * 100 * m, 64.34927 * 100 * m,
  //     57.30429 * 100 * m, 50.85425 * 100 * m, 44.96467 * 100 * m,
  //     39.60210 * 100 * m, 34.73413 * 100 * m, 30.32937 * 100 * m,
  //     26.35746 * 100 * m, 22.78907 * 100 * m, 19.59588 * 100 * m,
  //     16.75064 * 100 * m, 14.22710 * 100 * m, 12.00004 * 100 * m,
  //     10.04528 * 100 * m, 8.339666 * 100 * m, 6.861063 * 100 * m};

  // assert(sizeof(mie_water) == sizeof(energy_water));

  // gforward, gbackward, forward backward ratio
  //G4double mie_water_const[3] = {0.99, 0.99, 0.8};

  // myMPT1->AddProperty("MIEHG", energy_water, mie_water, numentries_water)
  //->SetSpline(true);
  // myMPT1->AddConstProperty("MIEHG_FORWARD", mie_water_const[0]);
  // myMPT1->AddConstProperty("MIEHG_BACKWARD", mie_water_const[1]);
  // myMPT1->AddConstProperty("MIEHG_FORWARD_RATIO", mie_water_const[2]);

  //G4cout << "Water G4MaterialPropertiesTable" << G4endl;
  //myMPT1->DumpTable();

  //set mpt to water
  materi_world->SetMaterialPropertiesTable(myMPT1);

  // Set the Birks Constant for the Water scintillator

  materi_world->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

  //for absorb material
  G4OpticalSurface *OpSurface1 = new G4OpticalSurface("OpSurface1");
  //OpSurface1->SetModel(glisur);
  OpSurface1->SetModel(unified);
  OpSurface1->SetType(dielectric_metal);
  OpSurface1->SetFinish(polished);

  G4double refractiveIndex2[] =
      {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
       0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
       0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
       0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
       0.00, 0.00, 0.00, 0.00};

  assert(sizeof(refractiveIndex2) == sizeof(photonEnergy));

  G4MaterialPropertiesTable *SMPT1 = new G4MaterialPropertiesTable();
  SMPT1->AddProperty("REFLECTIVITY", photonEnergy, refractiveIndex2, nEntries);

  // SMPT1->AddProperty("TRANSMITTANCE", photonEnergy, refractiveIndex2, nEntries);
  OpSurface1->SetMaterialPropertiesTable(SMPT1);

  //G4LogicalBorderSurface *Surface1 =
  //here surface definition
  //new G4LogicalBorderSurface("Surface1", PhysVol_LS, PhysVol_reflect, OpSurface1);
  for (int i = 0; i < PMTNum; i++)
  {
    new G4LogicalBorderSurface("Surface1", world, PMT_p[i], OpSurface1);
  }
  // ==============================================================
  //                      return the world volume
  // ==============================================================
#endif
  return world;
}
