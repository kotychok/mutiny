# pragma once

#include <tuple>
#include <ostream>

std::ostream& operator<<(std::ostream& o, const std::tuple<int, int, int> tuple)
{
  o << "(" << std::get<0>(tuple) << "," << std::get<1>(tuple) << "," << std::get<2>(tuple) << ")";
  return o;
}
