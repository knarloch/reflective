#ifndef REFLECTIVE_NLOHMANNJSONTRANSFORMATION_HPP
#define REFLECTIVE_NLOHMANNJSONTRANSFORMATION_HPP
#include "nlohmann/json.hpp"
#include "reflective.hpp"

namespace reflective {

namespace detailNlohmannJson {

template<typename = void>
struct ToTransformation
{

  template<typename unused>
  using TrasformationType = ToTransformation<unused>;

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

template<typename ValueT>
struct FromTransformation
{
  using Value = ValueT;
  Value transformed;

  template<typename OtherValueT>
  using TrasformationType = FromTransformation<OtherValueT>;

  FromTransformation() = default;

  template<typename Value>
  explicit FromTransformation(const Value& value)
  {
    transformed = value;
  }

  template<typename MemberTT>
  void applyMember(MemberTT&& member, FromTransformation alreadyTransformed)
  {
    get<MemberTT::ValueType>(transformed) = alreadyTransformed.transformed;
  }

  template<typename MemberTT>
  void applyMember(MemberTT&& member, vector<FromTransformation> alreadyTransformed)
  {
    get<MemberTT::ValueType>(transformed).clear();
    std::size_t idx = 0;
    for (auto& c : alreadyTransformed) {
      get<MemberTT::ValueType>(transformed).emplace_back(move(alreadyTransformed.transformed));
    }
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
ReflectiveStruct
toReflectiveStruct(NlohmannJson&& j)
{
  return reflective::forEachMember<detailNlohmannJson::FromTransformation>(ReflectiveStruct{});
}

}

#endif // REFLECTIVE_NLOHMANNJSONTRANSFORMATION_HPP
