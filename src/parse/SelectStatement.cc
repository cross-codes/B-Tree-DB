#include "SelectStatement.hh"

SelectStatement::SelectStatement(std::string input) : Statement(input) {};

auto SelectStatement::execute(Table &table) -> int
{
  Row row(-1, "", "");

  for (uint32_t i = 0; i < table.num_rows; i++)
  {
    Row::deserialize_into_row(table.row_slot(i), &row);
    row.console_display();
  }

  return 0;
}
