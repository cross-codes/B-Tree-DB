#pragma once
#include "defines.hh"
#include <cstdint>
#include <string>

struct Pager
{
public:
  int file_descriptor;
  int64_t file_length;
  void *pages[TABLE_MAX_PAGES];

  Pager(const std::string &filename);
  ~Pager();

  void *get_page(int64_t page_num);
  void flush(int64_t page_num, int64_t size);
};
