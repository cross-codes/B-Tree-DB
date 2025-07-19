#include "meta.hh"

namespace meta
{
auto process_meta_cmd(std::string input, Table &table) -> MetaCommandResult
{
  if (input == ".exit")
  {
    table.close_proc();
    exit(0);
  }
  else
    return MetaCommandResult::META_COMMAND_UNRECOGNIZED_COMMAND;
}
} // namespace meta
