#include "meta.hh"

namespace meta
{
auto process_meta_cmd(std::string input) -> MetaCommandResult
{
  if (input == ".exit")
    exit(0);
  else
    return MetaCommandResult::META_COMMAND_UNRECOGNIZED_COMMAND;
}
} // namespace meta
