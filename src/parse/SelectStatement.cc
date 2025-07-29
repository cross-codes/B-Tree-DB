#include "SelectStatement.hh"
#include "../Row.hh"

SelectStatement::SelectStatement(std::string input) : Statement(input) {};

auto SelectStatement::execute(Table &table) -> int
{
  Row row(-1, "", "");
  Cursor cursor(&table);

  while (!cursor.eot)
  {
    Row::deserialize_into_row(cursor.get_value(), &row);
    row.console_display();
    cursor.advance();
  }

  return 0;
}
