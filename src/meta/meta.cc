#include "meta.hh"
#include "../Node.hh"
#include <format>
#include <iostream>

namespace meta
{
auto process_meta_cmd(std::string input, Table &table) -> MetaCommandResult
{
  if (input == ".exit")
  {
    table.close_proc();
    exit(0);
  }
  else if (input == ".constants")
  {
    std::cout << "Displaying constants\n";
    print_constants();
    return MetaCommandResult::META_COMMAND_SUCCESS;
  }
  else if (input == ".btree")
  {
    std::cout << "Tree order statistics:\n";
    print_leaf_node(table.pager->get_page(0));
    return MetaCommandResult::META_COMMAND_SUCCESS;
  }
  else
    return MetaCommandResult::META_COMMAND_UNRECOGNIZED_COMMAND;
}

void print_constants()
{
  std::cout << std::format("ROW_SIZE: {}\n", ROW_SIZE);
  std::cout << std::format("COMMON_NODE_HEADER_SIZE: {}\n",
                           Node::COMMON_NODE_HEADER_SIZE);
  std::cout << std::format("LEAF_NODE_HEADER_SIZE: {}\n",
                           Node::LEAF_NODE_HEADER_SIZE);
  std::cout << std::format("LEAF_NODE_CELL_SIZE: {}\n",
                           Node::LEAF_NODE_CELL_SIZE);
  std::cout << std::format("LEAF_NODE_SPACE_FOR_CELLS: {}\n",
                           Node::LEAF_NODE_SPACE_FOR_CELLS);
  std::cout << std::format("LEAF_NODE_MAX_CELLS: {}\n",
                           Node::LEAF_NODE_MAX_CELLS);
}

void print_leaf_node(void *node)
{
  uint32_t num_cells = *(Node::leaf_node_num_cells(node));
  std::cout << std::format("leaf (size {})\n", num_cells);

  for (uint32_t i = 0; i < num_cells; i++)
  {
    uint32_t key = *reinterpret_cast<uint32_t *>(Node::leaf_node_key(node, i));
    std::cout << std::format("  - {} : {}\n", i, key);
  }
}
} // namespace meta
