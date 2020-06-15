# pragma once

#include <array>
#include <iterator>
#include <ostream>
#include <vector>

// https://stackoverflow.com/a/19152438
  template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& o, const std::array<T, N>& arr)
{
  copy(arr.cbegin(), arr.cend(), std::ostream_iterator<T>(o, " "));
  return o;
}

// https://stackoverflow.com/a/19152438
  template <class T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& arr)
{
  copy(arr.cbegin(), arr.cend(), std::ostream_iterator<T>(o, ", "));
  return o;
}
