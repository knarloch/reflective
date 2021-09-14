#ifndef REFLECTIVE_TOSTRING_HPP
#define REFLECTIVE_TOSTRING_HPP

#include "reflective.hpp"
#include <sstream>

namespace reflective {

struct ToStringContext
{
  std::string state;

  ToStringContext() = default;

  template<typename ValueT>
  explicit ToStringContext(const ValueT& value)
  {
    std::stringstream ss;
    ss << value;
    state = move(ss).str();
  }

  template<typename MemberT>
  void applyMember(MemberT&& member, ToStringContext context)
  {
    std::stringstream ss;
    ss << "\"" << member.getMemberName() << "\": " << move(context.state) << ", ";
    state.append(ss.str());
  }

  template<typename MemberT>
  void applyMember(MemberT&& member, vector<ToStringContext> contexts)
  {
    std::stringstream ss;
    ss << "\"" << member.getMemberName() << "\": ";
    for (auto& context : contexts) {
      ss << move(context.state) << ", ";
    }
    state.append(ss.str());
  }
};

template<typename Struct>
std::string
toString(const Struct& s)
{
  return "{ " + move(forEachMember<ToStringContext>(s).state) + " }";
}

}

#endif // REFLECTIVE_TOSTRING_HPP
