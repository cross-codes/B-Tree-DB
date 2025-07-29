#include "Cursor.hh"
#include "Node.hh"

Cursor::Cursor(Table *table) : table{table}
{
  this->page_num      = table->root_page_num;
  void *root          = table->pager->get_page(table->root_page_num);

  auto leaf_node_find = [this](void *node, uint32_t key) -> void {
    uint32_t num_cells          = *(Node::leaf_node_num_cells(node));

    uint32_t min_index          = 0;
    uint32_t one_past_max_index = num_cells;
    while (one_past_max_index != min_index)
    {
      uint32_t index = (min_index + one_past_max_index) / 2;
      uint32_t key_at_index =
          *reinterpret_cast<uint32_t *>((Node::leaf_node_key(node, index)));
      if (key == key_at_index)
      {
        this->cell_num = index;
        return;
      }

      if (key < key_at_index)
        one_past_max_index = index;
      else
        min_index = index + 1;
    }

    cell_num = min_index;
  };

  auto internal_node_find_recc = [this, &leaf_node_find,
                                  &table](auto &&internal_node_find_recc,
                                          uint32_t key) {
    void *node         = table->pager->get_page(page_num);
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

    uint32_t child_num = *Node::internal_node_child(node, min_index);
    void *child        = table->pager->get_page(child_num);
    switch (Node::get_node_type(child))
    {
    case Node::NodeType::LEAF: {
      this->page_num = child_num;
      return leaf_node_find(child, key);
    }
    case Node::NodeType::INTERNAL: {
      this->page_num = child_num;
      return internal_node_find_recc(internal_node_find_recc, key);
    }
    }
  };

  if (Node::get_node_type(root) == Node::NodeType::LEAF)
    leaf_node_find(root, 0);
  else
    internal_node_find_recc(internal_node_find_recc, 0);

  void *node         = table->pager->get_page(page_num);
  uint32_t num_cells = *Node::leaf_node_num_cells(node);
  this->eot          = (num_cells == 0);
}

Cursor::Cursor(Table *table, uint32_t key) : table{table}
{
  this->page_num      = table->root_page_num;
  void *root          = table->pager->get_page(table->root_page_num);

  auto leaf_node_find = [this](void *node, uint32_t key) -> void {
    uint32_t num_cells          = *(Node::leaf_node_num_cells(node));

    uint32_t min_index          = 0;
    uint32_t one_past_max_index = num_cells;
    while (one_past_max_index != min_index)
    {
      uint32_t index = (min_index + one_past_max_index) / 2;
      uint32_t key_at_index =
          *reinterpret_cast<uint32_t *>((Node::leaf_node_key(node, index)));
      if (key == key_at_index)
      {
        this->cell_num = index;
        return;
      }

      if (key < key_at_index)
        one_past_max_index = index;
      else
        min_index = index + 1;
    }

    cell_num = min_index;
  };

  auto internal_node_find_recc = [this, &leaf_node_find,
                                  &table](auto &&internal_node_find_recc,
                                          uint32_t key) {
    void *node         = table->pager->get_page(page_num);
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

    uint32_t child_num = *Node::internal_node_child(node, min_index);
    void *child        = table->pager->get_page(child_num);
    switch (Node::get_node_type(child))
    {
    case Node::NodeType::LEAF: {
      this->page_num = child_num;
      return leaf_node_find(child, key);
    }
    case Node::NodeType::INTERNAL: {
      this->page_num = child_num;
      return internal_node_find_recc(internal_node_find_recc, key);
    }
    }
  };

  if (Node::get_node_type(root) == Node::NodeType::LEAF)
    leaf_node_find(root, key);
  else
    internal_node_find_recc(internal_node_find_recc, key);
}

auto Cursor::get_value() -> void *
{
  char *page = reinterpret_cast<char *>(table->pager->get_page(page_num));
  return Node::leaf_node_value(page, static_cast<uint32_t>(cell_num));
};

void Cursor::advance()
{
  void *node = table->pager->get_page(page_num);
  cell_num += 1;
  if (cell_num >= (*(Node::leaf_node_num_cells(node))))
  {
    uint32_t next_page_num = *Node::leaf_node_next_leaf(node);
    if (next_page_num == 0)
      eot = true;
    else
    {
      page_num = next_page_num;
      cell_num = 0;
    }
  }
}
