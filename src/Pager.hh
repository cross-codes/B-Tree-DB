#pragma once
#include "defines.hh"
#include <cstdint>
#include <string>

class Pager
{
public:
  int file_descriptor;
  int64_t file_length;
  void *pages[TABLE_MAX_PAGES];
  int64_t num_pages;

  Pager(const std::string &filename);
  ~Pager();

  void *get_page(int64_t page_num);
  void flush(int64_t page_num);
  uint32_t get_unused_page();
};
