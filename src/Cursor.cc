#include "Cursor.hh"
#include "Node.hh"
#include <iostream>

Cursor::Cursor(Table *table, bool end) : table{table}
{
  if (!end)
  {
    page_num           = table->root_page_num;
    cell_num           = 0;
    void *root_node    = table->pager->get_page(table->root_page_num);
    uint32_t num_cells = *(Node::leaf_node_num_cells(root_node));
    eot                = (num_cells == 0);
  }
  else
  {
    page_num           = table->root_page_num;
    void *root_node    = table->pager->get_page(table->root_page_num);
    uint32_t num_cells = *(Node::leaf_node_num_cells(root_node));
    cell_num           = num_cells;
    eot                = true;
  }
}

Cursor::Cursor(Table *table, uint32_t key) : table{table}
{
  this->page_num = table->root_page_num;
  void *root     = table->pager->get_page(table->root_page_num);

  if (Node::get_node_type(root) == Node::NodeType::LEAF)
  {
    void *node                  = root;
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
  }
  else
  {
    std::cerr << "Unimplemented feature\n";
    std::exit(1);
  }
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
    eot = true;
}
