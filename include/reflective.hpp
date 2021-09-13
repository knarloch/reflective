#ifndef REFLECTIVE_REFLECTIVE_HPP
#define REFLECTIVE_REFLECTIVE_HPP

#include <tuple>
#include <type_traits>
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
using std::tuple_element;
using std::tuple_size;
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
  auto toTuple()&& { return reflective::toTupleOfMembers(reflective::import::move(*this), __VA_ARGS__); }                                  \
  static constexpr bool hasToTuple() { return true; };

template<size_t I = 0, typename Context, typename... Ts>
typename enable_if<I == sizeof...(Ts), Context>::type
forEachTupleMember(Context context, tuple<Ts...> t)
{
  return context;
}

template<typename, typename = void>
struct HasToTupleMethod : std::false_type
{};
template<typename T>
struct HasToTupleMethod<T, std::void_t<decltype(T::hasToTuple)>> : std::is_convertible<decltype(T::hasToTuple), bool (*)()>
{};

template<size_t I, typename Context, typename... Ts>
typename enable_if<!HasToTupleMethod<typename tuple_element<I, tuple<Ts...>>::type::ValueType>::value, Context>::type
forTupleMember(Context context, tuple<Ts...> t)
{
  context.applyMember(get<I>(t));
  return context;
}

template<size_t I, typename Context, typename... Ts>
typename enable_if<HasToTupleMethod<typename tuple_element<I, tuple<Ts...>>::type::ValueType>::value, Context>::type
forTupleMember(Context context, tuple<Ts...> t)
{
  context.applyStruct(get<I>(t).getMemberName(), forEachMember(Context{}, get<I>(t).getValue()));
  return context;
}

template<size_t I = 0, typename Context, typename... Ts>
typename enable_if<(I < sizeof...(Ts)), Context>::type
forEachTupleMember(Context context, tuple<Ts...> t)
{
  context =  forTupleMember<I>(context, t);
  return forEachTupleMember<I + 1>(move(context), t);
}

template<typename Context, typename StructT>
Context
forEachMember(Context context, StructT s)
{
  return forEachTupleMember(context, s.toTuple());
}

} // namespace reflective

#endif // REFLECTIVE_REFLECTIVE_HPP
