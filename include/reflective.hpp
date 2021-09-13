#ifndef REFLECTIVE_REFLECTIVE_HPP
#define REFLECTIVE_REFLECTIVE_HPP

#include <tuple>
#include <type_traits>
#include <utility>

namespace reflective {

namespace import {
using std::enable_if;
using std::false_type;
using std::forward;
using std::get;
using std::make_tuple;
using std::move;
using std::remove_cv_t;
using std::remove_reference_t;
using std::true_type;
using std::tuple;
using std::tuple_element;
using std::tuple_size;
using std::void_t;
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
  const ValueType& getValue() const& { return value; }
  ValueType getValue() && { return move(value); }
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
  using hasToTuple = reflective::import::true_type;

template<size_t I = 0, typename Context, typename... Ts>
typename enable_if<I == sizeof...(Ts), Context>::type
forEachTupleMember(Context context, const tuple<Ts...>& t)
{
  return context;
}

template<typename, typename = void>
struct HasToTupleMethod : false_type
{};
template<typename T>
struct HasToTupleMethod<T, void_t<typename T::hasToTuple>> : true_type
{};

template<size_t I, typename... Ts>
using IthType = typename tuple_element<I, tuple<Ts...>>::type;

template<size_t I, typename Context, typename... Ts>
typename enable_if<!HasToTupleMethod<typename IthType<I, Ts...>::ValueType>::value, Context>::type
forTupleMember(Context context, const tuple<Ts...>& t)
{
  context.applyMember(get<I>(t));
  return context;
}

template<size_t I, typename Context, typename... Ts>
typename enable_if<HasToTupleMethod<typename IthType<I, Ts...>::ValueType>::value, Context>::type
forTupleMember(Context context, const tuple<Ts...>& t)
{
  using MemberType = IthType<I, Ts...>;
  context.applyStruct(MemberType::getMemberName(), forEachMember(Context{}, get<I>(t).getValue()));
  return context;
}

template<size_t I = 0, typename Context, typename... Ts>
typename enable_if<(I < sizeof...(Ts)), Context>::type
forEachTupleMember(Context context, const tuple<Ts...>& t)
{
  return forEachTupleMember<I + 1>(forTupleMember<I>(move(context), t), t);
}

template<typename Context, typename StructT>
Context
forEachMember(Context context, StructT&& s)
{
  return forEachTupleMember(move(context), forward<StructT>(s).toTuple());
}

} // namespace reflective

#endif // REFLECTIVE_REFLECTIVE_HPP
