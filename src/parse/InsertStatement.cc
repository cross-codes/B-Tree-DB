#include "InsertStatement.hh"
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
    std::cout << "Error, unable to parse statement.\n";
    return -1;
  }

  Row row(ID_, username_, email_);

  if (row.truncated)
  {
    std::cout << "Error, entered string is too long.\n";
    return -1;
  }

  row.serialize_into_dest(table.row_slot(ID_));
  table.num_rows += 1;

  return 0;
}
