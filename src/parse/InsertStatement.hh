#pragma once

#include "../Statement.hh"
#include <cstdint>

class InsertStatement : public Statement
{
private:
  uint32_t ID_;
  std::string username_, email_;
  bool error_;

public:
  InsertStatement(std::string input);
  auto execute(Table &table) -> int override;
};
