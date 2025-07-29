#pragma once

#include "../Table.hh"
#include "../status/MetaCommandResult.hh"
#include <string>

namespace meta
{
auto process_meta_cmd(std::string input, Table &table) -> MetaCommandResult;
void print_constants();
void print_tree(Pager *pager, uint32_t page_num, uint32_t indentation_level);
} // namespace meta
