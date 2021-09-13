#ifndef REFLECTIVE_REFLECTIVE_HPP
#define REFLECTIVE_REFLECTIVE_HPP

#include <tuple>
#include <utility>

namespace reflective {

namespace import {
using std::enable_if;
using std::forward;
using std::get;
using std::make_tuple;
using std::move;
using std::remove_cv_t;
using std::remove_reference_t;
using std::tuple;
}
using namespace reflective::import;

template<typename T, typename Tag>
struct Member : public Tag
{
  using ValueType = T;
  ValueType value;

  Member(ValueType tt)
    : value{ move(tt) }
  {}

  ValueType& operator()() { return value; }

  const ValueType& operator()() const { return value; }

  // cast operators to enable static_cast<ValueType>()
  operator ValueType&() { return value; }

  operator const ValueType&() const { return value; }

  // Another possible interfaces (draft):
  template<typename TT>
  void setValue(TT& tt)
  {
    value = forward<TT>(tt);
  }
  auto toTuple() const& { return make_tuple(*this); }
  auto toTuple() && { return make_tuple(move(*this)); }

  const ValueType& getValue() const { return value; }
};

template<typename Struct, typename... MemberTs>
auto
toTupleOfMembers(Struct&& s, MemberTs&&...)
{
  return make_tuple(static_cast<remove_cv_t<remove_reference_t<MemberTs>>>(forward<Struct>(s))...);
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define DEFINE_MEMBER(type, name, defaultValue)                                                                                            \
  struct name##Tag                                                                                                                         \
  {                                                                                                                                        \
    constexpr static char const* memberName{ TOSTRING(name) };                                                                             \
    constexpr static char const* getMemberName() { return memberName; };                                                                   \
  };                                                                                                                                       \
  using name##_t = struct reflective::Member<type, name##Tag>;                                                                             \
  name##_t name{ defaultValue };                                                                                                           \
  explicit operator name##_t() const& { return name; }                                                                                     \
  explicit operator name##_t()&& { return reflective::import::move(name); }

#define DEFINE_TO_TUPLE(...)                                                                                                               \
  auto toTuple() const& { return reflective::toTupleOfMembers(*this, __VA_ARGS__); }                                                       \
  auto toTuple()&& { return reflective::toTupleOfMembers(reflective::import::move(*this), __VA_ARGS__); }

template<size_t I = 0, typename Callback, typename... Ts>
typename enable_if<I == sizeof...(Ts), void>::type
forEachTupleMember(Callback c, tuple<Ts...> t)
{
  return;
}

template<size_t I = 0, typename Callback, typename... Ts>
typename enable_if<(I < sizeof...(Ts)), void>::type
forEachTupleMember(Callback c, tuple<Ts...> t)
{
  c(get<I>(t));
  forEachTupleMember<I + 1>(c, t);
}

template<typename Callback, typename StructT>
void
forEachMember(Callback c, StructT s)
{
  forEachTupleMember(c, s.toTuple());
}

} // namespace reflective

#endif // REFLECTIVE_REFLECTIVE_HPP
