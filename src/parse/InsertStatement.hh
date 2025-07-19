#pragma once

#include "../Statement.hh"

class InsertStatement : public Statement
{
private:
  int ID_;
  std::string username_, email_;
  bool error_;

public:
  InsertStatement(std::string input);
  auto execute(Table &table) -> int override;
};
