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
    print_tree(table.pager, 0, 0);
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

void print_tree(Pager *pager, uint32_t page_num, uint32_t indentation_level)
{
  auto indent = [](uint32_t level) {
    for (uint32_t i = 0; i < level; i++)
      std::cout << " ";
  };

  void *node = pager->get_page(page_num);
  uint32_t num_keys, child;

  switch (Node::get_node_type(node))
  {
  case (Node::NodeType::LEAF):
    num_keys = *Node::leaf_node_num_cells(node);
    indent(indentation_level);
    std::cout << std::format("- leaf (size {})\n", num_keys);
    for (uint32_t i = 0; i < num_keys; i++)
    {
      indent(indentation_level + 1);
      std::cout << std::format(" - {}\n", *(reinterpret_cast<uint32_t *>(
                                              Node::leaf_node_key(node, i))));
    }
    break;

  case (Node::NodeType::INTERNAL):
    num_keys = *Node::internal_node_num_keys(node);
    indent(indentation_level);
    std::cout << std::format("- internal (size {})\n", num_keys);

    if (num_keys > 0)
    {
      for (uint32_t i = 0; i < num_keys; i++)
      {
        child = *Node::internal_node_child(node, i);
        print_tree(pager, child, indentation_level + 1);

        indent(indentation_level + 1);
        std::cout << std::format("- key {}\n",
                                 *Node::internal_node_key(node, i));
      }

      child = *Node::internal_node_right_child(node);
      print_tree(pager, child, indentation_level + 1);
    }
    break;
  }
}
} // namespace meta
