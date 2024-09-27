/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

namespace doh {

  constexpr size_t MAX_ELECTRON_SHELLS = 8;

  struct electronShell_t {
    int8_t s = 0, p = 0, d = 0, f = 0;
    constexpr electronShell_t& operator+=(const electronShell_t& o);
    constexpr electronShell_t operator+(const electronShell_t& o);
  };

  struct electronConfig_t {
    const electronShell_t electronConfig[MAX_ELECTRON_SHELLS];
    const char notation[MAX_ELECTRON_SHELLS * 4 * 5];//5 chars per subshell
    constexpr electronConfig_t(const std::string&);
    constexpr electronConfig_t& operator+=(const electronConfig_t& o);
    constexpr electronConfig_t operator+(const electronConfig_t& o);
  };

  // struct compoundAttributes_t {
  //   // TODO triplet coefficients to predict properties from temperature and pressure
  //   // int16_t netCharge;
  //   // float latentEnergy_evaporation, latentEnergy_melting, latentEnergy_suplamation;
  //   glm::mat4 color;//input and output both (r, g, b, heat), total output should equal total input (a larger matrix might one day be used to include other wavelengths of radiation)
  //   //TODO add more as needed
  //   //?? float refractiveIndex;//relative to vacuum
  // };

  struct elementAttributes_t {
    uint_8 atomicNumber;
    std::string symbol, name;
    float atomicWeight;
    uint32_t cpkColor;
    electronConfig_t electronConfig;
    int16_t atomicRadius;
    float ionizationEnegy, electronAffinity;
    float meltingPoint, boilingPoint, density;//std pressure, to be used as an input in computing the properties of compounds
    glm::mat4 color;//simple version for particle clouds, use compound simulator for more involved use cases. input and output both (r, g, b, heat), total output should equal total input (a larger matrix might one day be used to include other wavelengths of radiation)
    float refractiveScatterRate;//visible light refractive index at STP
  };

  struct compoundConstituent_t {
    elementAttributes_t ea;
    double density;//kg/m^3
  };

  struct compound_t {
    //TODO common iterator type based on idx and size?
    constexpr compound_t() = default;
    virtual constexpr ~compound_t() = default;
    virtual const compoundConstituent_t& operator[](size_t i) const = 0;
    virtual compoundConstituent_t& operator[](size_t i) = 0;
    const compoundConstituent_t& at(size_t i) const { return this->operator[](i); };
    compoundConstituent_t& at(size_t i) { return this->operator[](i); };
    virtual const size_t size() const = 0;
  };

  typedef std::shared_ptr<compound_t> compound_p;

  template<size_t S> struct compound_final : public compound_t {
    WITE::copyableArray<compoundConstituent_t, S> data;
    constexpr compound_final() = default;
    template<typename T> constexpr compound_final(const T& t) : data(t) {};
    constexpr compound_final(const compound_final<S>& t) : data(t.data) {};
    virtual constexpr ~compound_final() = default;
    virtual const compoundConstituent_t& operator[](size_t i) const { return data[i]; };
    virtual compoundConstituent_t& operator[](size_t i) { return data[i]; };
    virtual const size_t size() const { return S; };
  };

  struct compound_temp : public compound_t {
    std::vector<compoundConstituent_t> data;
    compound_temp() = default;
    template<typename T> compound_temp(const T& t) : data(t) {};
    virtual ~compound_temp() = default;
    virtual const compoundConstituent_t& operator[](size_t i) const { return data[i]; };
    virtual compoundConstituent_t& operator[](size_t i) { return data[i]; };
    virtual const size_t size() const { return data.size(); };
    template<size_t S> compound_final<S> finalize() const { return { data }; };
  };

  constexpr glm::mat4 mkDiffusiveMat(float r, float g, float b) {
    return { r, 0, 0, 1-r, 0, g, 0, 1-g, 0, 0, b, 1-b, 0, 0, 0, 1 };
  };

  //weight is relative density, for dust clouds. Weight = 0 for identity, weight = 2 for higher than STP density that would more thuroughly diffuse
  constexpr glm::mat4 mkDiffusiveMat(glm::mat4 base, float weight) {
    glm::mat4 ret;
    for(size_t c = 0;c < 4;c++) {
      float scale = glm::pow(base[c][c], weight);
      for(size_t i = 0;i < 4;i++) {
	ret[c][i] = i == c ? scale : base[c][i] * (1 - scale) / (1 - base[c][c]);
      }
    }
    return ret;
  };

  constexpr glm::mat4 mkDiffusiveMat(compound_p cp) {
    if(cp->size() == 0) return {1};
    glm::mat4 onePass = {0};
    float divisor = 0;
    for(size_t i = 0;i < cp->size();i++) {
      const auto& cc = cp->at(i);
      float volumetricDensity = cc.ea.atomicRadius * cc.ea.atomicRadius * cc.density / cc.ea.density;
      //r^2 not r^3 because the second lerp below adds a dimension of meaning to cc.density
      onePass += mkDiffusiveMat(cc.ea.color, cc.density / cc.ea.density) * volumetricDensity;
      divisor += volumetricDensity;
    }
    onePass *= 1.0f/divisor;
    //onePass is now the average diffusive matrix for all the elements, but does not handle interaction between elements yet
    //approximate interaction between elements by lerping it towards the identity and then concatenating it with itself a few times
    onePass = mkDiffusiveMat(onePass, 1.0f/cp->size());
    glm::mat4 ret = onePass;
    for(size_t i = 1;i < cp->size();i++)
      ret *= onePass;
    //TODO integrate into an actual limit?
    return ret;
  };

  //TODO finish filling in colors and scatter rates for other elements
  constexpr elementAttributes_t elementsByNumber[] = {
    { 1,"H","Hydrogen",1.008,0xFFFFFF,"1s1",120,13.598,0.754,13.81,20.28,0.00008988, {1}, 1.000132 },
    { 2,"He","Helium",4.0026,0xD9FFFF,"1s2",140,24.587,-0.5,0.95,4.22,0.0001785, {1}, 1.000036 },
    { 3,"Li","Lithium",7,0xCC80FF,"[He]2s1",182,5.392,0.618,453.65,1615,0.534 },
    { 4,"Be","Beryllium",9.012183,0xC2FF00,"[He]2s2",153,9.323,-0.5,1560,2744,1.85 },
    { 5,"B","Boron",10.81,0xFFB5B5,"[He]2s2 2p1",192,8.298,0.277,2348,4273,2.37 },
    { 6,"C","Carbon",12.011,0x909090,"[He]2s2 2p2",170,11.26,1.263,3823,4098,2.267 },
    { 7,"N","Nitrogen",14.007,0x3050F8,"[He] 2s2 2p3",155,14.534,-0.07,63.15,77.36,0.0012506, {1}, 1.000298 },
    { 8,"O","Oxygen",15.999,0xFF0D0D,"[He]2s2 2p4",152,13.618,1.461,54.36,90.2,0.001429, {1}, 1.000271 },
    { 9,"F","Fluorine",18.99840316,0x09E051,"[He]2s2 2p5",135,17.423,3.339,53.53,85.03,0.001696 },
    { 10,"Ne","Neon",20.18,0xB3E3F5,"[He]2s2 2p6",154,21.565,-1.2,24.56,27.07,0.0008999 },
    { 11,"Na","Sodium",22.9897693,0xAB5CF2,"[Ne]3s1",227,5.139,0.548,370.95,1156,0.97 },
    { 12,"Mg","Magnesium",24.305,0x8AFF00,"[Ne]3s2",173,7.646,-0.4,923,1363,1.74 },
    { 13,"Al","Aluminum",26.981538,0xBFA6A6,"[Ne]3s2 3p1",184,5.986,0.441,933.437,2792,2.7 },
    { 14,"Si","Silicon",28.085,0xF0C8A0,"[Ne]3s2 3p2",210,8.152,1.385,1687,3538,2.3296 },
    { 15,"P","Phosphorus",30.973762,0xFF8000,"[Ne]3s2 3p3",180,10.487,0.746,317.3,553.65,1.82 },
    { 16,"S","Sulfur",32.07,0xFFFF30,"[Ne]3s2 3p4",180,10.36,2.077,388.36,717.75,2.067, mkDiffusiveMat(0.8, 0.72, 0.19), 1.000686 },
    { 17,"Cl","Chlorine",35.45,0x1FF01F,"[Ne]3s2 3p5",175,12.968,3.617,171.65,239.11,0.003214, {1}, 1.000773 },
    { 18,"Ar","Argon",39.9,0x80D1E3,"[Ne]3s2 3p6",188,15.76,-1,83.8,87.3,0.0017837, {1}, 1.000281 },
    { 19,"K","Potassium",39.0983,0x8F40D4,"[Ar]4s1",275,4.341,0.501,336.53,1032,0.89 },
    { 20,"Ca","Calcium",40.08,0x3DFF00,"[Ar]4s2",231,6.113,0.02455,1115,1757,1.54 },
    { 21,"Sc","Scandium",44.95591,0xE6E6E6,"[Ar]4s2 3d1",211,6.561,0.188,1814,3109,2.99 },
    { 22,"Ti","Titanium",47.867,0xBFC2C7,"[Ar]4s2 3d2",187,6.828,0.079,1941,3560,4.5 },
    { 23,"V","Vanadium",50.9415,0xA6A6AB,"[Ar]4s2 3d3",179,6.746,0.525,2183,3680,6 },
    { 24,"Cr","Chromium",51.996,0x8A99C7,"[Ar]3d5 4s1",189,6.767,0.666,2180,2944,7.15 },
    { 25,"Mn","Manganese",54.93804,0x9C7AC7,"[Ar]4s2 3d5",197,7.434,-0.5,1519,2334,7.3 },
    { 26,"Fe","Iron",55.84,0xE06633,"[Ar]4s2 3d6",194,7.902,0.163,1811,3134,7.874 },
    { 27,"Co","Cobalt",58.93319,0xF090A0,"[Ar]4s2 3d7",192,7.881,0.661,1768,3200,8.86 },
    { 28,"Ni","Nickel",58.693,0x50D050,"[Ar]4s2 3d8",163,7.64,1.156,1728,3186,8.912 },
    { 29,"Cu","Copper",63.55,0xC88033,"[Ar]4s1 3d10",140,7.726,1.228,1357.77,2835,8.933 },
    { 30,"Zn","Zinc",65.4,0x7D80B0,"[Ar]4s2 3d10",139,9.394,-0.6,692.68,1180,7.134 },
    { 31,"Ga","Gallium",69.723,0xC28F8F,"[Ar]4s2 3d10 4p1",187,5.999,0.3,302.91,2477,5.91 },
    { 32,"Ge","Germanium",72.63,0x668F8F,"[Ar]4s2 3d10 4p2",211,7.9,1.35,1211.4,3106,5.323 },
    { 33,"As","Arsenic",74.92159,0xBD80E3,"[Ar]4s2 3d10 4p3",185,9.815,0.81,1090,887,5.776 },
    { 34,"Se","Selenium",78.97,0xFFA100,"[Ar]4s2 3d10 4p4",190,9.752,2.021,493.65,958,4.809 },
    { 35,"Br","Bromine",79.9,0xA62929,"[Ar]4s2 3d10 4p5",183,11.814,3.365,265.95,331.95,3.11 },
    { 36,"Kr","Krypton",83.8,0x5CB8D1,"[Ar]4s2 3d10 4p6",202,14,-1,115.79,119.93,0.003733 },
    { 37,"Rb","Rubidium",85.468,0x702EB0,"[Kr]5s1",303,4.177,0.468,312.46,961,1.53 },
    { 38,"Sr","Strontium",87.62,0x00FF00,"[Kr]5s2",249,5.695,0.052,1050,1655,2.64 },
    { 39,"Y","Yttrium",88.90584,0x94FFFF,"[Kr]5s2 4d1",219,6.217,0.307,1795,3618,4.47 },
    { 40,"Zr","Zirconium",91.22,0x94E0E0,"[Kr]5s2 4d2",186,6.634,0.426,2128,4682,6.52 },
    { 41,"Nb","Niobium",92.90637,0x73C2C9,"[Kr]5s1 4d4",207,6.759,0.893,2750,5017,8.57 },
    { 42,"Mo","Molybdenum",95.95,0x54B5B5,"[Kr]5s1 4d5",209,7.092,0.746,2896,4912,10.2 },
    { 43,"Tc","Technetium",96.90636,0x3B9E9E,"[Kr]5s2 4d5",209,7.28,0.55,2430,4538,11 },
    { 44,"Ru","Ruthenium",101.1,0x248F8F,"[Kr]5s1 4d7",207,7.361,1.05,2607,4423,12.1 },
    { 45,"Rh","Rhodium",102.9055,0x0A7D8C,"[Kr]5s1 4d8",195,7.459,1.137,2237,3968,12.4 },
    { 46,"Pd","Palladium",106.42,0x6985,"[Kr]4d10",202,8.337,0.557,1828.05,3236,12 },
    { 47,"Ag","Silver",107.868,0xC0C0C0,"[Kr]5s1 4d10",172,7.576,1.302,1234.93,2435,10.501 },
    { 48,"Cd","Cadmium",112.41,0xFFD98F,"[Kr]5s2 4d10",158,8.994,-0.7,594.22,1040,8.69 },
    { 49,"In","Indium",114.818,0xA67573,"[Kr]5s2 4d10 5p1",193,5.786,0.3,429.75,2345,7.31 },
    { 50,"Sn","Tin",118.71,0x668080,"[Kr]5s2 4d10 5p2",217,7.344,1.2,505.08,2875,7.287 },
    { 51,"Sb","Antimony",121.76,0x9E63B5,"[Kr]5s2 4d10 5p3",206,8.64,1.07,903.78,1860,6.685 },
    { 52,"Te","Tellurium",127.6,0xD47A00,"[Kr]5s2 4d10 5p4",206,9.01,1.971,722.66,1261,6.232 },
    { 53,"I","Iodine",126.9045,0x940094,"[Kr]5s2 4d10 5p5",198,10.451,3.059,386.85,457.55,4.93 },
    { 54,"Xe","Xenon",131.29,0x429EB0,"[Kr]5s2 4d10 5p6",216,12.13,-0.8,161.36,165.03,0.005887 },
    { 55,"Cs","Cesium",132.905452,0x57178F,"[Xe]6s1",343,3.894,0.472,301.59,944,1.93 },
    { 56,"Ba","Barium",137.33,0x00C900,"[Xe]6s2",268,5.212,0.145,1000,2170,3.62 },
    { 57,"La","Lanthanum",138.9055,0x70D4FF,"[Xe]6s2 5d1",240,5.577,0.5,1191,3737,6.15 },
    { 58,"Ce","Cerium",140.116,0xFFFFC7,"[Xe]6s2 4f1 5d1",235,5.539,0.5,1071,3697,6.77 },
    { 59,"Pr","Praseodymium",140.90766,0xD9FFC7,"[Xe]6s2 4f3",239,5.464,0.11,1204,3793,6.77 },
    { 60,"Nd","Neodymium",144.24,0xC7FFC7,"[Xe]6s2 4f4",229,5.525,0.097,1294,3347,7.01 },
    { 61,"Pm","Promethium",144.91276,0xA3FFC7,"[Xe]6s2 4f5",236,5.55,0.129,1315,3273,7.26 },
    { 62,"Sm","Samarium",150.4,0x8FFFC7,"[Xe]6s2 4f6",229,5.644,0.162,1347,2067,7.52 },
    { 63,"Eu","Europium",151.964,0x61FFC7,"[Xe]6s2 4f7",233,5.67,0.116,1095,1802,5.24 },
    { 64,"Gd","Gadolinium",157.2,0x45FFC7,"[Xe]6s2 4f7 5d1",237,6.15,0.212,1586,3546,7.9 },
    { 65,"Tb","Terbium",158.92535,0x30FFC7,"[Xe]6s2 4f9",221,5.864,0.131,1629,3503,8.23 },
    { 66,"Dy","Dysprosium",162.5,0x1FFFC7,"[Xe]6s2 4f10",229,5.939,0.015,1685,2840,8.55 },
    { 67,"Ho","Holmium",164.93033,0x00FF9C,"[Xe]6s2 4f11",216,6.022,0.338,1747,2973,8.8 },
    { 68,"Er","Erbium",167.26,0x0,"[Xe]6s2 4f12",235,6.108,0.312,1802,3141,9.07 },
    { 69,"Tm","Thulium",168.93422,0x00D452,"[Xe]6s2 4f13",227,6.184,1.029,1818,2223,9.32 },
    { 70,"Yb","Ytterbium",173.05,0x00BF38,"[Xe]6s2 4f14",242,6.254,-0.02,1092,1469,6.9 },
    { 71,"Lu","Lutetium",174.9668,0x00AB24,"[Xe]6s2 4f14 5d1",221,5.426,0.239,1936,3675,9.84 },
    { 72,"Hf","Hafnium",178.49,0x4DC2FF,"[Xe]6s2 4f14 5d2",212,6.825,0.178,2506,4876,13.3 },
    { 73,"Ta","Tantalum",180.9479,0x4DA6FF,"[Xe]6s2 4f14 5d3",217,7.89,0.322,3290,5731,16.4 },
    { 74,"W","Tungsten",183.84,0x2194D6,"[Xe]6s2 4f14 5d4",210,7.98,0.815,3695,5828,19.3 },
    { 75,"Re","Rhenium",186.207,0x267DAB,"[Xe]6s2 4f14 5d5",217,7.88,0.15,3459,5869,20.8 },
    { 76,"Os","Osmium",190.2,0x266696,"[Xe]6s2 4f14 5d6",216,8.7,1.1,3306,5285,22.57 },
    { 77,"Ir","Iridium",192.22,0x175487,"[Xe]6s2 4f14 5d7",202,9.1,1.565,2719,4701,22.42 },
    { 78,"Pt","Platinum",195.08,0xD0D0E0,"[Xe]6s1 4f14 5d9",209,9,2.128,2041.55,4098,21.46 },
    { 79,"Au","Gold",196.96657,0xFFD123,"[Xe]6s1 4f14 5d10",166,9.226,2.309,1337.33,3129,19.282 },
    { 80,"Hg","Mercury",200.59,0xB8B8D0,"[Xe]6s2 4f14 5d10",209,10.438,-0.5,234.32,629.88,13.5336 },
    { 81,"Tl","Thallium",204.383,0xA6544D,"[Xe]6s2 4f14 5d10 6p1",196,6.108,0.2,577,1746,11.8 },
    { 82,"Pb","Lead",207,0x575961,"[Xe]6s2 4f14 5d10 6p2",202,7.417,0.36,600.61,2022,11.342 },
    { 83,"Bi","Bismuth",208.9804,0x9E4FB5,"[Xe]6s2 4f14 5d10 6p3",207,7.289,0.946,544.55,1837,9.807 },
    { 84,"Po","Polonium",208.98243,0xAB5C00,"[Xe]6s2 4f14 5d10 6p4",197,8.417,1.9,527,1235,9.32 },
    { 85,"At","Astatine",209.98715,0x754F45,"[Xe]6s2 4f14 5d10 6p5",202,9.5,2.8,575,623,7 },
    { 86,"Rn","Radon",222.01758,0x428296,"[Xe]6s2 4f14 5d10 6p6",220,10.745,-0.7,202,211.45,0.00973 },
    { 87,"Fr","Francium",223.01973,0x420066,"[Rn]7s1",348,3.9,0.47,300,923,2.4 },
    { 88,"Ra","Radium",226.02541,0x007D00,"[Rn]7s2",283,5.279,0.1,973,1413,5 },
    { 89,"Ac","Actinium",227.02775,0x70ABFA,"[Rn]7s2 6d1",260,5.17,0.35,1324,3471,10.07 },
    { 90,"Th","Thorium",232.038,0x00BAFF,"[Rn]7s2 6d2",237,6.08,0.6,2023,5061,11.72 },
    { 91,"Pa","Protactinium",231.03588,0x00A1FF,"[Rn]7s2 5f2 6d1",243,5.89,0.55,1845,4273,15.37 },
    { 92,"U","Uranium",238.0289,0x008FFF,"[Rn]7s2 5f3 6d1",240,6.194,0.315,1408,4404,18.95 },
    { 93,"Np","Neptunium",237.048172,0x0080FF,"[Rn]7s2 5f4 6d1",221,6.266,0.48,917,4175,20.25 },
    { 94,"Pu","Plutonium",244.0642,0x006BFF,"[Rn]7s2 5f6",243,6.06,-0.5,913,3501,19.84 },
    { 95,"Am","Americium",243.06138,0x545CF2,"[Rn]7s2 5f7",244,5.993,0.1,1449,2284,13.69 },
    { 96,"Cm","Curium",247.07035,0x785CE3,"[Rn]7s2 5f7 6d1",245,6.02,0.28,1618,3400,13.51 },
    { 97,"Bk","Berkelium",247.07031,0x8A4FE3,"[Rn]7s2 5f9",244,6.23,-1.72,1323,2900,14 },
    { 98,"Cf","Californium",251.07959,0xA136D4,"[Rn]7s2 5f10",245,6.3,-1.01,1173,1743,15.1 },
    { 99,"Es","Einsteinium",252.083,0xB31FD4,"[Rn]7s2 5f11",245,6.42,-0.3,1133,1269,8.84 },
  };

}
