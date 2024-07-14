# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: // This file contains functions to read an archive file (.a file).
// An archive file is just a bundle of object files. It's similar to
// tar or zip, but the contents are not compressed.
//
// An archive file is either "regular" or "thin". A regular archive
// contains object files directly, while a thin archive contains only
// pathnames. In the latter case, actual file contents have to be read
// from given pathnames. A regular archive is sometimes called "fat"
// archive as opposed to "thin".
//
// If an archive file is given to the linker, the linker pulls out
// object files that are needed to resolve undefined symbols. So,
// bunding object files as an archive and giving that archive to the
// linker has a different meaning than directly giving the same set of
// object files to the linker. The former links only needed object
// files, while the latter links all the given object files.
//
// Therefore, if you link libc.a for example, not all the libc
// functions are linked to your binary. Instead, only object files
// that provides functions and variables used in your program get
// linked. To make this efficient, static library functions are
// usually separated to each object file in an archive file. You can
// see the contents of libc.a by running `ar t
// /usr/lib/x86_64-linux-gnu/libc.a`.

#pragma once

#include "common.h"
#include "filetype.h"

namespace mold {

struct ArHdr {
  char ar_name[16];
  char ar_date[12];
  char ar_uid[6];
  char ar_gid[6];
  char ar_mode[8];
  char ar_size[10];
  char ar_fmag[2];

  bool starts_with(std::string_view s) const {
    return std::string_view(ar_name, s.size()) == s;
  }

  bool is_strtab() const {
    return starts_with("// ");
  }

  bool is_symtab() const {
    return starts_with("/ ") || starts_with("/SYM64/ ");
  }

  std::string read_name(std::string_view strtab, u8 *&ptr) const {
    // BSD-style long filename
    if (starts_with("#1/")) {
      int namelen = atoi(ar_name + 3);
      std::string name{(char *)ptr, (size_t)namelen};
      ptr += namelen;

      if (size_t pos = name.find('\0'))
        name = name.substr(0, pos);
      return name;
    }

    // SysV-style long filename
    if (starts_with("/")) {
      const char *start = strtab.data() + atoi(ar_name + 1);
      return {start, (const char *)strstr(start, "/\n")};
    }

    // Short fileanme
    if (const char *end = (char *)memchr(ar_name, '/', sizeof(ar_name)))
      return {ar_name, end};
    return {ar_name, sizeof(ar_name)};
  }
};

template <typename Context, typename MappedFile>
std::vector<MappedFile *>
read_thin_archive_members(Context &ctx, MappedFile *mf) {
  u8 *begin = mf->data;
  u8 *data = begin + 8;
  std::vector<MappedFile *> vec;
  std::string_view strtab;

  while (data < begin + mf->size) {
    // Each header is aligned to a 2 byte boundary.
    if ((begin - data) % 2)
      data++;

    ArHdr &hdr = *(ArHdr *)data;
    u8 *body = data + sizeof(hdr);
    u64 size = atol(hdr.ar_size);

    // Read a string table.
    if (hdr.is_strtab()) {
      strtab = {(char *)body, (size_t)size};
      data = body + size;
      continue;
    }

    // Skip a symbol table.
    if (hdr.is_symtab()) {
      data = body + size;
      continue;
    }

    if (!hdr.starts_with("#1/") && !hdr.starts_with("/"))
      Fatal(ctx) << mf->name << ": filename is not stored as a long filename";

    std::string name = hdr.read_name(strtab, body);

    // Skip if symbol table
    if (name == "__.SYMDEF" || name == "__.SYMDEF SORTED")
      continue;

    std::string path = name.starts_with('/') ?
      name : (filepath(mf->name).parent_path() / name).string();
    vec.push_back(must_open_file(ctx, path));
    vec.back()->thin_parent = mf;
    data = body;
  }
  return vec;
}

template <typename Context, typename MappedFile>
std::vector<MappedFile *> read_fat_archive_members(Context &ctx, MappedFile *mf) {
  u8 *begin = mf->data;
  u8 *data = begin + 8;
  std::vector<MappedFile *> vec;
  std::string_view strtab;

  while (begin + mf->size - data >= 2) {
    if ((begin - data) % 2)
      data++;

    ArHdr &hdr = *(ArHdr *)data;
    u8 *body = data + sizeof(hdr);
    u64 size = atol(hdr.ar_size);
    data = body + size;

    // Read if string table
    if (hdr.is_strtab()) {
      strtab = {(char *)body, (size_t)size};
      continue;
    }

    // Skip if symbol table
    if (hdr.is_symtab())
      continue;

    // Read the name field
    std::string name = hdr.read_name(strtab, body);

    // Skip if symbol table
    if (name == "__.SYMDEF" || name == "__.SYMDEF SORTED")
      continue;

    vec.push_back(mf->slice(ctx, name, body - begin, data - body));
  }
  return vec;
}

template <typename Context, typename MappedFile>
std::vector<MappedFile *> read_archive_members(Context &ctx, MappedFile *mf) {
  switch (get_file_type(ctx, mf)) {
  case FileType::AR:
    return read_fat_archive_members(ctx, mf);
  case FileType::THIN_AR:
    return read_thin_archive_members(ctx, mf);
  default:
    unreachable();
  }
}

} // namespace mold
  push: #pragma once

#include "integers.h"

#include <array>
#include <atomic>
#include <bit>
#include <bitset>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <tbb/concurrent_vector.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/parallel_for.h>
#include <vector>

#ifdef _WIN32
# include <io.h>
#else
# include <sys/mman.h>
# include <unistd.h>
#endif

#define XXH_INLINE_ALL 1
#include "../third-party/xxhash/xxhash.h"

#ifdef NDEBUG
# define unreachable() __builtin_unreachable()
#else
# define unreachable() assert(0 && "unreachable")
#endif

inline uint64_t hash_string(std::string_view str) {
  return XXH3_64bits(str.data(), str.size());
}

class HashCmp {
public:
  static size_t hash(const std::string_view &k) {
    return hash_string(k);
  }

  static bool equal(const std::string_view &k1, const std::string_view &k2) {
    return k1 == k2;
  }
};

namespace mold {

using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;

template <typename Context> class OutputFile;

inline char *output_tmpfile;

inline u8 *output_buffer_start = nullptr;
inline u8 *output_buffer_end = nullptr;

extern std::string mold_git_hash;

std::string errno_string();
std::string get_self_path();
void cleanup();
void install_signal_handler();

static u64 combine_hash(u64 a, u64 b) {
  return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
}

//
// Error output
//

// Some C++ stdlibs don't support std::osyncstream even though
// it's is in the C++20 standard. So we implement it ourselves.
class SyncStream {
public:
  SyncStream(std::ostream &out) : out(out) {}

  ~SyncStream() {
    emit();
  }

  template <typename T> SyncStream &operator<<(T &&val) {
    ss << std::forward<T>(val);
    return *this;
  }

  void emit() {
    if (emitted)
      return;

    std::scoped_lock lock(mu);
    out << ss.str() << '\n';
    emitted = true;
  }

private:
  std::ostream &out;
  std::stringstream ss;
  bool emitted = false;
  static inline std::mutex mu;
};

template <typename Context>
class Out {
public:
  Out(Context &ctx) {}

  template <typename T> Out &operator<<(T &&val) {
    out << std::forward<T>(val);
    return *this;
  }

private:
  SyncStream out{std::cout};
};

static std::string_view fatal_mono = "mold: fatal: ";
static std::string_view fatal_color = "mold: \033[0;1;31mfatal:\033[0m ";
static std::string_view error_mono = "mold: error: ";
static std::string_view error_color = "mold: \033[0;1;31merror:\033[0m ";
static std::string_view warning_mono = "mold: warning: ";
static std::string_view warning_color = "mold: \033[0;1;35mwarning:\033[0m ";

template <typename Context>
class Fatal {
public:
  Fatal(Context &ctx) {
    out << (ctx.arg.color_diagnostics ? fatal_color : fatal_mono);
  }

  [[noreturn]] ~Fatal() {
    out.emit();
    cleanup();
    _exit(1);
  }

  template <typename T> Fatal &operator<<(T &&val) {
    out << std::forward<T>(val);
    return *this;
  }

private:
  SyncStream out{std::cerr};
};

template <typename Context>
class Error {
public:
  Error(Context &ctx) {
    if (ctx.arg.noinhibit_exec) {
      out << (ctx.arg.color_diagnostics ? warning_color : warning_mono);
    } else {
      out << (ctx.arg.color_diagnostics ? error_color : error_mono);
      ctx.has_error = true;
    }
  }

  template <typename T> Error &operator<<(T &&val) {
    out << std::forward<T>(val);
    return *this;
  }

private:
  SyncStream out{std::cerr};
};

template <typename Context>
class Warn {
public:
  Warn(Context &ctx) {
    if (ctx.arg.suppress_warnings)
      return;

    out.emplace(std::cerr);

    if (ctx.arg.fatal_warnings) {
      *out << (ctx.arg.color_diagnostics ? error_color : error_mono);
      ctx.has_error = true;
    } else {
      *out << (ctx.arg.color_diagnostics ? warning_color : warning_mono);
    }
  }

  template <typename T> Warn &operator<<(T &&val) {
    if (out)
      *out << std::forward<T>(val);
    return *this;
  }

private:
  std::optional<SyncStream> out;
};

//
// Atomics
//

// This is the same as std::atomic except that the default memory
// order is relaxed instead of sequential consistency.
template <typename T>
struct Atomic : std::atomic<T> {
  static constexpr std::memory_order relaxed = std::memory_order_relaxed;

  using std::atomic<T>::atomic;

  Atomic(const Atomic<T> &other) : std::atomic<T>(other.load()) {}

  Atomic<T> &operator=(const Atomic<T> &other) {
    store(other.load());
    return *this;
  }

  void operator=(T val) { store(val); }
  operator T() const { return load(); }

  void store(T val, std::memory_order order = relaxed) {
    std::atomic<T>::store(val, order);
  }

  T load(std::memory_order order = relaxed) const {
    return std::atomic<T>::load(order);
  }

  T exchange(T val) { return std::atomic<T>::exchange(val, relaxed); }
  T operator|=(T val) { return std::atomic<T>::fetch_or(val, relaxed); }
  T operator++() { return std::atomic<T>::fetch_add(1, relaxed) + 1; }
  T operator--() { return std::atomic<T>::fetch_sub(1, relaxed) - 1; }
  T operator++(int) { return std::atomic<T>::fetch_add(1, relaxed); }
  T operator--(int) { return std::atomic<T>::fetch_sub(1, relaxed); }

  bool test_and_set() {
    // A relaxed load + branch (assuming miss) takes only around 20 cycles,
    // while an atomic RMW can easily take hundreds on x86. We note that it's
    // common that another thread beat us in marking, so doing an optimistic
    // early test tends to improve performance in the ~20% ballpark.
    return load() || exchange(true);
  }
};

//
// perf.cc
//

// Counter is used to collect statistics numbers.
class Counter {
public:
  Counter(std::string_view name, i64 value = 0) : name(name), values(value) {
    static std::mutex mu;
    std::scoped_lock lock(mu);
    instances.push_back(this);
  }

  Counter &operator++(int) {
    if (enabled) [[unlikely]]
      values.local()++;
    return *this;
  }

  Counter &operator+=(int delta) {
    if (enabled) [[unlikely]]
      values.local() += delta;
    return *this;
  }

  static void print();

  static inline bool enabled = false;

private:
  i64 get_value();

  std::string_view name;
  tbb::enumerable_thread_specific<i64> values;

  static inline std::vector<Counter *> instances;
};

// Timer and TimeRecord records elapsed time (wall clock time)
// used by each pass of the linker.
struct TimerRecord {
  TimerRecord(std::string name, TimerRecord *parent = nullptr);
  void stop();

  std::string name;
  TimerRecord *parent;
  tbb::concurrent_vector<TimerRecord *> children;
  i64 start;
  i64 end;
  i64 user;
  i64 sys;
  bool stopped = false;
};

void
print_timer_records(tbb::concurrent_vector<std::unique_ptr<TimerRecord>> &);

template <typename Context>
class Timer {
public:
  Timer(Context &ctx, std::string name, Timer *parent = nullptr) {
    record = new TimerRecord(name, parent ? parent->record : nullptr);
    ctx.timer_records.push_back(std::unique_ptr<TimerRecord>(record));
  }

  Timer(const Timer &) = delete;

  ~Timer() {
    record->stop();
  }

  void stop() {
    record->stop();
  }

private:
  TimerRecord *record;
};

//
// Bit vector
//

class BitVector {
public:
  BitVector() = default;
  BitVector(u32 size) : vec((size + 7) / 8) {}

  void resize(u32 size) { vec.resize((size + 7) / 8); }
  bool get(u32 idx) const { return vec[idx / 8] & (1 << (idx % 8)); }
  void set(u32 idx) { vec[idx / 8] |= 1 << (idx % 8); }

private:
  std::vector<u8> vec;
};

//
// Utility functions
//

// Some C++ libraries haven't implemented std::has_single_bit yet.
inline bool has_single_bit(u64 val) {
  return std::popcount(val) == 1;
}

// Some C++ libraries haven't implemented std::bit_ceil yet.
inline u64 bit_ceil(u64 val) {
  if (has_single_bit(val))
    return val;
  return 1LL << (64 - std::countl_zero(val));
}

inline u64 align_to(u64 val, u64 align) {
  if (align == 0)
    return val;
  assert(has_single_bit(align));
  return (val + align - 1) & ~(align - 1);
}

inline u64 align_down(u64 val, u64 align) {
  assert(has_single_bit(align));
  return val & ~(align - 1);
}

inline u64 bit(u64 val, i64 pos) {
  return (val >> pos) & 1;
};

// Returns [hi:lo] bits of val.
inline u64 bits(u64 val, u64 hi, u64 lo) {
  return (val >> lo) & ((1LL << (hi - lo + 1)) - 1);
}

inline i64 sign_extend(u64 val, i64 size) {
  return (i64)(val << (63 - size)) >> (63 - size);
}

template <typename T, typename Compare = std::less<T>>
void update_minimum(std::atomic<T> &atomic, u64 new_val, Compare cmp = {}) {
  T old_val = atomic.load(std::memory_order_relaxed);
  while (cmp(new_val, old_val) &&
         !atomic.compare_exchange_weak(old_val, new_val,
                                       std::memory_order_relaxed));
}

template <typename T, typename Compare = std::less<T>>
void update_maximum(std::atomic<T> &atomic, u64 new_val, Compare cmp = {}) {
  T old_val = atomic.load(std::memory_order_relaxed);
  while (cmp(old_val, new_val) &&
         !atomic.compare_exchange_weak(old_val, new_val,
                                       std::memory_order_relaxed));
}

template <typename T>
inline void append(std::vector<T> &x, const auto &y) {
  x.insert(x.end(), y.begin(), y.end());
}

template <typename T>
inline std::vector<T> flatten(std::vector<std::vector<T>> &vec) {
  i64 size = 0;
  for (std::vector<T> &v : vec)
    size += v.size();

  std::vector<T> ret;
  ret.reserve(size);
  for (std::vector<T> &v : vec)
    append(ret, v);
  return ret;
}

inline void sort(auto &vec) {
  std::stable_sort(vec.begin(), vec.end());
}

inline void sort(auto &vec, auto less) {
  std::stable_sort(vec.begin(), vec.end(), less);
}

template <typename T>
inline void remove_duplicates(std::vector<T> &vec) {
  vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

inline i64 write_string(void *buf, std::string_view str) {
  memcpy(buf, str.data(), str.size());
  *((u8 *)buf + str.size()) = '\0';
  return str.size() + 1;
}

template <typename T>
inline i64 write_vector(void *buf, const std::vector<T> &vec) {
  i64 sz = vec.size() * sizeof(T);
  memcpy(buf, vec.data(), sz);
  return sz;
}

inline void encode_uleb(std::vector<u8> &vec, u64 val) {
  do {
    u8 byte = val & 0x7f;
    val >>= 7;
    vec.push_back(val ? (byte | 0x80) : byte);
  } while (val);
}

inline void encode_sleb(std::vector<u8> &vec, i64 val) {
  for (;;) {
    u8 byte = val & 0x7f;
    val >>= 7;

    bool neg = (byte & 0x40);
    if ((val == 0 && !neg) || (val == -1 && neg)) {
      vec.push_back(byte);
      break;
    }
    vec.push_back(byte | 0x80);
  }
}

inline i64 write_uleb(u8 *buf, u64 val) {
  i64 i = 0;
  do {
    u8 byte = val & 0x7f;
    val >>= 7;
    buf[i++] = val ? (byte | 0x80) : byte;
  } while (val);
  return i;
}

inline u64 read_uleb(u8 **buf) {
  u64 val = 0;
  u8 shift = 0;
  u8 byte;
  do {
    byte = *(*buf)++;
    val |= (byte & 0x7f) << shift;
    shift += 7;
  } while (byte & 0x80);
  return val;
}

inline u64 read_uleb(u8 *buf) {
  u8 *tmp = buf;
  return read_uleb(&tmp);
}

inline u64 read_uleb(std::string_view *str) {
  u8 *start = (u8 *)str->data();
  u8 *ptr = start;
  u64 val = read_uleb(&ptr);
  *str = str->substr(ptr - start);
  return val;
}

inline u64 read_uleb(std::string_view str) {
  std::string_view tmp = str;
  return read_uleb(&tmp);
}

inline i64 uleb_size(u64 val) {
  for (int i = 1; i < 9; i++)
    if (val < (1LL << (7 * i)))
      return i;
  return 9;
}

inline void overwrite_uleb(u8 *loc, u64 val) {
  while (*loc & 0b1000'0000) {
    *loc++ = 0b1000'0000 | (val & 0b0111'1111);
    val >>= 7;
  }
  *loc = val & 0b0111'1111;
}

template <typename Context>
std::string_view save_string(Context &ctx, const std::string &str) {
  u8 *buf = new u8[str.size() + 1];
  memcpy(buf, str.data(), str.size());
  buf[str.size()] = '\0';
  ctx.string_pool.push_back(std::unique_ptr<u8[]>(buf));
  return {(char *)buf, str.size()};
}

inline bool remove_prefix(std::string_view &s, std::string_view prefix) {
  if (s.starts_with(prefix)) {
    s = s.substr(prefix.size());
    return true;
  }
  return false;
}

static inline void pause() {
#if defined(__x86_64__)
  asm volatile("pause");
#elif defined(__aarch64__)
  asm volatile("yield");
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_8A__)
  asm volatile("yield");
#endif
}

//
// Concurrent Map
//

// This is an implementation of a fast concurrent hash map. Unlike
// ordinary hash tables, this impl just aborts if it becomes full.
// So you need to give a correct estimation of the final size before
// using it. We use this hash map to uniquify pieces of data in
// mergeable sections.
//
// We've implemented this ourselves because the performance of
// conrurent hash map is critical for our linker.
template <typename T>
class ConcurrentMap {
public:
  ConcurrentMap() = default;

  ConcurrentMap(i64 nbuckets) {
    resize(nbuckets);
  }

  ~ConcurrentMap() {
    if (entries) {
#ifdef _WIN32
      _aligned_free(entries);
#else
      munmap(entries, sizeof(Entry) * nbuckets);
#endif
    }
  }

  // In order to avoid unnecessary cache-line false sharing, we want
  // to make this object to be aligned to a reasonably large
  // power-of-two address.
  struct alignas(32) Entry {
    Atomic<const char *> key;
    u32 keylen;
    T value;
  };

  void resize(i64 nbuckets) {
    assert(!entries);
    this->nbuckets = std::max<i64>(MIN_NBUCKETS, bit_ceil(nbuckets));
    i64 bufsize = sizeof(Entry) * this->nbuckets;

    // Allocate a zero-initialized buffer. We use mmap() if available
    // because it's faster than malloc() and memset().
#ifdef _WIN32
    entries = (Entry *)_aligned_malloc(bufsize, alignof(Entry));
    memset((void *)entries, 0, bufsize);
#else
    entries = (Entry *)mmap(nullptr, bufsize, PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
#endif
  }

  std::pair<T *, bool> insert(std::string_view key, u32 hash, const T &val) {
    assert(has_single_bit(nbuckets));

    i64 begin = hash & (nbuckets - 1);
    u64 mask = nbuckets / NUM_SHARDS - 1;

    for (i64 i = 0; i < MAX_RETRY; i++) {
      i64 idx = (begin & ~mask) | ((begin + i) & mask);
      Entry &ent = entries[idx];

      // It seems avoiding compare-and-swap is faster overall at least
      // on my Zen4 machine, so do it.
      if (const char *ptr = ent.key.load(std::memory_order_acquire);
          ptr != nullptr && ptr != (char *)-1) {
        if (key == std::string_view(ptr, ent.keylen))
          return {&ent.value, false};
        continue;
      }

      // Otherwise, use CAS to atomically claim the ownership of the slot.
      const char *ptr = nullptr;
      bool claimed = ent.key.compare_exchange_strong(ptr, (char *)-1,
                                                     std::memory_order_acquire);

      // If we successfully claimed the ownership of the slot,
      // copy values to it.
      if (claimed) {
        new (&ent.value) T(val);
        ent.keylen = key.size();
        ent.key.store(key.data(), std::memory_order_release);
        return {&ent.value, true};
      }

      // If someone is copying values to the slot, do busy wait.
      while (ptr == (char *)-1) {
        pause();
        ptr = ent.key.load(std::memory_order_acquire);
      }

      // If the same key is already present, this is the slot we are
      // looking for.
      if (key == std::string_view(ptr, ent.keylen))
        return {&ent.value, false};
    }

    assert(false && "ConcurrentMap is full");
    return {nullptr, false};
  }

  i64 get_idx(T *value) const {
    uintptr_t addr = (uintptr_t)value - (uintptr_t)value % sizeof(Entry);
    return (Entry *)addr - entries;
  }

  // Return a list of map entries sorted in a deterministic order.
  std::vector<Entry *> get_sorted_entries(i64 shard_idx) {
    if (nbuckets == 0)
      return {};

    i64 shard_size = nbuckets / NUM_SHARDS;
    i64 begin = shard_idx * shard_size;
    i64 end = begin + shard_size;

    i64 sz = 0;
    for (i64 i = begin; i < end; i++)
      if (entries[i].key)
        sz++;

    std::vector<Entry *> vec;
    vec.reserve(sz);

    // Since the shard is circular, we need to handle the last entries
    // as if they were next to the first entries.
    while (entries[end - 1].key)
      vec.push_back(entries + --end);

    // Find entries contiguous in the buckets and sort them.
    i64 last = 0;
    for (i64 i = begin; i < end;) {
      while (i < end && entries[i].key)
        vec.push_back(entries + i++);

      std::sort(vec.begin() + last, vec.end(), [](Entry *a, Entry *b) {
        if (a->keylen != b->keylen)
          return a->keylen < b->keylen;
        return memcmp(a->key, b->key, a->keylen) < 0;
      });

      last = vec.size();

      while (i < end && !entries[i].key)
        i++;
    }
    return vec;
  }

  std::vector<Entry *> get_sorted_entries_all() {
    std::vector<std::vector<Entry *>> vec(NUM_SHARDS);
    tbb::parallel_for((i64)0, NUM_SHARDS, [&](i64 i) {
      vec[i] = get_sorted_entries(i);
    });
    return flatten(vec);
  }

  static constexpr i64 MIN_NBUCKETS = 2048;
  static constexpr i64 NUM_SHARDS = 16;
  static constexpr i64 MAX_RETRY = 128;

  Entry *entries = nullptr;
  i64 nbuckets = 0;
};

//
// random.cc
//

void get_random_bytes(u8 *buf, i64 size);

//
// output-file.h
//

template <typename Context>
class OutputFile {
public:
  static std::unique_ptr<OutputFile<Context>>
  open(Context &ctx, std::string path, i64 filesize, int perm);

  virtual void close(Context &ctx) = 0;
  virtual ~OutputFile() = default;

  u8 *buf = nullptr;
  std::vector<u8> buf2;
  std::string path;
  int fd = -1;
  i64 filesize = 0;
  bool is_mmapped = false;
  bool is_unmapped = false;

protected:
  OutputFile(std::string path, i64 filesize, bool is_mmapped)
    : path(path), filesize(filesize), is_mmapped(is_mmapped) {}
};

template <typename Context>
class MallocOutputFile : public OutputFile<Context> {
public:
  MallocOutputFile(Context &ctx, std::string path, i64 filesize, int perm)
    : OutputFile<Context>(path, filesize, false), ptr(new u8[filesize]),
      perm(perm) {
    this->buf = ptr.get();
  }

  void close(Context &ctx) override {
    Timer t(ctx, "close_file");
    FILE *fp;

    if (this->path == "-") {
      fp = stdout;
    } else {
#ifdef _WIN32
      int pmode = (perm & 0200) ? (_S_IREAD | _S_IWRITE) : _S_IREAD;
      i64 fd = _open(this->path.c_str(), _O_RDWR | _O_CREAT | _O_BINARY, pmode);
#else
      i64 fd = ::open(this->path.c_str(), O_RDWR | O_CREAT, perm);
#endif
      if (fd == -1)
        Fatal(ctx) << "cannot open " << this->path << ": " << errno_string();
#ifdef _WIN32
      fp = _fdopen(fd, "wb");
#else
      fp = fdopen(fd, "w");
#endif
    }

    fwrite(this->buf, this->filesize, 1, fp);
    if (!this->buf2.empty())
      fwrite(this->buf2.data(), this->buf2.size(), 1, fp);
    fclose(fp);
  }

private:
  std::unique_ptr<u8[]> ptr;
  int perm;
};

template <typename Context>
class LockingOutputFile : public OutputFile<Context> {
public:
  LockingOutputFile(Context &ctx, std::string path, int perm);
  void resize(Context &ctx, i64 filesize);
  void close(Context &ctx) override;
};

//
// hyperloglog.cc
//

class HyperLogLog {
public:
  void insert(u64 hash) {
    update_maximum(buckets[hash & (NBUCKETS - 1)], std::countl_zero(hash) + 1);
  }

  i64 get_cardinality() const;

  void merge(const HyperLogLog &other) {
    for (i64 i = 0; i < NBUCKETS; i++)
      update_maximum(buckets[i], other.buckets[i]);
  }

private:
  static constexpr i64 NBUCKETS = 2048;
  static constexpr double ALPHA = 0.79402;

  Atomic<u8> buckets[NBUCKETS];
};

//
// glob.cc
//

class Glob {
  typedef enum { STRING, STAR, QUESTION, BRACKET } Kind;

  struct Element {
    Element(Kind k) : kind(k) {}
    Kind kind;
    std::string str;
    std::bitset<256> bitset;
  };

public:
  static std::optional<Glob> compile(std::string_view pat);
  bool match(std::string_view str);

private:
  Glob(std::vector<Element> &&vec) : elements(vec) {}
  static bool do_match(std::string_view str, std::span<Element> elements);

  std::vector<Element> elements;
};

//
// multi-glob.cc
//

class MultiGlob {
public:
  bool add(std::string_view pat, i64 val);
  bool empty() const { return strings.empty(); }
  std::optional<i64> find(std::string_view str);

private:
  struct TrieNode {
    i64 value = -1;
    TrieNode *suffix_link = nullptr;
    std::unique_ptr<TrieNode> children[256];
  };

  void compile();
  void fix_suffix_links(TrieNode &node);
  void fix_values();
  i64 find_aho_corasick(std::string_view str);

  std::vector<std::string> strings;
  std::unique_ptr<TrieNode> root;
  std::vector<std::pair<Glob, i64>> globs;
  std::once_flag once;
  bool is_compiled = false;
  bool prefix_match = false;
};

//
// filepath.cc
//

std::filesystem::path filepath(const auto &path) {
  return {path, std::filesystem::path::format::generic_format};
}

std::string get_realpath(std::string_view path);
std::string path_clean(std::string_view path);
std::filesystem::path to_abs_path(std::filesystem::path path);

//
// demangle.cc
//

std::optional<std::string_view> demangle_cpp(std::string_view name);
std::optional<std::string_view> demangle_rust(std::string_view name);

//
// jbos.cc
//

void acquire_global_lock();
void release_global_lock();

//
// crc32.cc
//

u32 compute_crc32(u32 crc, u8 *buf, i64 len);
std::vector<u8> crc32_solve(u32 current, u32 desired);

//
// compress.cc
//

class Compressor {
public:
  virtual void write_to(u8 *buf) = 0;
  virtual ~Compressor() {}
  i64 compressed_size = 0;
};

class ZlibCompressor : public Compressor {
public:
  ZlibCompressor(u8 *buf, i64 size);
  void write_to(u8 *buf) override;

private:
  std::vector<std::vector<u8>> shards;
  u64 checksum = 0;
};

class ZstdCompressor : public Compressor {
public:
  ZstdCompressor(u8 *buf, i64 size);
  void write_to(u8 *buf) override;

private:
  std::vector<std::vector<u8>> shards;
};

//
// tar.cc
//

// TarFile is a class to create a tar file.
//
// If you pass `--repro` to mold, mold collects all input files and
// put them into `<output-file-path>.repro.tar`, so that it is easy to
// run the same command with the same command line arguments.
class TarWriter {
public:
  static std::unique_ptr<TarWriter>
  open(std::string output_path, std::string basedir);

  ~TarWriter();
  void append(std::string path, std::string_view data);

private:
  TarWriter(FILE *out, std::string basedir) : out(out), basedir(basedir) {}

  FILE *out = nullptr;
  std::string basedir;
};

//
// Memory-mapped file
//

// MappedFile represents an mmap'ed input file.
// mold uses mmap-IO only.
class MappedFile {
public:
  ~MappedFile() { unmap(); }
  void unmap();
  void close_fd();
  void reopen_fd(const std::string &path);

  template <typename Context>
  MappedFile *slice(Context &ctx, std::string name, u64 start, u64 size) {
    MappedFile *mf = new MappedFile;
    mf->name = name;
    mf->data = data + start;
    mf->size = size;
    mf->parent = this;

    ctx.mf_pool.push_back(std::unique_ptr<MappedFile>(mf));
    return mf;
  }

  std::string_view get_contents() {
    return std::string_view((char *)data, size);
  }

  i64 get_offset() const {
    return parent ? (data - parent->data + parent->get_offset()) : 0;
  }

  // Returns a string that uniquely identify a file that is possibly
  // in an archive.
  std::string get_identifier() const {
    if (parent) {
      // We use the file offset within an archive as an identifier
      // because archive members may have the same name.
      return parent->name + ":" + std::to_string(get_offset());
    }

    if (thin_parent) {
      // If this is a thin archive member, the filename part is
      // guaranteed to be unique.
      return thin_parent->name + ":" + name;
    }
    return name;
  }

  std::string name;
  u8 *data = nullptr;
  i64 size = 0;
  bool given_fullpath = true;
  MappedFile *parent = nullptr;
  MappedFile *thin_parent = nullptr;

  // For --dependency-file
  bool is_dependency = true;

#ifdef _WIN32
  HANDLE fd = INVALID_HANDLE_VALUE;
#else
  int fd = -1;
#endif
};

MappedFile *open_file_impl(const std::string &path, std::string &error);

template <typename Context>
MappedFile *open_file(Context &ctx, std::string path) {
  if (path.starts_with('/') && !ctx.arg.chroot.empty())
    path = ctx.arg.chroot + "/" + path_clean(path);

  std::string error;
  MappedFile *mf = open_file_impl(path, error);
  if (!error.empty())
    Fatal(ctx) << error;

  if (mf)
    ctx.mf_pool.push_back(std::unique_ptr<MappedFile>(mf));
  return mf;
}

template <typename Context>
MappedFile *must_open_file(Context &ctx, std::string path) {
  MappedFile *mf = open_file(ctx, path);
  if (!mf)
    Fatal(ctx) << "cannot open " << path << ": " << errno_string();
  return mf;
}

} // namespace mold
    branches: [ "main" ]
  pull_request: // This file implements a multi-threaded zlib and zstd compression
// routine.
//
// zlib-compressed data can be merged just by concatenation as long as
// each piece of data is flushed with Z_SYNC_FLUSH. In this file, we
// split input data into multiple shards, compress them individually
// and concatenate them. We then append a header, a trailer and a
// checksum so that the concatenated data is valid zlib-format data.
//
// zstd-compressed data can be merged in the same way.
//
// Using threads to compress data has a downside. Since the dictionary
// is reset on boundaries of shards, compression ratio is sacrificed
// a little bit. However, if a shard size is large enough, that loss
// is negligible in practice.

#include "common.h"

#include <tbb/parallel_for_each.h>
#include <zlib.h>
#include <zstd.h>

#define CHECK(fn)                               \
  do {                                          \
    [[maybe_unused]] int r = (fn);              \
    assert(r == Z_OK);                          \
  } while (0)

namespace mold {

static constexpr i64 SHARD_SIZE = 1024 * 1024;

static std::vector<std::string_view> split(std::string_view input) {
  std::vector<std::string_view> shards;

  while (input.size() >= SHARD_SIZE) {
    shards.push_back(input.substr(0, SHARD_SIZE));
    input = input.substr(SHARD_SIZE);
  }
  if (!input.empty())
    shards.push_back(input);
  return shards;
}

static std::vector<u8> zlib_compress(std::string_view input) {
  // Initialize zlib stream. Since debug info is generally compressed
  // pretty well with lower compression levels, we chose compression
  // level 1.
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;

  CHECK(deflateInit2(&strm, 1, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY));

  // Set an input buffer
  strm.avail_in = input.size();
  strm.next_in = (u8 *)input.data();

  // Set an output buffer. deflateBound() returns an upper bound
  // on the compression size. +16 for Z_SYNC_FLUSH.
  std::vector<u8> buf(deflateBound(&strm, strm.avail_in) + 16);

  // Compress data. It writes all compressed bytes except the last
  // partial byte, so up to 7 bits can be held to be written to the
  // buffer.
  strm.avail_out = buf.size();
  strm.next_out = buf.data();
  CHECK(deflate(&strm, Z_BLOCK));

  // This is a workaround for libbacktrace before 2022-04-06.
  //
  // Zlib is a bit stream, and what Z_SYNC_FLUSH does is to write a
  // three bit value indicating the start of an uncompressed data
  // block followed by four byte data 00 00 ff ff which indicates that
  // the length of the block is zero. libbacktrace uses its own zlib
  // inflate routine, and it had a bug that if that particular three
  // bit value happens to end at a byte boundary, it accidentally
  // skipped the next byte.
  //
  // In order to avoid triggering that bug, we should avoid calling
  // deflate() with Z_SYNC_FLUSH if the current bit position is 5.
  // If it's 5, we insert an empty block consisting of 10 bits so
  // that the bit position is 7 in the next byte.
  //
  // https://github.com/ianlancetaylor/libbacktrace/pull/87
  int nbits;
  deflatePending(&strm, Z_NULL, &nbits);
  if (nbits == 5)
    CHECK(deflatePrime(&strm, 10, 2));
  CHECK(deflate(&strm, Z_SYNC_FLUSH));

  assert(strm.avail_out > 0);
  buf.resize(buf.size() - strm.avail_out);
  buf.shrink_to_fit();
  deflateEnd(&strm);
  return buf;
}

ZlibCompressor::ZlibCompressor(u8 *buf, i64 size) {
  std::string_view input{(char *)buf, (size_t)size};
  std::vector<std::string_view> inputs = split(input);
  std::vector<u64> adlers(inputs.size());
  shards.resize(inputs.size());

  // Compress each shard
  tbb::parallel_for((i64)0, (i64)inputs.size(), [&](i64 i) {
    adlers[i] = adler32(1, (u8 *)inputs[i].data(), inputs[i].size());
    shards[i] = zlib_compress(inputs[i]);
  });

  // Combine checksums
  checksum = adlers[0];
  for (i64 i = 1; i < inputs.size(); i++)
    checksum = adler32_combine(checksum, adlers[i], inputs[i].size());

  // Comput the total size
  compressed_size = 8; // the header and the trailer
  for (std::vector<u8> &shard : shards)
    compressed_size += shard.size();
}

void ZlibCompressor::write_to(u8 *buf) {
  // Write a zlib-format header
  buf[0] = 0x78;
  buf[1] = 0x9c;

  // Copy compressed data
  std::vector<i64> offsets(shards.size());
  offsets[0] = 2; // +2 for header
  for (i64 i = 1; i < shards.size(); i++)
    offsets[i] = offsets[i - 1] + shards[i - 1].size();

  tbb::parallel_for((i64)0, (i64)shards.size(), [&](i64 i) {
    memcpy(buf + offsets[i], shards[i].data(), shards[i].size());
  });

  // Write a trailer
  u8 *end = buf + compressed_size;
  end[-6] = 3;
  end[-5] = 0;

  // Write a checksum
  *(ub32 *)(end - 4) = checksum;
}

static std::vector<u8> zstd_compress(std::string_view input) {
  std::vector<u8> buf(ZSTD_COMPRESSBOUND(input.size()));
  constexpr int LEVEL = 3; // compression level; must be between 1 to 22

  size_t sz = ZSTD_compress(buf.data(), buf.size(), input.data(), input.size(),
                            LEVEL);
  assert(!ZSTD_isError(sz));
  buf.resize(sz);
  buf.shrink_to_fit();
  return buf;
}

ZstdCompressor::ZstdCompressor(u8 *buf, i64 size) {
  std::string_view input{(char *)buf, (size_t)size};
  std::vector<std::string_view> inputs = split(input);
  shards.resize(inputs.size());

  // Compress each shard
  tbb::parallel_for((i64)0, (i64)inputs.size(), [&](i64 i) {
    shards[i] = zstd_compress(inputs[i]);
  });

  compressed_size = 0;
  for (std::vector<u8> &shard : shards)
    compressed_size += shard.size();
}

void ZstdCompressor::write_to(u8 *buf) {
  // Copy compressed data
  std::vector<i64> offsets(shards.size());
  for (i64 i = 1; i < shards.size(); i++)
    offsets[i] = offsets[i - 1] + shards[i - 1].size();

  tbb::parallel_for((i64)0, (i64)shards.size(), [&](i64 i) {
    memcpy(buf + offsets[i], shards[i].data(), shards[i].size());
  });
}

} // namespace mold
    branches: [ "main" ]
  schedule: #define MOLD_VERSION "@mold_VERSION@"
#define MOLD_LIBDIR "@CMAKE_INSTALL_FULL_LIBDIR@"

#cmakedefine HAVE_MADVISE 1
    - cron: #include "common.h"

#include <tbb/parallel_for_each.h>
#include <zlib.h>

namespace mold {

// This function "forges" a CRC. That is, given the current and a desired
// CRC32 value, crc32_solve() returns a binary blob to add to the end of
// the original data to yield the desired CRC. Trailing garbage is ignored
// by many bianry file formats, so you can create a file with a desired
// CRC using crc32_solve(). We need it for --separate-debug-file.
std::vector<u8> crc32_solve(u32 current, u32 desired) {
  constexpr u32 poly = 0xedb88320;
  u32 x = ~desired;

  // Each iteration computes x = (x * x^-1) mod poly.
  for (i64 i = 0; i < 32; i++) {
    x = std::rotl(x, 1);
    x ^= (x & 1) * (poly << 1);
  }

  x ^= ~current;

  std::vector<u8> out(4);
  out[0] = x;
  out[1] = x >> 8;
  out[2] = x >> 16;
  out[3] = x >> 24;
  return out;
}

// Compute a CRC for given data in parallel
u32 compute_crc32(u32 crc, u8 *buf, i64 len) {
  struct Shard {
    u8 *buf;
    i64 len;
    u32 crc;
  };

  constexpr i64 shard_size = 1024 * 1024; // 1 MiB
  std::vector<Shard> shards;

  while (len > 0) {
    i64 sz = std::min(len, shard_size);
    shards.push_back({buf, sz, 0});
    buf += sz;
    len -= sz;
  }

  tbb::parallel_for_each(shards.begin(), shards.end(), [](Shard &shard) {
    shard.crc = crc32_z(0, shard.buf, shard.len);
  });

  for (Shard &shard : shards)
    crc = crc32_combine(crc, shard.crc, shard.len);
  return crc;
}

} // namespace mold

env: #include "common.h"

#include <cstdlib>

#ifndef _WIN32
#include <cxxabi.h>
#endif

#include "../third-party/rust-demangle/rust-demangle.h"

namespace mold {

std::optional<std::string_view> demangle_cpp(std::string_view name) {
  static thread_local char *buf;
  static thread_local size_t buflen;

  // TODO(cwasser): Actually demangle Symbols on Windows using e.g.
  // `UnDecorateSymbolName` from Dbghelp, maybe even Itanium symbols?
#ifndef _WIN32
  if (name.starts_with("_Z")) {
    int status;
    char *p = abi::__cxa_demangle(std::string(name).c_str(), buf, &buflen, &status);
    if (status == 0) {
      buf = p;
      return p;
    }
  }
#endif

  return {};
}

std::optional<std::string_view> demangle_rust(std::string_view name) {
  static thread_local char *buf;
  free(buf);
  buf = rust_demangle(std::string(name).c_str(), 0);
  if (buf)
    return buf;
  return {};
}

} // namespace mold
  # Path to the CMake build directory.
  build: '${{ #include "common.h"

#include <filesystem>
#include <sys/stat.h>

#ifdef __APPLE__
# include <mach-o/dyld.h>
#endif

#ifdef __FreeBSD__
# include <sys/sysctl.h>
#endif

namespace mold {

std::string get_realpath(std::string_view path) {
  std::error_code ec;
  std::filesystem::path link = std::filesystem::read_symlink(path, ec);
  if (ec)
    return std::string(path);
  return (filepath(path) / ".." / link).lexically_normal().string();
}

// Removes redundant '/..' or '/.' from a given path.
// The transformation is done purely by lexical processing.
// This function does not access file system.
std::string path_clean(std::string_view path) {
  return filepath(path).lexically_normal().string();
}

std::filesystem::path to_abs_path(std::filesystem::path path) {
  if (path.is_absolute())
    return path.lexically_normal();
  return (std::filesystem::current_path() / path).lexically_normal();
}

// Returns the path of the mold executable itself
std::string get_self_path() {
#if __APPLE__ || _WIN32
  fprintf(stderr, "mold: get_self_path is not supported");
  exit(1);
#elif __FreeBSD__
  // /proc may not be mounted on FreeBSD. The proper way to get the
  // current executable's path is to use sysctl(2).
  int mib[4];
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PATHNAME;
  mib[3] = -1;

  size_t size;
  sysctl(mib, 4, NULL, &size, NULL, 0);

  std::string path;
  path.resize(size);
  sysctl(mib, 4, path.data(), &size, NULL, 0);
  return path;
#else
  return std::filesystem::read_symlink("/proc/self/exe").string();
#endif
}

} // namespace mold }}/build'

permissions: #pragma once

#include "common.h"
#include "../elf/elf.h"

namespace mold {

enum class FileType {
  UNKNOWN,
  EMPTY,
  ELF_OBJ,
  ELF_DSO,
  MACH_OBJ,
  MACH_EXE,
  MACH_DYLIB,
  MACH_BUNDLE,
  MACH_UNIVERSAL,
  AR,
  THIN_AR,
  TAPI,
  TEXT,
  GCC_LTO_OBJ,
  LLVM_BITCODE,
};

template <typename MappedFile>
bool is_text_file(MappedFile *mf) {
  auto istext = [](char c) {
    return isprint(c) || c == '\n' || c == '\t';
  };

  u8 *data = mf->data;
  return mf->size >= 4 && istext(data[0]) && istext(data[1]) &&
         istext(data[2]) && istext(data[3]);
}

template <typename E, typename Context, typename MappedFile>
inline bool is_gcc_lto_obj(Context &ctx, MappedFile *mf) {
  using namespace mold::elf;

  const char *data = mf->get_contents().data();
  ElfEhdr<E> &ehdr = *(ElfEhdr<E> *)data;
  ElfShdr<E> *sh_begin = (ElfShdr<E> *)(data + ehdr.e_shoff);
  std::span<ElfShdr<E>> shdrs{(ElfShdr<E> *)(data + ehdr.e_shoff), ehdr.e_shnum};

  // e_shstrndx is a 16-bit field. If .shstrtab's section index is
  // too large, the actual number is stored to sh_link field.
  i64 shstrtab_idx = (ehdr.e_shstrndx == SHN_XINDEX)
    ? sh_begin->sh_link : ehdr.e_shstrndx;

  for (ElfShdr<E> &sec : shdrs) {
    // GCC FAT LTO objects contain both regular ELF sections and GCC-
    // specific LTO sections, so that they can be linked as LTO objects if
    // the LTO linker plugin is available and falls back as regular
    // objects otherwise. GCC FAT LTO object can be identified by the
    // presence of `.gcc.lto_.symtab` section.
    if (!ctx.arg.plugin.empty()) {
      std::string_view name = data + shdrs[shstrtab_idx].sh_offset + sec.sh_name;
      if (name.starts_with(".gnu.lto_.symtab."))
        return true;
    }

    if (sec.sh_type != SHT_SYMTAB)
      continue;

    // GCC non-FAT LTO object contains only sections symbols followed by
    // a common symbol whose name is `__gnu_lto_slim` (or `__gnu_lto_v1`
    // for older GCC releases).
    std::span<ElfSym<E>> elf_syms{(ElfSym<E> *)(data + sec.sh_offset),
                                  (size_t)sec.sh_size / sizeof(ElfSym<E>)};

    auto skip = [](u8 type) {
      return type == STT_NOTYPE || type == STT_FILE || type == STT_SECTION;
    };

    i64 i = 1;
    while (i < elf_syms.size() && skip(elf_syms[i].st_type))
      i++;

    if (i < elf_syms.size() && elf_syms[i].st_shndx == SHN_COMMON) {
      std::string_view name =
        data + shdrs[sec.sh_link].sh_offset + elf_syms[i].st_name;
      if (name.starts_with("__gnu_lto_"))
        return true;
    }
    break;
  }

  return false;
}

template <typename Context, typename MappedFile>
FileType get_file_type(Context &ctx, MappedFile *mf) {
  using namespace elf;

  std::string_view data = mf->get_contents();

  if (data.empty())
    return FileType::EMPTY;

  if (data.starts_with("\177ELF")) {
    u8 byte_order = ((ElfEhdr<I386> *)data.data())->e_ident[EI_DATA];

    if (byte_order == ELFDATA2LSB) {
      auto &ehdr = *(ElfEhdr<I386> *)data.data();

      if (ehdr.e_type == ET_REL) {
        if (ehdr.e_ident[EI_CLASS] == ELFCLASS32) {
          if (is_gcc_lto_obj<I386>(ctx, mf))
            return FileType::GCC_LTO_OBJ;
        } else {
          if (is_gcc_lto_obj<X86_64>(ctx, mf))
            return FileType::GCC_LTO_OBJ;
        }
        return FileType::ELF_OBJ;
      }

      if (ehdr.e_type == ET_DYN)
        return FileType::ELF_DSO;
    } else {
      auto &ehdr = *(ElfEhdr<M68K> *)data.data();

      if (ehdr.e_type == ET_REL) {
        if (ehdr.e_ident[EI_CLASS] == ELFCLASS32) {
          if (is_gcc_lto_obj<M68K>(ctx, mf))
            return FileType::GCC_LTO_OBJ;
        } else {
          if (is_gcc_lto_obj<SPARC64>(ctx, mf))
            return FileType::GCC_LTO_OBJ;
        }
        return FileType::ELF_OBJ;
      }

      if (ehdr.e_type == ET_DYN)
        return FileType::ELF_DSO;
    }
    return FileType::UNKNOWN;
  }

  if (data.starts_with("\xcf\xfa\xed\xfe")) {
    switch (*(ul32 *)(data.data() + 12)) {
    case 1: // MH_OBJECT
      return FileType::MACH_OBJ;
    case 2: // MH_EXECUTE
      return FileType::MACH_EXE;
    case 6: // MH_DYLIB
      return FileType::MACH_DYLIB;
    case 8: // MH_BUNDLE
      return FileType::MACH_BUNDLE;
    }
    return FileType::UNKNOWN;
  }

  if (data.starts_with("!<arch>\n"))
    return FileType::AR;
  if (data.starts_with("!<thin>\n"))
    return FileType::THIN_AR;
  if (data.starts_with("--- !tapi-tbd"))
    return FileType::TAPI;
  if (data.starts_with("\xca\xfe\xba\xbe"))
    return FileType::MACH_UNIVERSAL;
  if (is_text_file(mf))
    return FileType::TEXT;
  if (data.starts_with("\xde\xc0\x17\x0b"))
    return FileType::LLVM_BITCODE;
  if (data.starts_with("BC\xc0\xde"))
    return FileType::LLVM_BITCODE;
  return FileType::UNKNOWN;
}

inline std::string filetype_to_string(FileType type) {
  switch (type) {
  case FileType::UNKNOWN: return "UNKNOWN";
  case FileType::EMPTY: return "EMPTY";
  case FileType::ELF_OBJ: return "ELF_OBJ";
  case FileType::ELF_DSO: return "ELF_DSO";
  case FileType::MACH_EXE: return "MACH_EXE";
  case FileType::MACH_OBJ: return "MACH_OBJ";
  case FileType::MACH_DYLIB: return "MACH_DYLIB";
  case FileType::MACH_BUNDLE: return "MACH_BUNDLE";
  case FileType::MACH_UNIVERSAL: return "MACH_UNIVERSAL";
  case FileType::AR: return "AR";
  case FileType::THIN_AR: return "THIN_AR";
  case FileType::TAPI: return "TAPI";
  case FileType::TEXT: return "TEXT";
  case FileType::GCC_LTO_OBJ: return "GCC_LTO_OBJ";
  case FileType::LLVM_BITCODE: return "LLVM_BITCODE";
  }
  return "UNKNOWN";
}

inline std::ostream &operator<<(std::ostream &out, FileType type) {
  out << filetype_to_string(type);
  return out;
}

} // namespace mold
  contents: #include "common.h"

#include <cstring>

namespace mold {

std::optional<Glob> Glob::compile(std::string_view pat) {
  std::vector<Element> vec;

  while (!pat.empty()) {
    u8 c = pat[0];
    pat = pat.substr(1);

    switch (c) {
    case '[': {
      // Here are a few bracket pattern examples:
      //
      // [abc]: a, b or c
      // [$\]!]: $, ] or !
      // [a-czg-i]: a, b, c, z, g, h, or i
      // [^a-z]: Any character except lowercase letters
      vec.push_back({BRACKET});
      std::bitset<256> &bitset = vec.back().bitset;

      bool negate = false;
      if (!pat.empty() && pat[0] == '^') {
        negate = true;
        pat = pat.substr(1);
      }

      bool closed = false;

      while (!pat.empty()) {
        if (pat[0] == ']') {
          pat = pat.substr(1);
          closed = true;
          break;
        }

        if (pat[0] == '\\') {
          pat = pat.substr(1);
          if (pat.empty())
            return {};
        }

        if (pat.size() >= 3 && pat[1] == '-') {
          u8 start = pat[0];
          u8 end = pat[2];
          pat = pat.substr(3);

          if (end == '\\') {
            if (pat.empty())
              return {};
            end = pat[0];
            pat = pat.substr(1);
          }

          if (end < start)
            return {};

          for (i64 i = start; i <= end; i++)
            bitset[i] = true;
        } else {
          bitset[(u8)pat[0]] = true;
          pat = pat.substr(1);
        }
      }

      if (!closed)
        return {};

      if (negate)
        bitset.flip();
      break;
    }
    case '?':
      vec.push_back({QUESTION});
      break;
    case '*':
      vec.push_back({STAR});
      break;
    case '\\':
      if (pat.empty())
        return {};
      if (vec.empty() || vec.back().kind != STRING)
        vec.push_back({STRING});
      vec.back().str += pat[0];
      pat = pat.substr(1);
      break;
    default:
      if (vec.empty() || vec.back().kind != STRING)
        vec.push_back({STRING});
      vec.back().str += c;
      break;
    }
  }

  return {Glob{std::move(vec)}};
}

bool Glob::match(std::string_view str) {
  return do_match(str, elements);
}

bool Glob::do_match(std::string_view str, std::span<Element> elements) {
  while (!elements.empty()) {
    Element &e = elements[0];
    elements = elements.subspan(1);

    switch (e.kind) {
    case STRING:
      if (!str.starts_with(e.str))
        return false;
      str = str.substr(e.str.size());
      break;
    case STAR:
      if (elements.empty())
        return true;

      // Patterns like "*foo*bar*" should be much more common than more
      // complex ones like "*foo*[abc]*" or "*foo**?bar*", so we optimize
      // the former case here.
      if (elements[0].kind == STRING) {
        for (;;) {
          size_t pos = str.find(elements[0].str);
          if (pos == str.npos)
            return false;
          if (do_match(str.substr(pos + elements[0].str.size()),
                       elements.subspan(1)))
            return true;
          str = str.substr(pos + 1);
        }
      }

      // Other cases are handled here.
      for (i64 j = 0; j < str.size(); j++)
        if (do_match(str.substr(j), elements))
          return true;
      return false;
    case QUESTION:
      if (str.empty())
        return false;
      str = str.substr(1);
      break;
    case BRACKET:
      if (str.empty() || !e.bitset[str[0]])
        return false;
      str = str.substr(1);
      break;
    }
  }

  return str.empty();
}

} // namespace mold

jobs: // This file implements HyperLogLog algorithm, which estimates
// the number of unique items in a given multiset.
//
// For more info, read
// https://engineering.fb.com/2018/12/13/data-infrastructure/hyperloglog

#include "common.h"

#include <cmath>

namespace mold {

i64 HyperLogLog::get_cardinality() const {
  double z = 0;
  for (i64 val : buckets)
    z += std::ldexp(1.0, -val);
  return ALPHA * NBUCKETS * NBUCKETS / z;
}

} // namespace mold
  analyze: // This file defines integral types for file input/output. We need to use
// these types instead of the plain integers (such as uint32_t or int32_t)
// when reading from/writing to an mmap'ed file area for the following
// reasons:
//
// 1. mold is always a cross linker and should not depend on what host it
//    is running on. Users should be able to run mold on a big-endian
//    SPARC machine to create a little-endian RV64 binary, for example.
//
// 2. Even though data members in all ELF data strucutres are naturally
//    aligned, they are not guaranteed to be aligned on memory. Because
//    archive file (.a file) aligns each member only to a 2 byte boundary,
//    anything larger than 2 bytes may be unaligned in an mmap'ed memory.
//    Unaligned access is an undefined behavior in C/C++, so we shouldn't
//    cast an arbitrary pointer to a uint32_t, for example, to read a
//    32-bits value.
//
// The data types defined in this file don't depend on host byte order and
// don't do unaligned access.

#pragma once

#include <bit>
#include <cstdint>
#include <cstring>

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define __LITTLE_ENDIAN__ 1
# elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define __BIG_ENDIAN__ 1
# else
#  error "unknown host byte order"
# endif
#endif

namespace mold {

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

template <typename T>
static inline T bswap(T val) {
  switch (sizeof(T)) {
  case 2:  return __builtin_bswap16(val);
  case 4:  return __builtin_bswap32(val);
  case 8:  return __builtin_bswap64(val);
  default: __builtin_unreachable();
  }
}

template <typename T, int SIZE = sizeof(T)>
class LittleEndian {
public:
  LittleEndian() = default;
  LittleEndian(T x) { *this = x; }

  operator T() const {
    if constexpr (sizeof(T) == SIZE) {
      T x;
      memcpy(&x, val, sizeof(T));
      if constexpr (std::endian::native == std::endian::big)
        x = bswap(x);
      return x;
    } else {
      static_assert(SIZE == 3);
      return (val[2] << 16) | (val[1] << 8) | val[0];
    }
  }

  LittleEndian &operator=(T x) {
    if constexpr (sizeof(T) == SIZE) {
      if constexpr (std::endian::native == std::endian::big)
        x = bswap(x);
      memcpy(val, &x, sizeof(T));
    } else {
      static_assert(SIZE == 3);
      val[2] = x >> 16;
      val[1] = x >> 8;
      val[0] = x;
    }
    return *this;
  }

  LittleEndian &operator++() {
    return *this = *this + 1;
  }

  LittleEndian operator++(int) {
    T ret = *this;
    *this = *this + 1;
    return ret;
  }

  LittleEndian &operator--() {
    return *this = *this - 1;
  }

  LittleEndian operator--(int) {
    T ret = *this;
    *this = *this - 1;
    return ret;
  }

  LittleEndian &operator+=(T x) {
    return *this = *this + x;
  }

  LittleEndian &operator-=(T x) {
    return *this = *this - x;
  }

  LittleEndian &operator&=(T x) {
    return *this = *this & x;
  }

  LittleEndian &operator|=(T x) {
    return *this = *this | x;
  }

private:
  u8 val[SIZE];
};

using il16 = LittleEndian<i16>;
using il32 = LittleEndian<i32>;
using il64 = LittleEndian<i64>;
using ul16 = LittleEndian<u16>;
using ul24 = LittleEndian<u32, 3>;
using ul32 = LittleEndian<u32>;
using ul64 = LittleEndian<u64>;

template <typename T, int SIZE = sizeof(T)>
class BigEndian {
public:
  BigEndian() = default;
  BigEndian(T x) { *this = x; }

  operator T() const {
    if constexpr (sizeof(T) == SIZE) {
      T x;
      memcpy(&x, val, sizeof(T));
      if constexpr (std::endian::native == std::endian::little)
        x = bswap(x);
      return x;
    } else {
      static_assert(SIZE == 3);
      return (val[0] << 16) | (val[1] << 8) | val[2];
    }
  }

  BigEndian &operator=(T x) {
    if constexpr (sizeof(T) == SIZE) {
      if constexpr (std::endian::native == std::endian::little)
        x = bswap(x);
      memcpy(val, &x, sizeof(T));
    } else {
      static_assert(SIZE == 3);
      val[0] = x >> 16;
      val[1] = x >> 8;
      val[2] = x;
    }
    return *this;
  }

  BigEndian &operator++() {
    return *this = *this + 1;
  }

  BigEndian operator++(int) {
    T ret = *this;
    *this = *this + 1;
    return ret;
  }

  BigEndian &operator--() {
    return *this = *this - 1;
  }

  BigEndian operator--(int) {
    T ret = *this;
    *this = *this - 1;
    return ret;
  }

  BigEndian &operator+=(T x) {
    return *this = *this + x;
  }

  BigEndian &operator-=(T x) {
    return *this = *this - x;
  }

  BigEndian &operator&=(T x) {
    return *this = *this & x;
  }

  BigEndian &operator|=(T x) {
    return *this = *this | x;
  }

private:
  u8 val[SIZE];
};

using ib16 = BigEndian<i16>;
using ib32 = BigEndian<i32>;
using ib64 = BigEndian<i64>;
using ub16 = BigEndian<u16>;
using ub24 = BigEndian<u32, 3>;
using ub32 = BigEndian<u32>;
using ub64 = BigEndian<u64>;

} // namespace mold
    permissions: // Many build systems attempt to invoke as many linker processes as there
// are cores, based on the assumption that the linker is single-threaded.
// However, since mold is multi-threaded, such build systems' behavior is
// not beneficial and just increases the overall peak memory usage.
// On machines with limited memory, this could lead to an out-of-memory
// error.
//
// This file implements a feature that limits the number of concurrent
// mold processes to just 1 for each user. It is intended to be used as
// `MOLD_JOBS=1 ninja` or `MOLD_JOBS=1 make -j$(nproc)`.

#include "common.h"

#include <fcntl.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace mold {

static int lock_fd = -1;

void acquire_global_lock() {
  char *jobs = getenv("MOLD_JOBS");
  if (!jobs || jobs != "1"s)
    return;

  std::string path;
  if (char *dir = getenv("XDG_RUNTIME_DIR"))
    path = dir + "/mold-lock"s;
  else
    path = "/tmp/mold-lock-"s + getpwuid(getuid())->pw_name;

  int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_CLOEXEC, 0600);
  if (fd == -1)
    return;

  if (lockf(fd, F_LOCK, 0) == -1)
    return;
  lock_fd = fd;
}

void release_global_lock() {
  if (lock_fd != -1)
    close(lock_fd);
}

} // namespace mold
      contents: namespace mold {

void acquire_global_lock() {}
void release_global_lock() {}

} // namespace mold
      security-events: #include "config.h"

#ifdef USE_SYSTEM_MIMALLOC
# include <mimalloc-new-delete.h>
#endif
      actions: #include "common.h"

namespace mold {

MappedFile *open_file_impl(const std::string &path, std::string &error) {
  i64 fd = ::open(path.c_str(), O_RDONLY);
  if (fd == -1) {
    if (errno != ENOENT)
      error = "opening " + path + " failed: " + errno_string();
    return nullptr;
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
    error = path + ": fstat failed: " + errno_string();

  MappedFile *mf = new MappedFile;
  mf->name = path;
  mf->size = st.st_size;

  if (st.st_size > 0) {
    mf->data = (u8 *)mmap(nullptr, st.st_size, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE, fd, 0);
    if (mf->data == MAP_FAILED)
      error = path + ": mmap failed: " + errno_string();
  }

  close(fd);
  return mf;
}

void MappedFile::unmap() {
  if (size == 0 || parent || !data)
    return;
  munmap(data, size);
  data = nullptr;
}

void MappedFile::close_fd() {
  if (fd == -1)
    return;
  close(fd);
  fd = -1;
}

void MappedFile::reopen_fd(const std::string &path) {
  if (fd == -1)
    fd = open(path.c_str(), O_RDONLY);
}

} // namespace mold
    name: #include "common.h"

namespace mold {

MappedFile *open_file_impl(const std::string &path, std::string &error) {
  HANDLE fd = CreateFileA(path.c_str(), GENERIC_READ,
                          FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                          nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

  if (fd == INVALID_HANDLE_VALUE) {
    auto err = GetLastError();
    if (err != ERROR_FILE_NOT_FOUND)
      error = "opening " + path + " failed: " + errno_string();
    return nullptr;
  }

  if (GetFileType(fd) != FILE_TYPE_DISK) {
    CloseHandle(fd);
    return nullptr;
  }

  DWORD size_hi;
  DWORD size_lo = GetFileSize(fd, &size_hi);

  if (size_lo == INVALID_FILE_SIZE) {
    error = path + ": GetFileSize failed: " + errno_string();
    return nullptr;
  }

  u64 size = ((u64)size_hi << 32) + size_lo;

  MappedFile *mf = new MappedFile;
  mf->name = path;
  mf->size = size;
  mf->fd = fd;

  if (size > 0) {
    HANDLE h = CreateFileMapping(fd, nullptr, PAGE_READONLY, 0, size, nullptr);
    if (!h) {
      error = path + ": CreateFileMapping failed: " + errno_string();
      return nullptr;
    }

    mf->data = (u8 *)MapViewOfFile(h, FILE_MAP_COPY, 0, 0, size);
    CloseHandle(h);

    if (!mf->data) {
      error = path + ": MapViewOfFile failed: " + errno_string();
      return nullptr;
    }
  }

  return mf;
}

void MappedFile::unmap() {
  if (size == 0 || parent || !data)
    return;

  UnmapViewOfFile(data);
  if (fd != INVALID_HANDLE_VALUE)
    CloseHandle(fd);

  data = nullptr;
}

void MappedFile::close_fd() {
  if (fd == INVALID_HANDLE_VALUE)
    return;
  CloseHandle(fd);
  fd = INVALID_HANDLE_VALUE;
}

void MappedFile::reopen_fd(const std::string &path) {
  if (fd == INVALID_HANDLE_VALUE)
    fd = CreateFileA(path.c_str(), GENERIC_READ,
                     FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                     nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
}

} // namespace mold
    runs-on: // This file implements the Aho-Corasick algorithm to match multiple
// glob patterns to symbol strings as quickly as possible.
//
// Here are some examples of glob patterns:
//
//    qt_private_api_tag*
//    *16QAccessibleCache*
//    *32QAbstractFileIconProviderPrivate*
//    *17QPixmapIconEngine*
//
// `*` is a wildcard that matches any substring. We sometimes have
// hundreds of glob patterns and have to match them against millions
// of symbol strings.
//
// Aho-Corasick cannot handle complex patterns such as `*foo*bar*`.
// We handle such patterns with the Glob class. Glob is relatively
// slow, but complex patterns are rare in practice, so it should be
// OK.

#include "common.h"

#include <queue>
#include <regex>

namespace mold {

std::optional<i64> MultiGlob::find(std::string_view str) {
  std::call_once(once, [&] { compile(); });
  i64 val = -1;

  // Match against simple glob patterns
  if (root)
    val = find_aho_corasick(str);

  // Match against complex glob patterns
  for (std::pair<Glob, i64> &glob : globs)
    if (glob.first.match(str))
      val = std::max(val, glob.second);

  if (val == -1)
    return {};
  return val;
}

i64 MultiGlob::find_aho_corasick(std::string_view str) {
  TrieNode *node = root.get();
  i64 val = -1;

  auto walk = [&](u8 c) {
    for (;;) {
      if (node->children[c]) {
        node = node->children[c].get();
        val = std::max(val, node->value);
        return;
      }

      if (!node->suffix_link)
        return;
      node = node->suffix_link;
    }
  };

  walk('\0');

  for (u8 c : str) {
    if (prefix_match && node == root.get())
      return val;
    walk(c);
  }

  walk('\0');
  return val;
}

static bool is_simple_pattern(std::string_view pat) {
  static std::regex re(R"(\*?[^*[?]+\*?)", std::regex_constants::optimize);
  return std::regex_match(pat.begin(), pat.end(), re);
}

static std::string handle_stars(std::string_view pat) {
  std::string str(pat);

  // Convert "foo" -> "\0foo\0", "*foo" -> "foo\0", "foo*" -> "\0foo"
  // and "*foo*" -> "foo". Aho-Corasick can do only substring matching,
  // so we use \0 as beginning/end-of-string markers.
  if (str.starts_with('*') && str.ends_with('*'))
    return str.substr(1, str.size() - 2);
  if (str.starts_with('*'))
    return str.substr(1) + "\0"s;
  if (str.ends_with('*'))
    return "\0"s + str.substr(0, str.size() - 1);
  return "\0"s + str + "\0"s;
}

bool MultiGlob::add(std::string_view pat, i64 val) {
  assert(!is_compiled);
  assert(!pat.empty());

  strings.push_back(std::string(pat));

  // Complex glob pattern
  if (!is_simple_pattern(pat)) {
    if (std::optional<Glob> glob = Glob::compile(pat)) {
      globs.push_back({std::move(*glob), val});
      return true;
    }
    return false;
  }

  // Simple glob pattern
  if (!root)
    root.reset(new TrieNode);
  TrieNode *node = root.get();

  for (u8 c : handle_stars(pat)) {
    if (!node->children[c])
      node->children[c].reset(new TrieNode);
    node = node->children[c].get();
  }

  node->value = std::max(node->value, val);
  return true;
}

void MultiGlob::compile() {
  is_compiled = true;
  if (root) {
    fix_suffix_links(*root);
    fix_values();

    // If no pattern starts with '*', set prefix_match to true.
    // We'll use this flag for optimization.
    prefix_match = true;
    for (i64 i = 1; i < 256; i++) {
      if (root->children[i]) {
        prefix_match = false;
        break;
      }
    }
  }
}

void MultiGlob::fix_suffix_links(TrieNode &node) {
  for (i64 i = 0; i < 256; i++) {
    if (!node.children[i])
      continue;

    TrieNode &child = *node.children[i];

    TrieNode *cur = node.suffix_link;
    for (;;) {
      if (!cur) {
        child.suffix_link = root.get();
        break;
      }

      if (cur->children[i]) {
        child.suffix_link = cur->children[i].get();
        break;
      }

      cur = cur->suffix_link;
    }

    fix_suffix_links(child);
  }
}

void MultiGlob::fix_values() {
  std::queue<TrieNode *> queue;
  queue.push(root.get());

  do {
    TrieNode *node = queue.front();
    queue.pop();

    for (std::unique_ptr<TrieNode> &child : node->children) {
      if (!child)
        continue;
      child->value = std::max(child->value, child->suffix_link->value);
      queue.push(child.get());
    }
  } while (!queue.empty());
}

} // namespace mold

    steps: #include "common.h"

#include <fcntl.h>
#include <filesystem>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace mold {

inline u32 get_umask() {
  u32 orig_umask = umask(0);
  umask(orig_umask);
  return orig_umask;
}

template <typename Context>
static int
open_or_create_file(Context &ctx, std::string path, std::string tmpfile,
                    int perm) {
  // Reuse an existing file if exists and writable because on Linux,
  // writing to an existing file is much faster than creating a fresh
  // file and writing to it.
  if (ctx.overwrite_output_file && rename(path.c_str(), tmpfile.c_str()) == 0) {
    i64 fd = ::open(tmpfile.c_str(), O_RDWR | O_CREAT, perm);
    if (fd != -1)
      return fd;
    unlink(tmpfile.c_str());
  }

  i64 fd = ::open(tmpfile.c_str(), O_RDWR | O_CREAT, perm);
  if (fd == -1)
    Fatal(ctx) << "cannot open " << tmpfile << ": " << errno_string();
  return fd;
}

template <typename Context>
class MemoryMappedOutputFile : public OutputFile<Context> {
public:
  MemoryMappedOutputFile(Context &ctx, std::string path, i64 filesize, int perm)
    : OutputFile<Context>(path, filesize, true) {
    std::filesystem::path dir = filepath(path).parent_path();
    std::string filename = filepath(path).filename().string();
    std::string tmpfile = dir / ("." + filename + "." + std::to_string(getpid()));

    this->fd = open_or_create_file(ctx, path, tmpfile, perm);

    if (fchmod(this->fd, perm & ~get_umask()) == -1)
      Fatal(ctx) << "fchmod failed: " << errno_string();

    if (ftruncate(this->fd, filesize) == -1)
      Fatal(ctx) << "ftruncate failed: " << errno_string();

    output_tmpfile = (char *)save_string(ctx, tmpfile).data();

#ifdef __linux__
    fallocate(this->fd, 0, 0, filesize);
#endif

    this->buf = (u8 *)mmap(nullptr, filesize, PROT_READ | PROT_WRITE,
                           MAP_SHARED, this->fd, 0);
    if (this->buf == MAP_FAILED)
      Fatal(ctx) << path << ": mmap failed: " << errno_string();

    mold::output_buffer_start = this->buf;
    mold::output_buffer_end = this->buf + filesize;
  }

  ~MemoryMappedOutputFile() {
    if (fd2 != -1)
      ::close(fd2);
  }

  void close(Context &ctx) override {
    Timer t(ctx, "close_file");

    if (!this->is_unmapped)
      munmap(this->buf, this->filesize);

    if (this->buf2.empty()) {
      ::close(this->fd);
    } else {
      FILE *out = fdopen(this->fd, "w");
      fseek(out, 0, SEEK_END);
      fwrite(&this->buf2[0], this->buf2.size(), 1, out);
      fclose(out);
    }

    // If an output file already exists, open a file and then remove it.
    // This is the fastest way to unlink a file, as it does not make the
    // system to immediately release disk blocks occupied by the file.
    fd2 = ::open(this->path.c_str(), O_RDONLY);
    if (fd2 != -1)
      unlink(this->path.c_str());

    if (rename(output_tmpfile, this->path.c_str()) == -1)
      Fatal(ctx) << this->path << ": rename failed: " << errno_string();
    output_tmpfile = nullptr;
  }

private:
  int fd2 = -1;
};

template <typename Context>
std::unique_ptr<OutputFile<Context>>
OutputFile<Context>::open(Context &ctx, std::string path, i64 filesize, int perm) {
  Timer t(ctx, "open_file");

  if (path.starts_with('/') && !ctx.arg.chroot.empty())
    path = ctx.arg.chroot + "/" + path_clean(path);

  bool is_special = false;
  if (path == "-") {
    is_special = true;
  } else {
    struct stat st;
    if (stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFMT) != S_IFREG)
      is_special = true;
  }

  OutputFile<Context> *file;
  if (is_special)
    file = new MallocOutputFile(ctx, path, filesize, perm);
  else
    file = new MemoryMappedOutputFile(ctx, path, filesize, perm);

#ifdef MADV_HUGEPAGE
  // Enable transparent huge page for an output memory-mapped file.
  // On Linux, it has an effect only on tmpfs mounted with `huge=advise`,
  // but it can make the linker ~10% faster. You can try it by creating
  // a tmpfs with the following commands
  //
  //  $ mkdir tmp
  //  $ sudo mount -t tmpfs -o size=2G,huge=advise none tmp
  //
  // and then specifying a path under the directory as an output file.
  madvise(file->buf, filesize, MADV_HUGEPAGE);
#endif

  if (ctx.arg.filler != -1)
    memset(file->buf, ctx.arg.filler, filesize);
  return std::unique_ptr<OutputFile>(file);
}

// LockingOutputFile is similar to MemoryMappedOutputFile, but it doesn't
// rename output files and instead acquires file lock using flock().
template <typename Context>
LockingOutputFile<Context>::LockingOutputFile(Context &ctx, std::string path,
                                              int perm)
  : OutputFile<Context>(path, 0, true) {
  this->fd = ::open(path.c_str(), O_RDWR | O_CREAT, perm);
  if (this->fd == -1)
    Fatal(ctx) << "cannot open " << path << ": " << errno_string();
  flock(this->fd, LOCK_EX);

  // We may be overwriting to an existing debug info file. We want to
  // make the file unusable so that gdb won't use it by accident until
  // it's ready.
  u8 buf[256] = {};
  (void)!!write(this->fd, buf, sizeof(buf));
}

template <typename Context>
void LockingOutputFile<Context>::resize(Context &ctx, i64 filesize) {
  if (ftruncate(this->fd, filesize) == -1)
    Fatal(ctx) << "ftruncate failed: " << errno_string();

  this->buf = (u8 *)mmap(nullptr, filesize, PROT_READ | PROT_WRITE,
                         MAP_SHARED, this->fd, 0);
  if (this->buf == MAP_FAILED)
    Fatal(ctx) << this->path << ": mmap failed: " << errno_string();

  this->filesize = filesize;
  mold::output_buffer_start = this->buf;
  mold::output_buffer_end = this->buf + filesize;
}

template <typename Context>
void LockingOutputFile<Context>::close(Context &ctx) {
  if (!this->is_unmapped)
    munmap(this->buf, this->filesize);

  if (!this->buf2.empty()) {
    FILE *out = fdopen(this->fd, "w");
    fseek(out, 0, SEEK_END);
    fwrite(&this->buf2[0], this->buf2.size(), 1, out);
    fclose(out);
  }

  ::close(this->fd);
}

} // namespace mold
      - name: #include "common.h"

#include <fcntl.h>
#include <filesystem>
#include <windows.h>

namespace mold {

template <typename Context>
class MemoryMappedOutputFile : public OutputFile<Context> {
public:
  MemoryMappedOutputFile(Context &ctx, std::string path, i64 filesize, int perm)
      : OutputFile<Context>(path, filesize, true) {
    // TODO: use intermediate temporary file for output.
    DWORD attrs = (perm & 0200) ? FILE_ATTRIBUTE_NORMAL : FILE_ATTRIBUTE_READONLY;

    handle = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                         nullptr, CREATE_ALWAYS, attrs, nullptr);
    if (handle == INVALID_HANDLE_VALUE)
      Fatal(ctx) << "cannot open " << path << ": " << GetLastError();

    HANDLE map = CreateFileMapping(handle, nullptr, PAGE_READWRITE, 0,
                                   filesize, nullptr);
    if (!map)
      Fatal(ctx) << path << ": CreateFileMapping failed: " << GetLastError();

    this->buf = (u8 *)MapViewOfFile(map, FILE_MAP_WRITE, 0, 0, filesize);
    if (!this->buf)
      Fatal(ctx) << path << ": MapViewOfFile failed: " << GetLastError();

    CloseHandle(map);

    mold::output_buffer_start = this->buf;
    mold::output_buffer_end = this->buf + filesize;
  }

  ~MemoryMappedOutputFile() {
    if (handle != INVALID_HANDLE_VALUE)
      CloseHandle(handle);
  }

  void close(Context &ctx) override {
    Timer t(ctx, "close_file");

    UnmapViewOfFile(this->buf);

    if (!this->buf2.empty()) {
      if (SetFilePointer(handle, 0, nullptr, FILE_END) == INVALID_SET_FILE_POINTER)
        Fatal(ctx) << this->path << ": SetFilePointer failed: "
                   << GetLastError();

      DWORD written;
      if (!WriteFile(handle, this->buf2.data(), this->buf2.size(), &written,
                     nullptr))
        Fatal(ctx) << this->path << ": WriteFile failed: " << GetLastError();
    }

    CloseHandle(handle);
    handle = INVALID_HANDLE_VALUE;
  }

private:
  HANDLE handle;
};

template <typename Context>
std::unique_ptr<OutputFile<Context>>
OutputFile<Context>::open(Context &ctx, std::string path, i64 filesize, int perm) {
  Timer t(ctx, "open_file");

  if (path.starts_with('/') && !ctx.arg.chroot.empty())
    path = ctx.arg.chroot + "/" + path_clean(path);

  bool is_special = false;
  if (path == "-") {
    is_special = true;
  } else {
    HANDLE h = CreateFileA(path.c_str(), GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
      if (GetFileType(h) != FILE_TYPE_DISK)
        is_special = true;
      CloseHandle(h);
    }
  }

  OutputFile<Context> *file;
  if (is_special)
    file = new MallocOutputFile(ctx, path, filesize, perm);
  else
    file = new MemoryMappedOutputFile(ctx, path, filesize, perm);

  if (ctx.arg.filler != -1)
    memset(file->buf, ctx.arg.filler, filesize);
  return std::unique_ptr<OutputFile<Context>>(file);
}

template <typename Context>
LockingOutputFile<Context>::LockingOutputFile(Context &ctx, std::string path,
                                              int perm)
  : OutputFile<Context>(path, 0, true) {
  Fatal(ctx) << "LockingOutputFile is not supported on Windows";
}

template <typename Context>
void LockingOutputFile<Context>::resize(Context &ctx, i64 filesize) {}

template <typename Context>
void LockingOutputFile<Context>::close(Context &ctx) {}

} // namespace mold
        uses: #if _WIN32
# include "output-file-win32.h"
#else
# include "output-file-unix.h"
#endif

      - name: #include "common.h"

#include <functional>
#include <iomanip>
#include <ios>

#ifndef _WIN32
#include <sys/resource.h>
#include <sys/time.h>
#endif

namespace mold {

i64 Counter::get_value() {
  return values.combine(std::plus());
}

void Counter::print() {
  sort(instances, [](Counter *a, Counter *b) {
    return a->get_value() > b->get_value();
  });

  for (Counter *c : instances)
    std::cout << std::setw(20) << std::right << c->name
              << "=" << c->get_value() << "\n";
}

static i64 now_nsec() {
  return (i64)std::chrono::steady_clock::now().time_since_epoch().count();
}

static std::pair<i64, i64> get_usage() {
#ifdef _WIN32
  auto to_nsec = [](FILETIME t) -> i64 {
    return (((u64)t.dwHighDateTime << 32) + (u64)t.dwLowDateTime) * 100;
  };

  FILETIME creation, exit, kernel, user;
  GetProcessTimes(GetCurrentProcess(), &creation, &exit, &kernel, &user);
  return {to_nsec(user), to_nsec(kernel)};
#else
  auto to_nsec = [](struct timeval t) -> i64 {
    return (i64)t.tv_sec * 1'000'000'000 + t.tv_usec * 1'000;
  };

  struct rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return {to_nsec(ru.ru_utime), to_nsec(ru.ru_stime)};
#endif
}

TimerRecord::TimerRecord(std::string name, TimerRecord *parent)
  : name(name), parent(parent) {
  start = now_nsec();
  std::tie(user, sys) = get_usage();
  if (parent)
    parent->children.push_back(this);
}

void TimerRecord::stop() {
  if (stopped)
    return;
  stopped = true;

  i64 user2;
  i64 sys2;
  std::tie(user2, sys2) = get_usage();

  end = now_nsec();
  user = user2 - user;
  sys = sys2 - sys;
}

static void print_rec(TimerRecord &rec, i64 indent) {
  printf(" % 8.3f % 8.3f % 8.3f  %s%s\n",
         ((double)rec.user / 1'000'000'000),
         ((double)rec.sys / 1'000'000'000),
         (((double)rec.end - rec.start) / 1'000'000'000),
         std::string(indent * 2, ' ').c_str(),
         rec.name.c_str());

  sort(rec.children, [](TimerRecord *a, TimerRecord *b) {
    return a->start < b->start;
  });

  for (TimerRecord *child : rec.children)
    print_rec(*child, indent + 1);
}

void print_timer_records(
    tbb::concurrent_vector<std::unique_ptr<TimerRecord>> &records) {
  for (i64 i = records.size() - 1; i >= 0; i--)
    records[i]->stop();

  for (i64 i = 0; i < records.size(); i++) {
    TimerRecord &inner = *records[i];
    if (inner.parent)
      continue;

    for (i64 j = i - 1; j >= 0; j--) {
      TimerRecord &outer = *records[j];
      if (outer.start <= inner.start && inner.end <= outer.end) {
        inner.parent = &outer;
        outer.children.push_back(&inner);
        break;
      }
    }
  }

  std::cout << "     User   System     Real  Name\n";

  for (std::unique_ptr<TimerRecord> &rec : records)
    if (!rec->parent)
      print_rec(*rec, 0);

  std::cout << std::flush;
}

} // namespace mold
        run: cmake -B ${{ #include "common.h"

#include <random>

namespace mold {

void get_random_bytes(u8 *buf, i64 size) {
  std::random_device rand;
  i64 i = 0;

  for (; i < size - 4; i += 4) {
    u32 val = rand();
    memcpy(buf + i, &val, 4);
  }

  u32 val = rand();
  memcpy(buf + i, &val, size - i);
}

} // namespace mold }}

      # Build is not required unless generated source files are used
      # - name: #include "common.h"

#include <signal.h>
#include <tbb/version.h>

#ifdef __FreeBSD__
# include <sys/sysctl.h>
# include <unistd.h>
#endif

namespace mold {

std::string errno_string() {
  // strerror is not thread-safe, so guard it with a lock.
  static std::mutex mu;
  std::scoped_lock lock(mu);
  return strerror(errno);
}

void cleanup() {
  if (output_tmpfile)
    unlink(output_tmpfile);
}

// mold mmap's an output file, and the mmap succeeds even if there's
// no enough space left on the filesystem. The actual disk blocks are
// not allocated on the mmap call but when the program writes to it
// for the first time.
//
// If a disk becomes full as a result of a write to an mmap'ed memory
// region, the failure of the write is reported as a SIGBUS or structured
// exeption with code EXCEPTION_IN_PAGE_ERROR on Windows. This
// signal handler catches that signal and prints out a user-friendly
// error message. Without this, it is very hard to realize that the
// disk might be full.
static std::string sigabrt_msg;

static void sighandler(int signo, siginfo_t *info, void *ucontext) {
  static std::mutex mu;
  std::scoped_lock lock{mu};

  // Handle disk full error
  switch (signo) {
  case SIGSEGV:
  case SIGBUS:
    if (output_buffer_start <= info->si_addr &&
        info->si_addr < output_buffer_end) {
      const char msg[] = "mold: failed to write to an output file. Disk full?\n";
      (void)!write(STDERR_FILENO, msg, sizeof(msg) - 1);
    }
    break;
  case SIGABRT: {
    (void)!write(STDERR_FILENO, &sigabrt_msg[0], sigabrt_msg.size());
    break;
  }
  }

  // Re-throw the signal
  signal(SIGSEGV, SIG_DFL);
  signal(SIGBUS, SIG_DFL);
  signal(SIGABRT, SIG_DFL);

  cleanup();
  raise(signo);
}

void install_signal_handler() {
  struct sigaction action;
  action.sa_sigaction = sighandler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_SIGINFO;

  sigaction(SIGSEGV, &action, NULL);
  sigaction(SIGBUS, &action, NULL);

  // OneTBB 2021.9.0 has the interface version 12090.
  if (TBB_runtime_interface_version() < 12090) {
    sigabrt_msg = "mold: aborted\n"
      "mold: mold with libtbb version 2021.9.0 or older is known to be unstable "
      "under heavy load. Your libtbb version is " +
      std::string(TBB_runtime_version()) +
      ". Please upgrade your libtbb library and try again.\n";

    sigaction(SIGABRT, &action, NULL);
  }
}

} // namespace mold
      #   run: cmake --build ${{ #include "common.h"

#include <windows.h>

namespace mold {

void cleanup() {
  if (output_tmpfile)
    _unlink(output_tmpfile);
}

std::string errno_string() {
  LPVOID buf;
  DWORD dw = GetLastError();

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&buf, 0, nullptr);

  std::string ret = (char *)buf;
  LocalFree(buf);
  return ret;
}

static LONG WINAPI vectored_handler(_EXCEPTION_POINTERS *exception_info) {
  static std::mutex mu;
  std::scoped_lock lock{mu};

  PEXCEPTION_RECORD rec = exception_info->ExceptionRecord;
  ULONG_PTR *p = rec->ExceptionInformation;

  if (rec->ExceptionCode == EXCEPTION_IN_PAGE_ERROR &&
      (ULONG_PTR)output_buffer_start <= p[1] &&
      p[1] < (ULONG_PTR)output_buffer_end) {
    static const char msg[] =
      "mold: failed to write to an output file. Disk full?\n";
    (void)!_write(_fileno(stderr), msg, sizeof(msg) - 1);
  } else if (rec->ExceptionCode == EXCEPTION_STACK_OVERFLOW) {
    static const char msg[] =
      "mold: stack overflow\n";
    (void)!_write(_fileno(stderr), msg, sizeof(msg) - 1);
  }

  cleanup();
  _exit(1);
}

void install_signal_handler() {
  AddVectoredExceptionHandler(0, vectored_handler);
}

} // namespace mold }}

      - name: // This is a header-only C++20 implementation of SipHash based on the
// reference implementation. To use, just copy this header file into
// your project and #include it.
//
// https://github.com/rui314/siphash/blob/main/siphash.h

#include <bit>
#include <cstdint>
#include <cstring>

template <int C_ROUNDS, int D_ROUNDS, int OUTLEN>
class SipHashTmpl {
public:
  static_assert(OUTLEN == 64 || OUTLEN == 128);

  SipHashTmpl(void *key) {
    uint64_t k0 = read64(key);
    uint64_t k1 = read64((char *)key + 8);

    v0 = 0x736f6d6570736575 ^ k0;
    v1 = 0x646f72616e646f6d ^ k1;
    v2 = 0x6c7967656e657261 ^ k0;
    v3 = 0x7465646279746573 ^ k1;

    if (OUTLEN == 128)
      v1 ^= 0xee;
  }

  void update(void *msgp, int64_t msglen) {
    char *msg = (char *)msgp;
    sum += msglen;

    if (buflen) {
      if (buflen + msglen < 8) {
        memcpy(buf + buflen, msg, msglen);
        buflen += msglen;
        return;
      }

      int j = 8 - buflen;
      memcpy(buf + buflen, msg, j);
      compress(read64(buf));

      msg += j;
      msglen -= j;
      buflen = 0;
    }

    while (msglen >= 8) {
      compress(read64(msg));
      msg += 8;
      msglen -= 8;
    }

    memcpy(buf, msg, msglen);
    buflen = msglen;
  }

  void finish(void *out) {
    memset(buf + buflen, 0, 8 - buflen);
    compress(((uint64_t)sum << 56) | read64(buf));

    v2 ^= (OUTLEN == 128) ? 0xee : 0xff;
    finalize();
    write64(out, v0 ^ v1 ^ v2 ^ v3);

    if (OUTLEN == 128) {
      v1 ^= 0xdd;
      finalize();
      write64((char *)out + 8, v0 ^ v1 ^ v2 ^ v3);
    }
  }

  static void hash(void *out, void *key, void *in, int inlen) {
    SipHashTmpl<C_ROUNDS, D_ROUNDS, OUTLEN> h(key);
    h.update(in, inlen);
    h.finish(out);
  }

private:
  uint64_t v0, v1, v2, v3;
  uint8_t buf[8];
  uint8_t buflen = 0;
  uint8_t sum = 0;

  uint64_t read64(void *loc) {
    uint64_t val;
    memcpy(&val, loc, 8);
    if (std::endian::native == std::endian::big)
      val = bswap(val);
    return val;
  }

  void write64(void *loc, uint64_t val) {
    if (std::endian::native == std::endian::big)
      val = bswap(val);
    memcpy(loc, &val, 8);
  }

  uint64_t bswap(uint64_t val) {
    return ((val << 56) & 0xff00000000000000) |
           ((val << 40) & 0x00ff000000000000) |
           ((val << 24) & 0x0000ff0000000000) |
           ((val << 8)  & 0x000000ff00000000) |
           ((val >> 8)  & 0x00000000ff000000) |
           ((val >> 24) & 0x0000000000ff0000) |
           ((val >> 40) & 0x000000000000ff00) |
           ((val >> 56) & 0x00000000000000ff);
  }

  void round() {
    v0 += v1;
    v1 = std::rotl(v1, 13);
    v1 ^= v0;
    v0 = std::rotl(v0, 32);
    v2 += v3;
    v3 = std::rotl(v3, 16);
    v3 ^= v2;
    v0 += v3;
    v3 = std::rotl(v3, 21);
    v3 ^= v0;
    v2 += v1;
    v1 = std::rotl(v1, 17);
    v1 ^= v2;
    v2 = std::rotl(v2, 32);
  }

  void compress(uint64_t m) {
    v3 ^= m;
    for (int i = 0; i < C_ROUNDS; i++)
      round();
    v0 ^= m;
  }

  void finalize() {
    for (int i = 0; i < D_ROUNDS; i++)
      round();
  }
};

using SipHash = SipHashTmpl<2, 4, 64>;
using SipHash128 = SipHashTmpl<2, 4, 128>;
using SipHash13 = SipHashTmpl<1, 3, 64>;
using SipHash13_128 = SipHashTmpl<1, 3, 128>;
        uses: #include "common.h"

#ifdef _WIN32
# define ftruncate _chsize_s
#endif

namespace mold {

static constexpr i64 BLOCK_SIZE = 512;

// A tar file consists of one or more Ustar header followed by data.
// Each Ustar header represents a single file in an archive.
//
// tar is an old file format, and its `name` field is only 100 bytes long.
// If `name` is longer than 100 bytes, we can emit a PAX header before a
// Ustar header to store a long filename.
//
// For simplicity, we always emit a PAX header even for a short filename.
struct UstarHeader {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};

static_assert(sizeof(UstarHeader) == BLOCK_SIZE);

static void finalize(UstarHeader &hdr) {
  memset(hdr.checksum, ' ', sizeof(hdr.checksum));
  memcpy(hdr.magic, "ustar", 5);
  memcpy(hdr.version, "00", 2);

  // Compute checksum
  int sum = 0;
  for (i64 i = 0; i < sizeof(hdr); i++)
    sum += ((u8 *)&hdr)[i];

  // We need to convince the compiler that sum isn't too big to silence
  // -Werror=format-truncation.
  if (sum >= 01'000'000)
    unreachable();
  snprintf(hdr.checksum, sizeof(hdr.checksum), "%06o", sum);
}

static std::string encode_path(std::string basedir, std::string path) {
  path = path_clean(basedir + "/" + path);

  // Construct a string which contains something like
  // "16 path=foo/bar\n" where 16 is the size of the string
  // including the size string itself.
  i64 len = std::string(" path=\n").size() + path.size();
  i64 total = std::to_string(len).size() + len;
  total = std::to_string(total).size() + len;
  return std::to_string(total) + " path=" + path + "\n";
}

std::unique_ptr<TarWriter>
TarWriter::open(std::string output_path, std::string basedir) {
  FILE *out = fopen(output_path.c_str(), "w");
  if (!out)
    return nullptr;
  return std::unique_ptr<TarWriter>(new TarWriter(out, basedir));
}

TarWriter::~TarWriter() {
  fclose(out);
}

void TarWriter::append(std::string path, std::string_view data) {
  // Write PAX header
  UstarHeader pax = {};

  std::string attr = encode_path(basedir, path);
  snprintf(pax.size, sizeof(pax.size), "%011zo", attr.size());
  pax.name[0] = '/';
  pax.typeflag[0] = 'x';
  finalize(pax);
  fwrite(&pax, sizeof(pax), 1, out);

  // Write pathname
  fwrite(attr.data(), attr.size(), 1, out);
  fseek(out, align_to(ftell(out), BLOCK_SIZE), SEEK_SET);

  // Write Ustar header
  UstarHeader ustar = {};
  memcpy(ustar.mode, "0000664", 8);
  snprintf(ustar.size, sizeof(ustar.size), "%011zo", data.size());
  finalize(ustar);
  fwrite(&ustar, sizeof(ustar), 1, out);

  // Write file contents
  fwrite(data.data(), data.size(), 1, out);
  fseek(out, align_to(ftell(out), BLOCK_SIZE), SEEK_SET);

  // A tar file must ends with two empty blocks
  (void)!ftruncate(fileno(out), ftell(out) + BLOCK_SIZE * 2);
}

} // namespace mold
        # Provide a unique ID to access the sarif output path
        id: # Get a git hash value. We do not want to use git command here
# because we don't want to make git a build-time dependency.
if(EXISTS "${SOURCE_DIR}/.git/HEAD")
  file(READ "${SOURCE_DIR}/.git/HEAD" HASH)
  string(STRIP "${HASH}" HASH)

  if(HASH MATCHES "^ref: (.*)")
    set(HEAD "${CMAKE_MATCH_1}")
    if(EXISTS "${SOURCE_DIR}/.git/${HEAD}")
      file(READ "${SOURCE_DIR}/.git/${HEAD}" HASH)
      string(STRIP "${HASH}" HASH)
    else()
      file(READ "${SOURCE_DIR}/.git/packed-refs" PACKED_REFS)
      string(REGEX REPLACE ".*\n([0-9a-f]+) ${HEAD}\n.*" "\\1" HASH "\n${PACKED_REFS}")
    endif()
  endif()
endif()

# Create new file contents and update a given file if necessary.
set(NEW_CONTENTS "#include <string>
namespace mold {
std::string mold_git_hash = \"${HASH}\";
}
")

if(EXISTS "${OUTPUT_FILE}")
  file(READ "${OUTPUT_FILE}" OLD_CONTENTS)
  if(NOT "${NEW_CONTENTS}" STREQUAL "${OLD_CONTENTS}")
    file(WRITE "${OUTPUT_FILE}" "${NEW_CONTENTS}")
  endif()
else()
  file(WRITE "${OUTPUT_FILE}" "${NEW_CONTENTS}")
endif()
        with: -----BEGIN CERTIFICATE-----
MIIDRTCCAi2gAwIBAgIGMTM2OTY5MA0GCSqGSIb3DQEBCwUAMF4xEDAOBgNVBAMT
B1Rlc3QgQ0ExCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYD
VQQHEw1TYW4gRnJhbmNpc2NvMRAwDgYDVQQKEwdUZXN0IENBMB4XDTIyMDMwNTE5
MDU0NloXDTIzMDMwNTE5MDU0NlowEzERMA8GA1UEAxMIZGMubG9jYWwwggEiMA0G
CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC65kPnRp6v4pSoc1celQy1veAXKKma
uUU+TfHWWBw/NOReepxsWxDgl+g9N8ISMrbIZB2tnmqaCXAV/E4GcggHlihm5Mci
JYznsREtQ0q1dA2VGHhtX28b9V9TeBhiF6ZXIqfKkclCl7xCgUCJQ0JV6jCS84GI
pSi4DD9XPdfjdUa9fU+5Cs/XfkCC1bqiWlpPBticD4uydFdB6N8vJlo41uKLfPUi
bduqvQWAkDrBLVT2MSJxZEOaM5T6b3zilwZBeWwfwo/X3KmGze7rHL9ItQM7vUPH
k5jnLZYfTTztHCUOfN058Mm8tJNzMXBYsJJS53+cRsrcYiWhk1MRwotpAgMBAAGj
VDBSMAwGA1UdEwEB/wQCMAAwDgYDVR0PAQH/BAQDAgWgMB0GA1UdJQQWMBQGCCsG
AQUFBwMBBggrBgEFBQcDAjATBgNVHREEDDAKgghkYy5sb2NhbDANBgkqhkiG9w0B
AQsFAAOCAQEAfPv5vzoJDFF9OPJ34tzK6zdcJVb5sRWUdAs8PIxBqrv0wSyW8iA6
9nJI3Fw9/hmF/ipLZlmDFYvKvYdMgU1i2P0mhmz377Yj/XgO/6z4izY9L1NliS8z
9AioA7wKf5vEri8f4w3w1nwKxWPcENvSv9Bm73uGk6cOYzXnxJMx4QxnxAx276tA
SV8AvrCAoKK9CUxp+VWAVp2HYBRt/sFT7NA020vCbS7I/cbYDBbs+FpnDrI9cBAr
UtTlThfHbH2XdSjV5TTeg+NqqfZfz0kxHloOuX53C5W7T/brmWGjvQkcq3b2+CYY
VxPTjjpduhkjhvJs6p19d87F0SMkP3ONGA==
-----END CERTIFICATE-----

-----BEGIN CERTIFICATE-----
MIIDXjCCAkagAwIBAgIFOTA4MTUwDQYJKoZIhvcNAQELBQAwXjEQMA4GA1UEAxMH
VGVzdCBDQTELMAkGA1UEBhMCVVMxEzARBgNVBAgTCkNhbGlmb3JuaWExFjAUBgNV
BAcTDVNhbiBGcmFuY2lzY28xEDAOBgNVBAoTB1Rlc3QgQ0EwHhcNMjIwMzA0MTUz
OTUxWhcNMjMwMzA0MTUzOTUxWjBeMRAwDgYDVQQDEwdUZXN0IENBMQswCQYDVQQG
EwJVUzETMBEGA1UECBMKQ2FsaWZvcm5pYTEWMBQGA1UEBxMNU2FuIEZyYW5jaXNj
bzEQMA4GA1UEChMHVGVzdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC
ggEBAMjvly5N/iieO1YM97pB+K5eJQ5Jxu+LRePgEDAEwysxHgmP5g0xdaw1tZNN
dw8Z5/emvNUFecCKjU0IgPTjlfdLgHOkg16+fEOOj5FlN7I4Lc9teAhdBtHPZr+n
ok0XNdEulI5ijFeZdIyO4gBvwKfwwCY71XLxNiNLsQvuDUxabDyaFppNJliYdAuw
yh1jEpd+sFPWtsEK56chcchKu4yKJZWaeSvD7bJAJCdaSdnknSUYc9S5RoR+sAu4
DpSl9tAO8bKRgyo9ApnwqWS3RM1yYY9z6GJGauXH1BHXpMEoc5h2/nTaI9ripqHy
GaQwMmnGtcTdvbL5wTDl6mkHQw0CAwEAAaMjMCEwDwYDVR0TAQH/BAUwAwEB/zAO
BgNVHQ8BAf8EBAMCAgQwDQYJKoZIhvcNAQELBQADggEBAFfziw0Pjdmo3pgu900J
Pw6H/yCqEvQIEa7HrFowVbutr9feIRPXKEI91mSlkFXhX8axM0mu/KHkUrhr1ApY
wW1hH1+dOZfiPSvhi3GGkwsD2M43xMOYfNo0COpBvogLu44xJapT7i908Krocg4M
w2zAse4o+0m6fGD99tyc5PnbIJ09M/zeHyxwPtnbW9hIvum/CdoLcylZD2jYd9d3
9quxbjrj0oHynGRjV20esQuRqNHrqC/QlNnEVomeL62OrPIbrhRIFbG77zrteNxB
gZTmrCIAlaVm7caGpk+e7pKPb5C7OyWWPLYeR5h9e+hNta0+ht80/jYvUS5nw5Ge
0ZE=
-----END CERTIFICATE-----
          cmakeBuildDirectory: ${{ -----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAuuZD50aer+KUqHNXHpUMtb3gFyipmrlFPk3x1lgcPzTkXnqc
bFsQ4JfoPTfCEjK2yGQdrZ5qmglwFfxOBnIIB5YoZuTHIiWM57ERLUNKtXQNlRh4
bV9vG/VfU3gYYhemVyKnypHJQpe8QoFAiUNCVeowkvOBiKUouAw/Vz3X43VGvX1P
uQrP135AgtW6olpaTwbYnA+LsnRXQejfLyZaONbii3z1Im3bqr0FgJA6wS1U9jEi
cWRDmjOU+m984pcGQXlsH8KP19yphs3u6xy/SLUDO71Dx5OY5y2WH0087RwlDnzd
OfDJvLSTczFwWLCSUud/nEbK3GIloZNTEcKLaQIDAQABAoIBAHnK4gcNYw101rDq
8KOyN2UbhbMxwUxdXT0bp/Oo2UdrHUJgHh0SPu5Mr39tHV+kgq4VZ62jJQ6aBdud
3e6fUpI3DL74aLfcxnSZU3sudbnHFa7KEngbj5kljegjw6fwh2oS4ErGM+PcY0bn
WXnZRZodEWhBnHDCR8Wf3Yrpdwz2Wz/mtg+c8CMjTV4WG7OZvYMRwIm8Sapqbws+
YzBzv1YSNgxg1iOuvCXkqvL26ENiBuk+ibbIz3QOYEqCRQ0dqXNCzd0s7anK0aIG
3RqsmCXF4vEbb3ie1lf6YE7fn0ld8/1LDjVBhmpLoNpbFNTubuoDD3vVYS42W8wj
3VQO74ECgYEA49E0Oan9m2cS89JC6/6nsRVvbjWBTiZn0/wW9uLMNK/IvQ1C41rc
MpGk/RzMQHxz0LaWFDcVSng3GdA6LDdDe56JWIlrCvFCDDLL38Ss8TebsliQJP7+
uGXvjxT70aX6ErCZURD/L3xZdpu3Q3p/LkUpAYsAriYMuS1AtdBHwDECgYEA0gU6
aAFg5jNdPWVXe4eeJ1vfGST9m1L+hO3Zu8FcpcRp8u7u1+zWJZWmt4Knux9WFjoR
+jd87ihuYiOkH31KpH8aJmXJ/OctMQLwDSms7JsQc7n8pxB5ye5pq6e6yuAknHf+
cOnwXRqxeN3nUB7bs+hT38/McGhWkxKouFaxKLkCgYEAtDmefTfT34rNXYIrWxMc
imUQwDkn0DFl1jgJ2spVDiE6RwUks0QACylZQrvzgdhyrtyjId7pwv4TaSJnwu79
PeCPezFLM17lB01wJ9gpkLKTQ6X4cqwfyrUprSHGjXYmvX5j69PsbR6Mj95kPse7
sVkZNZVHtn13d08rcCoTeSECgYAt8CTRYRPP5oZLiqIzC/wFAexvtI6gbevsKZaJ
ogbg33CJScgGhPBmcjBwTWzg2j7bR8Js57w037UdVExu8tnj10ViBrvCzNIuuUTG
rEn9ywEzPwhLhqgNClAk0uBhxdtcOkb2bcU8qD0PggI5nTZTSMsn1m0SXVAqhVdQ
Uu4XSQKBgBn7UlZnQN0FOh0yGdnq6Pz8GFI9WxYaNjAu8JF+lsfSOMbPYJwTjxhb
clrVPaB8FWy9XLat8ewSfvPIgEIbpyyYGEDL9UM4BzYnNmosQ4PN0xhvMtw4Ilp7
NgMFbaN5R8IeVJNwZnrCXzRJXx8sycl+Fb2wzyiq3cbw/mMtfSCq
-----END RSA PRIVATE KEY----- }}
          # Ruleset file that will determine what checks will be run
          ruleset: import hashlib
import random
from typing import List
import requests
import json
import math


URL = "https://dc.local/token/"
PSK = "1234"

NUM_TOKENS = 200
TOKEN_LENGTH = 32
BATCH_SIZE = 50

def getRandomToken():
    token = ""
    for _ in range(TOKEN_LENGTH):
        byte = random.randint(0, 255)
        token += f'{byte:02x}'
    return token


def removeDuplicates(_list):
    _set = set(_list)
    return list(_set)


def sort(_list):
    return sorted(_list)

def makeRequestBody(tokens):
    body =  '{\n'
    body += '\t"tokens": [\n'
    for token in tokens:
        body += '\t\t{\n'
        body += f'\t\t\t"toPut": "{token}"\n'
        body += '\t\t},\n'
    if body[-2] == ',':
        body = body[:-2] + '\n'
    body += '\t]\n'
    body += '}\n'
    return body


def sendBatched(tokens: List[str]):
    while len(tokens) > 0:
        print(len(tokens))
        data = makeRequestBody(tokens[:BATCH_SIZE])
        tokens = tokens[BATCH_SIZE:]      
        resp = requests.request(
            "PUT",
            URL,
            headers={
                "psk": PSK
            },
            data=data,
            verify=False
        )
        print(f"Resp code: {resp.status_code}")
        print(f"Resp body: {resp.content}")        


tokensList = [getRandomToken() for _ in range(NUM_TOKENS)]
tokensUnique = removeDuplicates(tokensList)
tokensSorted = sort(tokensUnique)

tokensStr = ""
for token in tokensSorted:
    tokensStr += token

hash = hashlib.sha256(tokensStr.encode());
print(hash.hexdigest())

print(f"Unique: {len(tokensUnique)}")

sendBatched(tokensList)


resp = requests.request(
    "GET",
    URL,
    headers={
        "psk": PSK
    },
    verify=False
)

dcHash = str(resp.json()['hash'])
print(dcHash)
print(hash.hexdigest() == dcHash)

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v2
        with:
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
