#ifndef REFLECTIVE_NLOHMANNJSONTRANSFORMATION_HPP
#define REFLECTIVE_NLOHMANNJSONTRANSFORMATION_HPP
#include "nlohmann/json.hpp"
#include "reflective.hpp"

namespace reflective {

namespace detailNlohmannJson {
struct ToTransformation
{

  nlohmann::json transformed;

  ToTransformation() = default;

  template<typename ValueT>
  explicit ToTransformation(const ValueT& value)
  {
    transformed = value;
  }

  template<typename MemberT>
  void applyMember(MemberT&& member, ToTransformation alreadyTransformed)
  {
    transformed[member.getMemberName()] = alreadyTransformed.transformed;
  }

  template<typename MemberT>
  void applyMember(MemberT&& member, vector<ToTransformation> alreadyTransformed)
  {
    std::size_t idx = 0;
    for (auto& c : alreadyTransformed)
      transformed[member.getMemberName()][idx++] = move(c.transformed);
  }
};

template<typename ReflectiveStruct>
struct FromTransformation
{

  nlohmann::json transformed;

  FromTransformation() = default;

  template<typename ValueT>
  explicit FromTransformation(const ValueT& value)
  {
    transformed = value;
  }

  template<typename MemberT>
  void applyMember(MemberT&& member, FromTransformation alreadyTransformed)
  {
    transformed[member.getMemberName()] = alreadyTransformed.transformed;
  }

  template<typename MemberT>
  void applyMember(MemberT&& member, vector<FromTransformation> alreadyTransformed)
  {
    std::size_t idx = 0;
    for (auto& c : alreadyTransformed)
      transformed[member.getMemberName()][idx++] = move(c.transformed);
  }
};
}

template<typename ReflectiveStruct>
nlohmann::json
toJson(ReflectiveStruct&& s)
{
  return reflective::forEachMember<detailNlohmannJson::ToTransformation>(forward<ReflectiveStruct>(s)).transformed;
}

template<typename ReflectiveStruct, typename NlohmannJson>
ReflectiveStruct toReflectiveStruct(NlohmannJson&& j)
{
  return reflective::forEachMember<detailNlohmannJson::FromTransformation>(ReflectiveStruct{});
}

}

#endif // REFLECTIVE_NLOHMANNJSONTRANSFORMATION_HPP
