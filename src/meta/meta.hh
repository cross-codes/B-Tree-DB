#pragma once

#include "../status/MetaCommandResult.hh"
#include <string>

namespace meta
{
auto process_meta_cmd(std::string input) -> MetaCommandResult;
} // namespace meta
