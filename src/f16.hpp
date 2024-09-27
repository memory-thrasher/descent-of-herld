/*
  Copyright 2024 Wafflecat Games, LLC

  This file is part of The Descent of Herld.

  The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

  Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

namespace doh {

  constexpr uint16_t f16Epsilon = 1,//lowest positive number
    f16NegativeEpsilon = 0x8001;//highest negative number

  //NOTE: the pack functions built into glm are garbage. Individual components can wrap around, turning values below f16epsilon into infinity or NaN. 1.0 is encoded as NaN.
  //roundUp: especially turn a input value below f16 epsilon into f16 epsilon rather than into 0 (and never NaN)
  constexpr uint16_t f16Encode(float v, bool roundUp = true) {
    static_assert(sizeof(float) == sizeof(uint32_t));
    const uint32_t i = std::bit_cast<uint32_t>(v);
    const uint32_t sign = i >> 31,
      exp_full = (i >> 23) & 0xFF,
      mantissa_full = i & 0x7FFFFF;
    //f32 exp norm is (1+M*2^-23)*2^(E-127), f16 subnorm = M*(2^-24) (aka (M/2^10)*2^-14)
    if(exp_full < 103) {
      if((exp_full == 0 && mantissa_full == 0) || !roundUp)
	return sign ? 0x8000 : 0;//±0
      else
	return sign ? f16NegativeEpsilon : f16Epsilon;//±epsilon (every fractional value in this range is too smal for f16)
    }
    if(exp_full <= 112) //normal in f32, subnormal in f16 (this is the hard part)
      //exp = 0, mantissa needs the implicit 1 added in
      return (sign << 15) | ((mantissa_full | 0x800000) >> (14 + 112 - exp_full));
    return (sign << 15) | ((exp_full - 112) << 10) | (mantissa_full >> 13);
  };

  constexpr bool f16isNaN(uint16_t v) {
    return !(~v & 0x7C00) && (v & 0x3FF);//E = 31, M ≠ 0
  };

  constexpr bool f16Less(uint16_t l, uint16_t r) {
    //f16 like most float types orders bits in order of significance, so the only difference between this and signed is NaN
    if(f16isNaN(l) || f16isNaN(r)) return false;
    return std::bit_cast<int16_t>(l) < std::bit_cast<int16_t>(r);
  };

  constexpr bool f16Equal(uint16_t l, uint16_t r) {
    if(f16isNaN(l) || f16isNaN(r)) return false;
    return l == r;
  };

  //just some quick sanity checks
  static_assert(f16Less(f16Epsilon, f16Encode(1)));
  static_assert(f16Less(f16Encode(0), f16Epsilon));
  static_assert(!f16isNaN(0x3FF));
  static_assert(!f16isNaN(f16NegativeEpsilon));
  static_assert(std::bit_cast<int16_t>(uint16_t(0x8000)) < std::bit_cast<int16_t>(uint16_t(0)));
  static_assert(f16Less(f16NegativeEpsilon, f16Encode(0)));
  static_assert(!f16Equal(f16Encode(0), f16Epsilon));
  static_assert(!f16Equal(f16NegativeEpsilon, f16Encode(0)));
  static_assert(!f16isNaN(f16Encode(1.0f/(1 << 25))));
  static_assert(f16Less(f16Encode(1.0f/(1 << 25)), f16Encode(1)));
  static_assert(f16Less(f16Encode(0), f16Encode(1.0f/(1 << 25))));
  static_assert(f16Less(f16Encode(1.5f), f16Encode(2)));
  static_assert(f16Less(f16Encode(1), f16Encode(1.5f)));

  constexpr uint64_t pack16x4(uint16_t a, uint16_t b, uint16_t c, uint16_t d) {
    return (a << 48) | (b << 32) | (c << 16) | d;
  };

  constexpr uint64_t f16Pack4(glm::vec4 v) {
    return pack16x4(f16Encode(v.x), f16Encode(v.y), f16Encode(v.z), f16Encode(v.w));
  };

}
