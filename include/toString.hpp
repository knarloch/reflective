#ifndef REFLECTIVE_TOSTRING_HPP
#define REFLECTIVE_TOSTRING_HPP

#include "reflective.hpp"
#include <sstream>

namespace reflective {

struct ToTreelikeMultilineStringTransformation
{
  std::string state;

  ToTreelikeMultilineStringTransformation() = default;

  template<typename ValueT>
  explicit ToTreelikeMultilineStringTransformation(const ValueT& value)
  {
    std::stringstream ss;
    ss << value;
    state = move(ss).str();
  }

  static std::string addIndentation(std::string s)
  {
    s.insert(0 , 1u, '\n');
    for (auto pos = 0; (pos = s.find('\n', pos)) < std::string::npos;) {
      s.insert(pos+1 , 2u, ' ');
      pos+=1;
    }
    return s;
  }

  template<typename MemberT>
  void applyMember(MemberT&& member, ToTreelikeMultilineStringTransformation context)
  {
    std::stringstream ss;
    ss << member.getMemberName() << ':' << addIndentation(move(context.state)) << '\n';
    state.append(ss.str());
  }

  template<typename MemberT>
  void applyMember(MemberT&& member, vector<ToTreelikeMultilineStringTransformation> contexts)
  {
    std::stringstream ss;
    ss << member.getMemberName() << ':';
    for (auto& context : contexts) {
      ss << addIndentation(move(context.state)) << '\n';
    }
    state.append(ss.str());
  }
};

template<typename Struct>
std::string
toTreelikeMultilineString(const Struct& s)
{
  return ToTreelikeMultilineStringTransformation::addIndentation(move(forEachMember<ToTreelikeMultilineStringTransformation>(s).state));
}

}

#endif // REFLECTIVE_TOSTRING_HPP
