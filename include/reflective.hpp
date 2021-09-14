#ifndef REFLECTIVE_REFLECTIVE_HPP
#define REFLECTIVE_REFLECTIVE_HPP

#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

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
using std::vector;
using std::void_t;
}
using namespace reflective::import;

enum class ContainerType
{
  None,
  Vector
};

template<typename T, typename Tag, ContainerType ct>
struct Member : public Tag
{
  using ValueType = T;
  ValueType value;

  static constexpr ContainerType containerType = ct;

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
  using name##_t = struct reflective::Member<type, name##Tag, reflective::ContainerType::None>;                                            \
  name##_t name{ defaultValue };                                                                                                           \
  explicit operator name##_t() const& { return name; }                                                                                     \
  explicit operator name##_t()&& { return reflective::import::move(name); }

#define DEFINE_TO_TUPLE(...)                                                                                                               \
  auto toTuple() const& { return reflective::toTupleOfMembers(*this, __VA_ARGS__); }                                                       \
  auto toTuple()&& { return reflective::toTupleOfMembers(reflective::import::move(*this), __VA_ARGS__); }                                  \
  using hasToTuple = reflective::import::true_type;

#define DEFINE_MEMBER_VECTOR(type, name, defaultValue)                                                                                     \
  struct name##Tag                                                                                                                         \
  {                                                                                                                                        \
    constexpr static char const* memberName{ TOSTRING(name) };                                                                             \
    constexpr static char const* getMemberName() { return memberName; };                                                                   \
  };                                                                                                                                       \
  using name##_t = struct reflective::Member<reflective::import::vector<type>, name##Tag, reflective::ContainerType::Vector>;              \
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

template<typename, typename = void>
struct HasIterator : false_type
{};
template<typename T>
struct HasIterator<T, void_t<typename T::iterator>> : true_type
{};

template<size_t I, typename... Ts>
using IthType = typename tuple_element<I, tuple<Ts...>>::type;

template<typename Context, typename MemberValueT>
typename enable_if<!HasToTupleMethod<MemberValueT>::value, Context>::type
forTupleMember_conditionHasToTuple(const MemberValueT& memberValue)
{
  return Context{ memberValue };
}

template<typename Context, typename MemberValueT>
typename enable_if<HasToTupleMethod<MemberValueT>::value, Context>::type
forTupleMember_conditionHasToTuple(const MemberValueT& memberValue)
{
  return forEachMember<Context>(memberValue);
}

template<size_t I, typename Context, typename... Ts>
typename enable_if<IthType<I, Ts...>::containerType == ContainerType::None, Context>::type
forTupleMember_conditionContainerSize(Context context, const tuple<Ts...>& t)
{
  context.applyMember(get<I>(t), forTupleMember_conditionHasToTuple<Context>(get<I>(t).getValue()));
  return context;
}

template<size_t I, typename Context, typename... Ts>
typename enable_if<IthType<I, Ts...>::containerType == ContainerType::Vector, Context>::type
forTupleMember_conditionContainerSize(Context context, const tuple<Ts...>& t)
{
  vector<Context> contexts;
  for (const auto& val : get<I>(t).getValue()) {
    contexts.emplace_back(forTupleMember_conditionHasToTuple<Context>(val));
  }
  context.applyMember(get<I>(t), move(contexts));
  return context;
}

template<size_t I = 0, typename Context, typename... Ts>
typename enable_if<(I < sizeof...(Ts)), Context>::type
forEachTupleMember(Context context, const tuple<Ts...>& t)
{
  return forEachTupleMember<I + 1>(forTupleMember_conditionContainerSize<I>(move(context), t), t);
}

template<typename Context, typename StructT>
Context
forEachMember(StructT&& s)
{
  return forEachTupleMember(Context{}, forward<StructT>(s).toTuple());
}

} // namespace reflective

#endif // REFLECTIVE_REFLECTIVE_HPP
