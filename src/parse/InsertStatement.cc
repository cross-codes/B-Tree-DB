#include "InsertStatement.hh"
#include "../Node.hh"
#include <iostream>
#include <sstream>

InsertStatement::InsertStatement(std::string input)
    : Statement(input), error_{false}
{
  std::istringstream iss(input.substr(6, std::string::npos));
  iss >> ID_ >> username_ >> email_;
  if (iss.fail() || ID_ < 0)
    error_ = true;
};

auto InsertStatement::execute(Table &table) -> int
{
  if (error_)
  {
    std::cerr << "Error, unable to parse statement.\n";
    return -1;
  }

  Row row(ID_, username_, email_);

  if (row.truncated)
  {
    std::cerr << "Error, entered string is too long.\n";
    return -1;
  }

  void *node             = table.pager->get_page(table.root_page_num);
  uint32_t num_cells     = *(Node::leaf_node_num_cells(node));
  uint32_t key_to_insert = ID_;
  Cursor cursor(&table, key_to_insert);

  if (cursor.cell_num < num_cells)
  {
    uint32_t key_at_idx = *reinterpret_cast<uint32_t *>(
        (Node::leaf_node_key(node, static_cast<uint32_t>(cursor.cell_num))));
    if (key_at_idx == key_to_insert)
    {
      std::cout << "Error: Duplicate key exists\n";
      return -1;
    }
  }

  Node::leaf_node_insert(cursor, ID_, row);

  return 0;
}
