//
// Created by Spencer Michaels on 8/13/18.
//

#ifndef XENDBG_COMMON_HPP
#define XENDBG_COMMON_HPP

#include <cstdint>

#include <xenctrl.h>

namespace xd::xen {
  using DomID = uint32_t;
  using DomInfo = xc_dominfo_t;
  using VCPU_ID = uint32_t;
  using Address = void*;
  using WordSize = unsigned int;
}

#endif //XENDBG_COMMON_HPP
