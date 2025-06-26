/**
 * @file def.hpp
 * @author lee kil hun (pba7189@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-06-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once
#ifndef COMMOM_DEF_HPP
#define COMMOM_DEF_HPP

#include <array>
#include <bitset>
#include <iostream>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


#include "core.hpp"

#ifndef aP_high
#define aP_high           true
#endif

#ifndef aP_low
#define aP_low           false
#endif

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS     0
#endif

#ifndef ERROR_FAIL
#define ERROR_FAIL       -1
#endif


#define MaKEWoRD(a, b)      ((uint16_t)(((uint8_t)(((uint32_t)(a)) & 0xff)) | ((uint16_t)((uint8_t)(((uint32_t)(b)) & 0xff))) << 8))
#define MaKEDWoRD(a, b)     ((uint32_t)(((uint16_t)(((uint32_t)(a)) & 0xffff)) | ((uint32_t)((uint16_t)(((uint32_t)(b)) & 0xffff))) << 16))
#define LoWoRD(l)           ((uint16_t)(((uint32_t)(l)) & 0xffff))
#define HiWoRD(l)           ((uint16_t)((((uint32_t)(l)) >> 16) & 0xffff))
#define LoBYTe(w)           ((uint8_t)(((uint16_t)(w)) & 0xff))
#define HiBYTe(w)           ((uint8_t)((((uint16_t)(w)) >> 8) & 0xff))

#include <array>
#include <bitset>
#include <iostream>

#include <cstdint>


/*
[[nodiscard]]는 C++17에서 도입된 속성(attribute) 으로, "함수의 반환값을 무시하면 경고를 발생시키라" 는 의미입니다.

*/



/*
항목	설명
std::bitset<32>	내부 비트 연산 관리 (편리하고 안전)
operator()(index)	비트 값 읽기
operator()(index, bool)	비트 값 설정
std::array<uint8_t, 4>	raw 바이트 접근 가능
set(uint32_t), get()	정수형으로 변환 및 복원 가능
constexpr 생성자	정적 초기화도 가능

*/
struct udword_bit
{
private:
  std::bitset<32>        bits;
  std::array<uint8_t, 4> raw_bytes{}; // 내부 저장용

  void sync_raw_bytes()
  {
    uint32_t raw = static_cast<uint32_t>(bits.to_ulong());
    raw_bytes[0] = static_cast<uint8_t>(raw >> 0);
    raw_bytes[1] = static_cast<uint8_t>(raw >> 8);
    raw_bytes[2] = static_cast<uint8_t>(raw >> 16);
    raw_bytes[3] = static_cast<uint8_t>(raw >> 24);
  }


public:
  constexpr udword_bit() = default;

  // uint32_t 초기화
  constexpr explicit udword_bit(uint32_t value) :
  bits(value),
  raw_bytes{static_cast<uint8_t>(value >> 0),
            static_cast<uint8_t>(value >> 8),
            static_cast<uint8_t>(value >> 16),
            static_cast<uint8_t>(value >> 24)} {}

  // raw value set
  void set(uint32_t value)
  {
    bits = value;
    sync_raw_bytes();
  }

  [[nodiscard]] uint32_t get() const
  {
    return static_cast<uint32_t>(bits.to_ulong());
  }

  [[nodiscard]] bool operator()(size_t index) const
  {
    if (index >= 32)
    {
      std::cerr << "Bit index out of range\n";
      return false;
    }
    return bits.test(index);
  }

  udword_bit &operator()(size_t index, bool value)
  {
    if (index >= 32)
    {
      std::cerr << "Bit index out of range\n";
      return *this;
    }
    bits.set(index, value);
    sync_raw_bytes();
    return *this;
  }

  [[nodiscard]] uint8_t operator[](size_t index) const
  {
    return index < 4 ? raw_bytes[index] : 0;
  }

  uint8_t &operator[](size_t index)
  {
    static uint8_t dummy = 0;
    return index < 4 ? raw_bytes[index] : dummy;
  }

  [[nodiscard]] const std::bitset<32> &bitset() const
  {
    return bits;
  }

  // ✅ set_mask: 지정한 마스크의 비트를 켠다
  udword_bit &set_mask(uint32_t mask)
  {
    bits |= mask;
    sync_raw_bytes();
    return *this;
  }

  // ✅ toggle_bit: 지정한 인덱스의 비트를 반전한다
  udword_bit &toggle_bit(size_t index)
  {
    if (index >= 32)
    {
      std::cerr << "Bit index out of range\n";
      return *this;
    }
    bits.flip(index);
    sync_raw_bytes();
    return *this;
  }

  // 비트 여러 개 동시에 Set (1로 설정)
  template <size_t... Bits>
  udword_bit &set_bits()
  {
    static_assert(((Bits < 32) && ...), "Bit index out of range");
    ((bits.set(Bits)), ...);
    sync_raw_bytes();
    return *this;
  }

  // 비트 여러 개 동시에 Clear (0으로 설정)
  template <size_t... Bits>
  udword_bit &clear_bits()
  {
    static_assert(((Bits < 32) && ...), "Bit index out of range");
    ((bits.reset(Bits)), ...);
    sync_raw_bytes();
    return *this;
  }
};



// '25.0508 보완
template <typename idxType = uint8_t, size_t idxOffset = 0>
union udword_st
{
  uint32_t u32;
  int32_t  s32;
  uint8_t  u8[4];
  int8_t   s8[4];
  uint16_t u16[2];
  int16_t  s16[2];

  inline uint32_t get() const { return u32; }

  inline udword_st &set_bit(idxType idx, bool val)
  {
    if constexpr (std::is_same_v<idxType, uint8_t>) // 8-bit
    {
      if (idx < 32)
      {
        if (val)
          u32 |= (1UL << idx);
        else
          u32 &= ~(1UL << idx);
      }
    }
    else
    {
      auto offset_idx = static_cast<uint8_t>(idx - idxOffset);
      if (offset_idx < 32)
      {
        if (val)
          u32 |= (1UL << offset_idx);
        else
          u32 &= ~(1UL << offset_idx);
      }
    }

    return *this;
  }

  inline bool get_bit(idxType idx) const
  {
    if constexpr (std::is_same_v<idxType, uint8_t>) // 8-bit
    {
      return idx < 32 ? ((u32 >> idx) & 0x01) : false;
    }
    else
    {
      auto offset_idx = static_cast<uint8_t>(idx - idxOffset);
      return offset_idx < 32 ? ((u32 >> idx) & 0x01) : false;
    }
  }

  inline uint32_t operator=(const uint32_t &data)
  {
    u32 = data;
    return u32;
  }

  udword_st &operator=(const std::array<uint8_t, 4> &arr)
  {
    for (size_t i = 0; i < 4; ++i)
    {
      ((uint8_t *)&u8)[i] = arr[i];
    }
    return *this;
  }

  uint8_t &operator[](size_t index)
  {
    static uint8_t dummy = 0;
    return (index < 4) ? u8[index] : dummy;
  }

  udword_st &operator()(idxType bit_idx, bool set)
  {
    return set_bit(bit_idx, set);
  }

  bool operator()(idxType bit_idx)
  {
    return get_bit(bit_idx);
  }

  // 비트 여러 개 동시에 Set (1로 설정)
  template <size_t... Bits>
  udword_st &set_bits()
  {
    (set_bit(Bits, true), ...);
    return *this;
  }

  // 비트 여러 개 동시에 Clear (0으로 설정)
  template <size_t... Bits>
  udword_st &clear_bits()
  {
    (set_bit(Bits, false), ...);
    return *this;
  }
};



/**
 * @brief
 * Data structure using 4-byte data in various forms
 */
union udword_t
{
  uint32_t u32Data;
  uint8_t  u8Data[4];
  uint16_t u16Data[2];
  int8_t   s8Data[4];
  int16_t  s16Data[2];
  uint32_t u32D;
  int32_t  s32Data;
  uint8_t  u8D;
  uint16_t u16D;
  int8_t   s8D;
  int16_t  s16D;
  int32_t  s32D;

  struct
  {
    unsigned _0  : 1; // = 0x00000001;
    unsigned _1  : 1; // = 0x00000002;
    unsigned _2  : 1; // = 0x00000004;
    unsigned _3  : 1; // = 0x00000008;
    unsigned _4  : 1; // = 0x00000010;
    unsigned _5  : 1; // = 0x00000020;
    unsigned _6  : 1; // = 0x00000040;
    unsigned _7  : 1; // = 0x00000080;
    unsigned _8  : 1; // = 0x00000100;
    unsigned _9  : 1; // = 0x00000200;
    unsigned _10 : 1; // = 0x00000400;
    unsigned _11 : 1; // = 0x00000800;
    unsigned _12 : 1; // = 0x00001000;
    unsigned _13 : 1; // = 0x00002000;
    unsigned _14 : 1; // = 0x00004000;
    unsigned _15 : 1; // = 0x00008000;
    unsigned _16 : 1; // = 0x00010000;
    unsigned _17 : 1; // = 0x00020000;
    unsigned _18 : 1; // = 0x00040000;
    unsigned _19 : 1; // = 0x00080000;
    unsigned _20 : 1; // = 0x00100000;
    unsigned _21 : 1; // = 0x00200000;
    unsigned _22 : 1; // = 0x00400000;
    unsigned _23 : 1; // = 0x00800000;
    unsigned _24 : 1; // = 0x01000000;
    unsigned _25 : 1; // = 0x02000000;
    unsigned _26 : 1; // = 0x04000000;
    unsigned _27 : 1; // = 0x08000000;
    unsigned _28 : 1; // = 0x10000000;
    unsigned _29 : 1; // = 0x20000000;
    unsigned _30 : 1; // = 0x40000000;
    unsigned _31 : 1; // = 0x80000000;
  };

  inline uint32_t get() const
  {
    return u32Data;
  }

  inline uint32_t *data() const
  {
    return (uint32_t *)&u32Data;
  }

  uint32_t operator=(const uint32_t &data)
  {
    u32Data = data;
    return u32Data;
  }

  udword_t &operator=(const std::array<uint8_t, 4> &arr)
  {
    for (size_t i = 0; i < 4; ++i)
    {
      ((uint8_t *)&u8Data)[i] = arr[i];
    }
    return *this;
  }

  uint8_t &operator[](size_t index)
  {
    if (index > 3)
    {
      std::cout << "Index out of range" << std::endl;
      return ((uint8_t *)&u8Data)[0];
    }
    return ((uint8_t *)&u8Data)[index];
  }

  udword_t &operator()(uint8_t bit_idx, bool set)
  {
    if (bit_idx > 31)
    {
      std::cout << "bit_idx out of range" << std::endl;
      return *this;
    }
    if (set)
      u32Data = u32Data | (1 << (size_t)bit_idx);
    else
      u32Data = u32Data & ~(1 << (size_t)bit_idx);
    return *this;
  }

  bool operator()(uint8_t bit_idx)
  {
    if (bit_idx > 31)
    {
      std::cout << "bit_idx out of range" << std::endl;
      return false;
    }
    return (u32Data & (1 << (size_t)bit_idx)) != 0;
  }

  // 비트 여러 개 동시에 Set (1로 설정)
  template <size_t... Bits>
  udword_t &set_bits()
  {
    static_assert(((Bits < 32) && ...), "Bit index out of range (0~31)");
    ((u32Data |= (1UL << Bits)), ...);
    return *this;
  }

  // 비트 여러 개 동시에 Clear (0으로 설정)
  template <size_t... Bits>
  udword_t &clear_bits()
  {
    static_assert(((Bits < 32) && ...), "Bit index out of range (0~31)");
    ((u32Data &= ~(1UL << Bits)), ...);
    return *this;
  }

  /*
  사용 예
  udword_t flag;
  flag = 0;

  // 1, 4, 5, 6번 비트를 1로 설정
  flag.set_bits<1, 4, 5, 6>();

  // 4, 5번 비트를 0으로 클리어
  flag.clear_bits<4, 5>();

  */
};

// 통신 상태를 확인
// life가 업데이트가 지속되는 상태
// 타이머 1초에서 update를 호출한다.
// 통신이 처리 완료 히  reset을 한다.
// max카운터 보다 작아야 한다. 
struct check_comm_t
{
  using dat_t = uint16_t;
  dat_t life{};
  dat_t lift_max{3};

  check_comm_t() = default;
  ~check_comm_t() = default;

  inline void update()
  {
    if (life < lift_max)
      life++;
  }

  //  
  inline void reset_flag()
  {
    life = 0;
  }

  inline bool is_lift() const
  {
    return (life < lift_max);
  }


};

#endif // COMMOM_DEF_HPP
