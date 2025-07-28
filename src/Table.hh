#pragma once

#include "Pager.hh"
#include <cstdint>

const uint32_t PAGE_SIZE = 4096; // 4 KiB

class Table
{
public:
  int64_t root_page_num;
  Pager *pager;

  Table(std::string filename);

  void close_proc();
};
