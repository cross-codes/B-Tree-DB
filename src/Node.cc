#include "Node.hh"
#include <cstring>
#include <iostream>

namespace Node
{

uint32_t *leaf_node_num_cells(void *node)
{
  return reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(node) +
                                      LEAF_NODE_NUM_CELLS_OFFSET);
}

void *leaf_node_cell(void *node, uint32_t cell_num)
{
  return reinterpret_cast<uint8_t *>(node) + LEAF_NODE_HEADER_SIZE +
         cell_num * LEAF_NODE_CELL_SIZE;
}

void *leaf_node_key(void *node, uint32_t cell_num)
{
  return leaf_node_cell(node, cell_num);
}

void *leaf_node_value(void *node, uint32_t cell_num)
{
  return reinterpret_cast<uint8_t *>(leaf_node_cell(node, cell_num)) +
         LEAF_NODE_KEY_SIZE;
}

void initialize_leaf_node(void *node)
{
  set_node_type(node, NodeType::LEAF);
  *leaf_node_num_cells(node) = 0;
}

void leaf_node_insert(Cursor &cursor, uint32_t key, Row &value)
{
  void *node     = cursor.table->pager->get_page(cursor.page_num);
  auto num_cells = *leaf_node_num_cells(node);
  if (num_cells >= LEAF_NODE_MAX_CELLS)
  {
    std::cerr << "Node does not split yet\n";
    std::exit(1);
  }

  if (cursor.cell_num < num_cells)
  {
    for (uint32_t i = num_cells; i > cursor.cell_num; i--)
    {
      std::memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1),
                  LEAF_NODE_CELL_SIZE);
    }
  }

  (*leaf_node_num_cells(node)) += 1;
  *reinterpret_cast<uint32_t *>(
      leaf_node_key(node, static_cast<uint32_t>(cursor.cell_num))) = key;
  value.serialize_into_dest(
      leaf_node_value(node, static_cast<uint32_t>(cursor.cell_num)));
}

NodeType get_node_type(void *node)
{
  auto data     = reinterpret_cast<uint8_t *>(node);
  uint8_t value = data[NODE_TYPE_OFFSET];
  return static_cast<NodeType>(value);
}

void set_node_type(void *node, NodeType type)
{
  auto data              = reinterpret_cast<uint8_t *>(node);
  data[NODE_TYPE_OFFSET] = static_cast<uint8_t>(type);
}

} // namespace Node
