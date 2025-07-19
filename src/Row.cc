#include "Row.hh"
#include <cstring>
#include <format>
#include <iostream>

Row::Row(uint32_t id, const std::string &username, const std::string &email)
    : id{id}
{
  if (username.size() > COLUMN_USERNAME_SIZE)
    this->truncated = true;

  if (email.size() > COLUMN_EMAIL_SIZE)
    this->truncated = true;

  std::strncpy(this->username, username.c_str(), sizeof(this->username) - 1);
  this->username[sizeof(this->username) - 1] = '\0';

  std::strncpy(this->email, email.c_str(), sizeof(this->email) - 1);
  this->email[sizeof(this->email) - 1] = '\0';
}

void Row::serialize_into_dest(void *destination)
{
  auto dest = reinterpret_cast<char *>(destination);
  std::memcpy(dest + ID_OFFSET, &id, ID_SIZE);
  std::memcpy(dest + USERNAME_OFFSET, username, USERNAME_SIZE);
  std::memcpy(dest + EMAIL_OFFSET, email, EMAIL_SIZE);
}

void Row::deserialize_into_row(void *source, Row *destination)
{
  auto src = reinterpret_cast<char *>(source);
  std::memcpy(&(destination->id), src + ID_OFFSET, ID_SIZE);
  std::memcpy(&(destination->username), src + USERNAME_OFFSET, USERNAME_SIZE);
  std::memcpy(&(destination->email), src + EMAIL_OFFSET, EMAIL_SIZE);
}

void Row::console_display()
{
  std::cout << std::format("({}, {}, {})\n", id, username, email);
}
