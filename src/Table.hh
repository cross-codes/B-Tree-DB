#pragma once

#include "Pager.hh"
#include "Row.hh"
#include "defines.hh"
#include <cstdint>

const uint32_t PAGE_SIZE      = 4096; // 4 KiB
const uint32_t ROWS_PER_PAGE  = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

struct Table
{
public:
  int64_t num_rows;
  Pager *pager;

  Table(std::string filename);

  auto row_slot(uint32_t row_num) -> void *;
  void close_proc();
};
