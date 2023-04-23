/**
 *@file fat.hpp
 *A file that collects programs for manipulating FAT file systems.
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace fat {

// Boot Parameter Block structure
struct BPB {
  uint8_t jump_boot[3];
  char oem_name[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sector_count;
  uint8_t num_fats;
  uint16_t root_entry_count;
  uint16_t total_sectors_16;
  uint8_t media;
  uint16_t fat_size_16;
  uint16_t sectors_per_track;
  uint16_t num_heads;
  uint32_t hidden_sectors;
  uint32_t total_sectors_32;
  uint32_t fat_size_32;
  uint16_t ext_flags;
  uint16_t fs_version;
  uint32_t root_cluster;
  uint16_t fs_info;
  uint16_t backup_boot_sector;
  uint8_t reserved[12];
  uint8_t drive_number;
  uint8_t reserved1;
  uint8_t boot_signature;
  uint32_t volume_id;
  char volume_label[11];
  char fs_type[8];
} __attribute__((packed));

// Attribute flags for directory entries
enum class Attribute : uint8_t {
  kReadOnly = 0x01,
  kHidden = 0x02,
  kSystem = 0x04,
  kVolumeID = 0x08,
  kDirectory = 0x10,
  kArchive = 0x20,
  kLongName = 0x0f,
};

// Directory entry structure
struct DirectoryEntry {
  unsigned char name[11];
  Attribute attr;
  uint8_t ntres;
  uint8_t create_time_tenth;
  uint16_t create_time;
  uint16_t create_date;
  uint16_t last_access_date;
  uint16_t first_cluster_high;
  uint16_t write_time;
  uint16_t write_date;
  uint16_t first_cluster_low;
  uint32_t file_size;

  uint32_t FirstCluster() const {
    return first_cluster_low |
           (static_cast<uint32_t>(first_cluster_high) << 16);
  }
} __attribute__((packed));

// Pointer to the boot volume image
extern BPB* boot_volume_image;
extern unsigned long bytes_per_cluster;
// Initializes the volume image
void Initialize(void* volume_image);

/** @brief Returns the memory address where the first sector of the specified
 *cluster is located.
 *@param cluster Cluster number (starting from 2)
 *@return The memory address where the first sector of the cluster is located
 */
uintptr_t GetClusterAddr(unsigned long cluster);
/** @brief Returns a pointer to the memory area where the first sector of the
 *specified cluster is located.
 *@param cluster Cluster number (starting from 2)
 *@return A pointer to the memory area where the first sector of the cluster is
 *located
 */
template <class T>
T* GetSectorByCluster(unsigned long cluster) {
  return reinterpret_cast<T*>(GetClusterAddr(cluster));
}
/** @brief Separates the short name of a directory entry into a base name and an
 *extension. Padding spaces (0x20) are removed and null-terminated.
 *@param entry The directory entry to get the file name from
 *@param base An array of 9 or more bytes to store the base name (without
 *extension)
 *@param ext An array of 4 or more bytes to store the extension
 */
void ReadName(const DirectoryEntry& entry, char* base, char* ext);

static const unsigned long kEndOfClusterchain = 0x0ffffffflu;

/** @brief Returns the next cluster number for the specified cluster.
 *
 * @param cluster cluster number
 * @return next cluster number (or kEndOfClusterchain if none)
 */
unsigned long NextCluster(unsigned long cluster);

/** @brief Search for files in the specified directory.
 *
 * @param name 8+3 format file name (case insensitive)
 * @param directory_cluster The starting cluster of the directory (if omitted,
 * search from the root directory)
 * @return Entry representing the file. If not found, nullptr.
 */
DirectoryEntry* FindFile(const char* name, unsigned long directory_cluster = 0);

bool NameIsEqual(const DirectoryEntry& entry, const char* name);

/** @brief Copies the contents of the specified file to a buffer.
 *
 * @param buf Destination of file contents.
 * @param len Size of buffer (in bytes)
 * @param entry Directory entry representing the file
 * @return Number of bytes read.
 */
size_t LoadFile(void* buf, size_t len, const DirectoryEntry& entry);

}  // namespace fat