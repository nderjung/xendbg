//
// Created by Spencer Michaels on 8/13/18.
//

#include <cstring>

#include "Domain.hpp"
#include "XenForeignMemory.hpp"
#include "XenException.hpp"

// NOTE: This needs to be declared after Domain.hpp, which includes xenctrl.h.
// For some reason, including xenforeignmemory.h before xenctrl.h will fail.
#include "BridgeHeaders/xenforeignmemory.h"

using xd::xen::MappedMemory;
using xd::xen::XenForeignMemory;
using xd::xen::XenException;

XenForeignMemory::XenForeignMemory()
    : _xen_foreign_memory(xenforeignmemory_open(NULL, 0), xenforeignmemory_close)
{
  if (!_xen_foreign_memory)
    throw XenException("Failed to open Xen foreign memory handle!");
}

MappedMemory XenForeignMemory::map(Domain &domain, Address address, size_t size, int prot) {
  xen_pfn_t base_page_frame_num = ((uintptr_t)address) >> XC_PAGE_SHIFT;
  size_t num_pages = (size + XC_PAGE_SIZE - 1) >> XC_PAGE_SHIFT;

  auto pages = (xen_pfn_t*)malloc(num_pages * sizeof(xen_pfn_t));
  auto errors = (int*)malloc(num_pages * sizeof(int));

  if (!pages)
    throw XenException("Failed to allocate PFN table!");
  if (!errors)
    throw XenException("Failed to allocate error table!"); // TODO

  for (int i = 0; i < num_pages; ++i) {
    pages[i] = base_page_frame_num + 1;
  }

  char *mem_page_base = (char*)xenforeignmemory_map(_xen_foreign_memory.get(), domain.get_domid(), prot, num_pages, pages, errors);
  char *mem = mem_page_base + ((uintptr_t)address) % XC_PAGE_SIZE;

  for (int i = 0; i < num_pages; ++i) {
    if (errors[i])
      throw XenException("Failed to map page " + std::to_string(i+1) + " of " + std::to_string(num_pages) + ": " + std::strerror(-errors[i])); // TODO
  }

  return std::shared_ptr<char>(mem, [this, address, num_pages](void *memory) {
    if (memory) {
      xenforeignmemory_unmap(_xen_foreign_memory.get(), address, num_pages);
    }
  });
}
