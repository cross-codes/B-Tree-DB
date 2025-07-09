#include <format>
#include <iostream>
#include <memory>
#include <string>

#include "Statement.hh"
#include "Table.hh"
#include "meta/meta.hh"
#include "parse/InsertStatement.hh"
#include "parse/SelectStatement.hh"

void display_prompt()
{
  std::cout << "db > ";
}

int main(int argc, char **argv)
{
  Table table;

  std::string input_buffer;
  while (true)
  {
    display_prompt();
    std::getline(std::cin, input_buffer);
    if (input_buffer.empty())
      continue;

    if (input_buffer[0] == '.')
    {
      auto code = meta::process_meta_cmd(input_buffer);
      switch (code)
      {
      case MetaCommandResult::META_COMMAND_SUCCESS:
        continue;

      case MetaCommandResult::META_COMMAND_UNRECOGNIZED_COMMAND:
        std::cout << std::format("Unrecognized command '{}'\n", input_buffer);
        continue;
      }
    }

    auto create_statement = [&input_buffer]() -> std::unique_ptr<Statement> {
      if (input_buffer.substr(0, 6) == "insert")
        return std::make_unique<InsertStatement>(input_buffer);

      if (input_buffer.substr(0, 6) == "select")
        return std::make_unique<SelectStatement>(input_buffer);

      return nullptr;
    };

    auto statement = create_statement();

    if (statement != nullptr)
      statement->execute(table);
  }
}
