#ifndef REFLECTIVE_TOSTRING_HPP
#define REFLECTIVE_TOSTRING_HPP

#include "reflective.hpp"
#include <sstream>

namespace reflective {

struct ToStringContext
{
  std::string state;

  template<typename MemberT>
  void applyMember(MemberT&& member)
  {
    std::stringstream ss;
    ss << "\"" << member.getMemberName() << "\": " << forward<MemberT>(member).getValue() << ", ";
    state.append(ss.str());
  }
  void applyStruct(const char* memberName, ToStringContext toStringContext)
  {
    state.append("\"");
    state.append(memberName);
    state.append("\": { ");
    state.append(move(toStringContext.state));
    state.append(" }, ");
  }
};

template<typename Struct>
std::string
toString(const Struct& s)
{
  return "{ " + move(forEachMember(ToStringContext{}, s).state) + " }";
}

}

#endif // REFLECTIVE_TOSTRING_HPP
