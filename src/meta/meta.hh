#pragma once

#include "../Table.hh"
#include "../status/MetaCommandResult.hh"
#include <string>

namespace meta
{
auto process_meta_cmd(std::string input, Table &table) -> MetaCommandResult;
void print_constants();
void print_leaf_node(void *node);
} // namespace meta
