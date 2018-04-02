/*
 * common.h
 *
 *  Created on: Dec 13, 2017
 *      Author: ranjeet
 */

#ifndef ENERGIMINER_COMMON_H_
#define ENERGIMINER_COMMON_H_

#include "Log.h"
#include "portable_endian.h"

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <mutex>
#include <array>

enum class MinerExecutionMode : unsigned
{
    kCPU   = 0x1,
    kCL    = 0x2,
    kMixed = 0x3,
    kCUDA  = 0x4
};

enum class EnumMinerEngine : unsigned
{
    kCPU    = 0x0,
    kCL     = 0x1,
    kTest   = 0x2
};

uint16_t inline ReadLE16(const unsigned char* ptr)
{
    return le16toh(*((uint16_t*)ptr));
}

uint32_t inline ReadLE32(const unsigned char* ptr)
{
    return le32toh(*((uint32_t*)ptr));
}

uint64_t inline ReadLE64(const unsigned char* ptr)
{
    return le64toh(*((uint64_t*)ptr));
}

void inline WriteLE16(unsigned char* ptr, uint16_t x)
{
    *((uint16_t*)ptr) = htole16(x);
}

void inline WriteLE32(unsigned char* ptr, uint32_t x)
{
    *((uint32_t*)ptr) = htole32(x);
}

void inline WriteLE64(unsigned char* ptr, uint64_t x)
{
    *((uint64_t*)ptr) = htole64(x);
}

uint32_t inline ReadBE32(const unsigned char* ptr)
{
    return be32toh(*((uint32_t*)ptr));
}

uint64_t inline ReadBE64(const unsigned char* ptr)
{
    return be64toh(*((uint64_t*)ptr));
}

void inline WriteBE32(unsigned char* ptr, uint32_t x)
{
    *((uint32_t*)ptr) = htobe32(x);
}

void inline WriteBE64(unsigned char* ptr, uint64_t x)
{
    *((uint64_t*)ptr) = htobe64(x);
}

inline std::vector<EnumMinerEngine> getEngineModes(MinerExecutionMode minerExecutionMode)
{
    std::vector<EnumMinerEngine> vEngine;
    if ( static_cast<unsigned>(minerExecutionMode) & static_cast<unsigned>(MinerExecutionMode::kCL) )
        vEngine.push_back(EnumMinerEngine::kCL);
    if ( static_cast<unsigned>(minerExecutionMode) & static_cast<unsigned>(MinerExecutionMode::kCPU) )
        vEngine.push_back(EnumMinerEngine::kCPU);

    return vEngine;
}

inline EnumMinerEngine getEngineMode(MinerExecutionMode minerExecutionMode)
{
    if ( static_cast<unsigned>(minerExecutionMode) & static_cast<unsigned>(MinerExecutionMode::kCL) )
        return EnumMinerEngine::kCL;
    if ( static_cast<unsigned>(minerExecutionMode) & static_cast<unsigned>(MinerExecutionMode::kCPU) )
        return EnumMinerEngine::kCPU;

    return EnumMinerEngine::kTest;
}

constexpr const char* StrMinerEngine[] = { "CPU", "CL", "Test" };

inline std::string to_string(EnumMinerEngine minerEngine)
{
    return StrMinerEngine[static_cast<int>(minerEngine)];
}

namespace energi
{
  using target    = std::array<uint32_t, 8>;
  using vchar     = std::vector<char>;
  using vbyte     = std::vector<uint8_t>;
  using vuint32   = std::vector<uint32_t>;

  inline void setBuffer(const uint8_t* ptr, uint32_t value) { *reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(ptr)) = value; }

  using MutexLGuard = std::lock_guard<std::mutex>;
  using MutexRLGuard = std::lock_guard<std::recursive_mutex>;

  inline std::string strToHex(const std::string& str)
  {
      std::ostringstream result;
      for (const auto& item : str) {
          result << std::hex
                 << std::setfill('0')
                 << std::setw(2)
                 << std::nouppercase
                 << static_cast<unsigned int>(static_cast<unsigned char>(item));
      }
      return result.str();
  }

  inline bool setenv(const char name[], const char value[], bool over = false)
  {
  #if _WIN32
    if (!over && std::getenv(name) != nullptr)
      return true;

    return ::_putenv_s(name, value) == 0;
  #else
    return ::setenv(name, value, over ? 1 : 0) == 0;
  #endif
  }

  class WorkException : public std::exception
  {
  public:
    WorkException(const std::string &reason):m_reason(reason){}
      const char* what() const noexcept override
      {
          return m_reason.c_str();
      }
  private:
      std::string m_reason;
  };

  inline std::string GetHex(const uint8_t* data, unsigned int size)
  {
      std::string psz(size * 2 + 1, '\0');
      for (unsigned int i = 0; i < size; i++)
          sprintf(const_cast<char*>(psz.data()) + i * 2, "%02x", data[size - i - 1]);
      return std::string(const_cast<char*>(psz.data()), const_cast<char*>(psz.data()) + size * 2);
  }

  /// Describes the progress of a mining operation.
  struct WorkingProgress
  {
    uint64_t hashes = 0;    ///< Total number of hashes computed.
    uint64_t ms = 0;        ///< Total number of milliseconds of mining thus far.
    uint64_t rate() const { return ms == 0 ? 0 : hashes * 1000 / ms; }

    std::map<std::string, uint64_t> minersHashes; // maps a miner's device name to it's hash count
    uint64_t minerRate(const uint64_t hashCount) const { return ms == 0 ? 0 : hashCount * 1000 / ms; }
  };

  inline std::ostream& operator<<(std::ostream& _out, WorkingProgress _p)
  {
    float mh = _p.rate() / 1000.0f;
    _out << "Speed "
       << EthTealBold << std::fixed << std::setw(6) << std::setprecision(2) << mh << EthReset
       << " Kh/s    ";

    for (auto const & i : _p.minersHashes)
    {
      mh = _p.minerRate(i.second) / 1000.0f;
      _out << i.first << " " << EthTeal << std::fixed << std::setw(5) << std::setprecision(2) << mh << EthReset << "  ";
    }

    return _out;
  }

  int varint_encode(unsigned char *p, uint64_t n);
  void bin2hex(char *s, const unsigned char *p, size_t len);
  char *abin2hex(const unsigned char *p, size_t len);
  bool hex2bin(unsigned char *p, const char *hexstr, size_t len);
  bool fulltest(const uint32_t *hash, const uint32_t *target);
  extern "C" void sha256d(unsigned char *hash, const unsigned char *data, int len);


  #define _ALIGN(x) __attribute__ ((aligned(x)))

  static inline bool is_windows(void) {
  #ifdef WIN32
    return 1;
  #else
    return 0;
  #endif
  }

  #if ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
  #define WANT_BUILTIN_BSWAP
  #else
  #define bswap_32(x) ((((x) << 24) & 0xff000000u) | (((x) << 8) & 0x00ff0000u) \
                     | (((x) >> 8) & 0x0000ff00u) | (((x) >> 24) & 0x000000ffu))
  #endif

  static inline uint32_t swab32(uint32_t v)
  {
  #ifdef WANT_BUILTIN_BSWAP
    return __builtin_bswap32(v);
  #else
    return bswap_32(v);
  #endif
  }

  #ifdef HAVE_SYS_ENDIAN_H
  #include <sys/endian.h>
  #endif

  typedef unsigned char uchar;

  #if !HAVE_DECL_BE32DEC
  static inline uint32_t be32dec(const void *pp)
  {
    const uint8_t *p = (uint8_t const *)pp;
    return ((uint32_t)(p[3]) + ((uint32_t)(p[2]) << 8) +
        ((uint32_t)(p[1]) << 16) + ((uint32_t)(p[0]) << 24));
  }
  #endif

  #if !HAVE_DECL_LE32DEC
  static inline uint32_t le32dec(const void *pp)
  {
    const uint8_t *p = (uint8_t const *)pp;
    return ((uint32_t)(p[0]) + ((uint32_t)(p[1]) << 8) +
        ((uint32_t)(p[2]) << 16) + ((uint32_t)(p[3]) << 24));
  }
  #endif

  #if !HAVE_DECL_BE32ENC
  static inline void be32enc(void *pp, uint32_t x)
  {
    uint8_t *p = (uint8_t *)pp;
    p[3] = x & 0xff;
    p[2] = (x >> 8) & 0xff;
    p[1] = (x >> 16) & 0xff;
    p[0] = (x >> 24) & 0xff;
  }
  #endif

  #if !HAVE_DECL_LE32ENC
  static inline void le32enc(void *pp, uint32_t x)
  {
    uint8_t *p = (uint8_t *)pp;
    p[0] = x & 0xff;
    p[1] = (x >> 8) & 0xff;
    p[2] = (x >> 16) & 0xff;
    p[3] = (x >> 24) & 0xff;
  }
  #endif

  #if !HAVE_DECL_LE16DEC
  static inline uint16_t le16dec(const void *pp)
  {
    const uint8_t *p = (uint8_t const *)pp;
    return ((uint16_t)(p[0]) + ((uint16_t)(p[1]) << 8));
  }
  #endif

  #if !HAVE_DECL_LE16ENC
  static inline void le16enc(void *pp, uint16_t x)
  {
    uint8_t *p = (uint8_t *)pp;
    p[0] = x & 0xff;
    p[1] = (x >> 8) & 0xff;
  }
  #endif
}


#endif /* ENERGIMINER_COMMON_H_ */