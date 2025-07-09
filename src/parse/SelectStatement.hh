#pragma once

#include "../Statement.hh"

class SelectStatement : public Statement
{
public:
  SelectStatement(std::string input);
  auto execute(Table &table) -> int override;
};
