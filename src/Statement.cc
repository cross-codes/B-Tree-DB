#include "Statement.hh"

Statement::Statement(std::string input_buffer)
{
  raw_input_ = input_buffer;

  if (input_buffer.substr(0, 6) == "insert")
    type_ = StatementType::STATEMENT_INSERT;
  else if (input_buffer.substr(0, 6) == "select")
    type_ = StatementType::STATEMENT_SELECT;
  else
    type_ = StatementType::STATEMENT_INVALID;
}

auto Statement::get_type() -> StatementType
{
  return type_;
}
