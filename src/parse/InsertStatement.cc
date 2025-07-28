#include "InsertStatement.hh"
#include "../Node.hh"
#include <iostream>
#include <sstream>

InsertStatement::InsertStatement(std::string input) : Statement(input)
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
  Cursor cursor(&table, true);

  if (row.truncated)
  {
    std::cerr << "Error, entered string is too long.\n";
    return -1;
  }

  Node::leaf_node_insert(cursor, ID_, row);

  return 0;
}
