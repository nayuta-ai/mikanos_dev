/**
 * @file paging.hpp
 *
 * @file paging.hpp * A file containing programs for memory paging.
 */

#pragma once

#include <cstddef>
/** @brief Number of page directories to be statically reserved
 *
 * This constant is used in SetupIdentityPageMap.
 * Since one page directory can have 512 2MiB pages, it is
 * * Since one page directory can have 512 2MiB pages, this means that
 * kPageDirectoryCount x 1GiB virtual addresses will be mapped.
 */

const size_t kPageDirectoryCount = 64;
/** @brief Set the page table so that virtual address = physical address.
 * Finally, the CR3 register points to the correctly set page table.
 */
void SetupIdentityPageTable();

void InitializePaging();