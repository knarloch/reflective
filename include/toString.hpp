#ifndef REFLECTIVE_TOSTRING_HPP
#define REFLECTIVE_TOSTRING_HPP

#include "reflective.hpp"
#include <sstream>

namespace reflective {

template<typename MemberT>
std::string
memberToSting(const MemberT& member)
{
  std::stringstream ss;
  ss << "\"" << member.getMemberName() << "\": " << member.getValue() << ", ";
  return ss.str();
}

template<typename Struct>
std::string
toString(const Struct& s)
{
  std::stringstream ss;

  forEachMember([&](const auto& m) { ss << memberToSting(m); }, s);
  return ss.str();
}

}

#endif // REFLECTIVE_TOSTRING_HPP
