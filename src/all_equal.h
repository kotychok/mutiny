# pragma once

#include <utility>

// https://stackoverflow.com/a/15208908
  template<typename T>
bool all_equal(T&& t, T&& u)
{
  return (t == u);
}

  template<typename T, typename... Ts>
bool all_equal(T&& t, T&& u, Ts&&... args)
{
  return (t == u) && all_equal(u, std::forward<Ts>(args)...);
}
