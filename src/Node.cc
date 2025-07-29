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
  set_node_root(node, false);
  *leaf_node_num_cells(node) = 0;
  *leaf_node_next_leaf(node) = 0;
}

void leaf_node_insert(Cursor &cursor, uint32_t key, Row &value)
{
  void *node     = cursor.table->pager->get_page(cursor.page_num);
  auto num_cells = *leaf_node_num_cells(node);
  if (num_cells >= LEAF_NODE_MAX_CELLS)
  {
    leaf_node_split_and_insert(cursor, key, value);
    return;
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

void leaf_node_split_and_insert(Cursor &cursor, uint32_t key, Row &value)
{
  void *old_node        = cursor.table->pager->get_page(cursor.page_num);
  uint32_t old_max      = get_node_max_key(old_node);

  uint32_t new_page_num = cursor.table->pager->get_unused_page();
  void *new_node        = cursor.table->pager->get_page(new_page_num);

  initialize_leaf_node(new_node);

  *node_parent(new_node)         = *node_parent(old_node);
  *leaf_node_next_leaf(new_node) = *leaf_node_next_leaf(old_node);
  *leaf_node_next_leaf(old_node) = new_page_num;

  for (uint32_t i = LEAF_NODE_MAX_CELLS + 1; i-- > 0;)
  {
    void *dest_node;
    uint32_t index_within_node;

    if (i >= LEAF_NODE_LEFT_SPLIT_COUNT)
    {
      dest_node         = new_node;
      index_within_node = i - LEAF_NODE_LEFT_SPLIT_COUNT;
    }
    else
    {
      dest_node         = old_node;
      index_within_node = i;
    }

    void *dest = leaf_node_cell(dest_node, index_within_node);

    if (i == cursor.cell_num)
    {
      value.serialize_into_dest(leaf_node_value(dest_node, index_within_node));
      *(reinterpret_cast<uint32_t *>(
          leaf_node_key(dest_node, index_within_node))) = key;
    }
    else
    {
      uint32_t src_cell_num = (i > cursor.cell_num) ? i - 1 : i;
      void *src             = leaf_node_cell(old_node, src_cell_num);
      std::memcpy(dest, src, LEAF_NODE_CELL_SIZE);
    }
  }

  *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
  *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

  if (is_node_root(old_node))
    create_new_root(cursor.table, new_page_num);
  else
  {
    uint32_t parent_page_num = *node_parent(old_node);
    uint32_t new_max         = get_node_max_key(old_node);
    void *parent             = cursor.table->pager->get_page(parent_page_num);

    update_internal_node_key(parent, old_max, new_max);
    internal_node_insert(cursor.table, parent_page_num, new_page_num);
  }
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

void create_new_root(Table *table, uint32_t right_child_page_num)
{
  void *root                   = table->pager->get_page(table->root_page_num);
  void *right_child            = table->pager->get_page(right_child_page_num);
  uint32_t left_child_page_num = table->pager->get_unused_page();
  void *left_child             = table->pager->get_page(left_child_page_num);
  std::memcpy(left_child, root, PAGE_SIZE);
  set_node_root(left_child, false);
  initialize_internal_node(root);
  set_node_root(root, true);
  *internal_node_num_keys(root)    = 1;
  *internal_node_child(root, 0)    = left_child_page_num;
  uint32_t left_child_max_key      = get_node_max_key(left_child);
  *internal_node_key(root, 0)      = left_child_max_key;
  *internal_node_right_child(root) = right_child_page_num;
  *node_parent(left_child)  = static_cast<uint32_t>(table->root_page_num);
  *node_parent(right_child) = static_cast<uint32_t>(table->root_page_num);
}

uint32_t *internal_node_num_keys(void *node)
{
  return reinterpret_cast<uint32_t *>(
      (reinterpret_cast<uint8_t *>(node) + INTERNAL_NODE_NUM_KEYS_OFFSET));
}

uint32_t *internal_node_right_child(void *node)
{
  return reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(node) +
                                      INTERNAL_NODE_RIGHT_CHILD_OFFSET);
}

uint32_t *internal_node_cell(void *node, uint32_t cell_num)
{
  return reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(node) +
                                      INTERNAL_NODE_HEADER_SIZE +
                                      cell_num * INTERNAL_NODE_CELL_SIZE);
}

uint32_t *internal_node_child(void *node, uint32_t child_num)
{
  uint32_t num_keys = *internal_node_num_keys(node);
  if (child_num > num_keys)
  {
    std::cerr << std::format("Tried to access child_num {} > {}\n", child_num,
                             num_keys);
    exit(1);
  }
  else if (child_num == num_keys)
    return internal_node_right_child(node);
  else
    return internal_node_cell(node, child_num);
}
uint32_t *internal_node_key(void *node, uint32_t key_num)
{
  void *cell_location = internal_node_cell(node, key_num);
  return reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(cell_location) +
                                      INTERNAL_NODE_CHILD_SIZE);
}

uint32_t get_node_max_key(void *node)
{
  switch (get_node_type(node))
  {
  case NodeType::INTERNAL:
    return *internal_node_key(node, *internal_node_num_keys(node) - 1);
  case NodeType::LEAF:
    return *(reinterpret_cast<uint32_t *>(
        leaf_node_key(node, *leaf_node_num_cells(node) - 1)));
  }

  __builtin_unreachable();
}

bool is_node_root(void *node)
{
  uint8_t value = *(reinterpret_cast<uint8_t *>(node) + IS_ROOT_OFFSET);
  return static_cast<bool>(value);
}

void set_node_root(void *node, bool is_root)
{
  uint8_t value                                         = is_root;
  *(reinterpret_cast<uint8_t *>(node) + IS_ROOT_OFFSET) = value;
}

void initialize_internal_node(void *node)
{
  set_node_type(node, NodeType::INTERNAL);
  set_node_root(node, false);
  *internal_node_num_keys(node) = 0;
}

uint32_t *leaf_node_next_leaf(void *node)
{
  return reinterpret_cast<uint32_t *>(
      (reinterpret_cast<uint8_t *>(node) + LEAF_NODE_NEXT_LEAF_OFFSET));
}

uint32_t *node_parent(void *node)
{
  return reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(node) +
                                      PARENT_POINTER_OFFSET);
}

void internal_node_insert(Table *table, uint32_t parent_page_num,
                          uint32_t child_page_num)
{
  void *parent           = table->pager->get_page(parent_page_num);
  void *child            = table->pager->get_page(child_page_num);

  uint32_t child_max_key = get_node_max_key(child);
  uint32_t index         = internal_node_find_child(parent, child_max_key);
  uint32_t orig_num_keys = *internal_node_num_keys(parent);
  *internal_node_num_keys(parent) = orig_num_keys + 1;

  if (orig_num_keys >= INTERNAL_NODE_MAX_CELLS)
  {
    std::cerr << "Unimplemented: Splitting internal nodes\n";
    std::exit(1);
  }

  uint32_t right_child_page_num = *internal_node_right_child(parent);
  void *right_child             = table->pager->get_page(right_child_page_num);

  if (child_max_key > get_node_max_key(right_child))
  {
    *internal_node_child(parent, orig_num_keys) = right_child_page_num;
    *internal_node_key(parent, orig_num_keys)   = get_node_max_key(right_child);
    *internal_node_right_child(parent)          = child_page_num;
  }
  else
  {
    for (uint32_t i = orig_num_keys; i > index; i--)
    {
      void *destination = internal_node_cell(parent, i);
      void *source      = internal_node_cell(parent, i - 1);
      memcpy(destination, source, INTERNAL_NODE_CELL_SIZE);
    }
    *internal_node_child(parent, index) = child_page_num;
    *internal_node_key(parent, index)   = child_max_key;
  }
}

void update_internal_node_key(void *node, uint32_t old_key, uint32_t new_key)
{
  uint32_t old_child_index = internal_node_find_child(node, old_key);
  *internal_node_key(node, old_child_index) = new_key;
}

uint32_t internal_node_find_child(void *node, uint32_t key)
{
  uint32_t num_keys  = *Node::internal_node_num_keys(node);

  uint32_t min_index = 0;
  uint32_t max_index = num_keys;

  while (min_index != max_index)
  {
    uint32_t index        = (min_index + max_index) / 2;
    uint32_t key_to_right = *Node::internal_node_key(node, index);
    if (key_to_right >= key)
      max_index = index;
    else
      min_index = index + 1;
  }

  return min_index;
}

} // namespace Node
