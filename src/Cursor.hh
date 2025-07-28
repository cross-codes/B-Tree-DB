#pragma once

#include "Table.hh"

class Cursor
{
public:
  int64_t page_num, cell_num;
  Table *table{};
  bool eot;
  Cursor(Table *table, bool end);

  void *get_value();
  void advance();
};
