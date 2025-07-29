#pragma once

#include "Table.hh"

class Cursor
{
public:
  int64_t page_num, cell_num;
  Table *table{};
  bool eot;
  Cursor(Table *table);
  Cursor(Table *table, uint32_t key_to_insert);

  void *get_value();
  void advance();
};
