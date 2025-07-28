#include "Cursor.hh"
#include "Node.hh"

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
