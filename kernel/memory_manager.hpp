/**
 * @file memory_manager.hpp
 *
 * File containing memory management classes and peripheral functions.
 */

#pragma once

#include <array>
#include <limits>

#include "error.hpp"
#include "memory_map.hpp"

namespace {
constexpr unsigned long long operator""_KiB(unsigned long long kib) {
  return kib * 1024;
}

constexpr unsigned long long operator""_MiB(unsigned long long mib) {
  return mib * 1024_KiB;
}

constexpr unsigned long long operator""_GiB(unsigned long long gib) {
  return gib * 1024_MiB;
}
}  // namespace

/** @brief Size of one physical memory frame (bytes) */
static const auto kBytesPerFrame{4_KiB};

class FrameID {
 public:
  explicit FrameID(size_t id) : id_{id} {}
  size_t ID() const { return id_; }
  void *Frame() const { return reinterpret_cast<void *>(id_ * kBytesPerFrame); }

 private:
  size_t id_;
};

static const FrameID kNullFrame{std::numeric_limits<size_t>::max()};

/** @brief A class that manages memory on a frame-by-frame basis using a bitmap
array.
* Each bit of the bitmap corresponds to one frame, with a value of 0 indicating
an
* available frame and 1 indicating an allocated frame. The m-th bit of
* alloc_map[n] corresponds to the physical address calculated as follows:
* kFrameBytes * (n * kBitsPerMapLine + m)
*/
// #@@range_begin(bitmap_memory_manager)
class BitmapMemoryManager {
 public:
  /* @brief The maximum amount of physical memory (in bytes) that this memory
  manager can handle. */
  static const auto kMaxPhysicalMemoryBytes{128_GiB};
  /* @brief The number of frames required to handle physical memory up to
  kMaxPhysicalMemoryBytes. */
  static const auto kFrameCount{kMaxPhysicalMemoryBytes / kBytesPerFrame};
  /** @brief The type of each element of the bitmap array. */
  using MapLineType = unsigned long;
  /* @brief The number of frames per element of the bitmap array. */
  static const size_t kBitsPerMapLine{8 * sizeof(MapLineType)};

  /** @brief Initializes an instance of this class. */
  BitmapMemoryManager();

  /** @brief Allocates a region of the requested number of frames and returns
   * the ID of the first frame allocated. */
  WithError<FrameID> Allocate(size_t num_frames);
  Error Free(FrameID start_frame, size_t num_frames);
  void MarkAllocated(FrameID start_frame, size_t num_frames);

  /** @brief Sets the memory range managed by this memory manager.
   * After this call, memory allocation using Allocate will only be performed
   * within the specified range.
   * @param range_begin_ The start of the memory range.
   * @param range_end_ The end of the memory range, which is one frame past the
   * final frame.
   */
  void SetMemoryRange(FrameID range_begin, FrameID range_end);

 private:
  std::array<MapLineType, kFrameCount / kBitsPerMapLine> alloc_map_;
  /** @brief The start of the memory range managed by this memory manager. */
  FrameID range_begin_;
  /* @brief The end of the memory range managed by this memory manager, which is
  one frame past the final frame. */
  FrameID range_end_;

  bool GetBit(FrameID frame) const;
  void SetBit(FrameID frame, bool allocated);
};

extern BitmapMemoryManager* memory_manager;
void InitializeMemoryManager(const MemoryMap &memory_map);
