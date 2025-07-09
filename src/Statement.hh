#pragma once

#include "Table.hh"
#include <string>

enum class StatementType
{
  STATEMENT_INSERT,
  STATEMENT_SELECT,
  STATEMENT_INVALID
};

class Statement
{
protected:
  StatementType type_;
  std::string raw_input_;

public:
  Statement(std::string input_buffer);
  virtual ~Statement() = default;

  auto get_type() -> StatementType;
  virtual auto execute(Table &table) -> int = 0;
};
