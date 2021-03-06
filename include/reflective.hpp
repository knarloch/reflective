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
using std::remove_cv;
using std::remove_cv_t;
using std::remove_reference;
using std::remove_reference_t;
using std::true_type;
using std::tuple;
using std::tuple_element;
using std::tuple_size;
using std::vector;
using std::void_t;
}
using namespace reflective::import;

#if 201703L > __cplusplus
template<typename T>
remove_cvref_t = typename remove_cv<typename remove_reference<T>::type>::type;
#else
namespace import {
using std::remove_cvref_t;
}
#endif

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

  Member() = default;

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

template<typename... MemberTs>
auto
toTupleOfVariables(MemberTs&&...)
{
  return tuple<remove_cvref_t<MemberTs>...>();
}

template<typename Struct, typename... MemberTs>
auto
toTupleOfMembers(Struct&& s, MemberTs&&...)
{
  return make_tuple(static_cast<remove_cvref_t<MemberTs>>(forward<Struct>(s))...);
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
  using TupleType = decltype(toTupleOfVariables(__VA_ARGS__));                                                                             \
  auto toTuple() const& { return reflective::toTupleOfMembers(*this, __VA_ARGS__); }                                                       \
  auto toTuple()&& { return reflective::toTupleOfMembers(reflective::import::move(*this), __VA_ARGS__); }                                  \
  void setFromTuple(TupleType t){};

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
struct HasToTupleMethod<T, void_t<typename T::TupleType>> : true_type
{};

template<typename, typename = void>
struct HasIterator : false_type
{};
template<typename T>
struct HasIterator<T, void_t<typename T::iterator>> : true_type
{};

template<size_t I, typename... Ts>
using IthType = typename tuple_element<I, tuple<Ts...>>::type;

template<typename TransformationTypeForDifferentValueType, typename NewValueType>
using TransformationWithSwappedType = typename TransformationTypeForDifferentValueType::template TrasformationType<NewValueType>;

template<typename TransformationOfMemberValue, typename MemberT>
using TrasformationOfMemberT = TransformationWithSwappedType<TransformationOfMemberValue, typename remove_cvref_t<MemberT>::ValueType>;

template<typename Context, typename MemberValueT>
typename enable_if<!HasToTupleMethod<MemberValueT>::value, TransformationWithSwappedType<Context, MemberValueT>>::type
forTupleMember_conditionHasToTuple(Context&& c, const MemberValueT& memberValue)
{
  return TransformationWithSwappedType<Context, MemberValueT>{ memberValue };
}

template<typename Context, typename StructT>
Context
forEachMember(Context&& c, StructT&& s);

template<typename Context, typename MemberValueT>
typename enable_if<HasToTupleMethod<MemberValueT>::value, TransformationWithSwappedType<Context, MemberValueT>>::type
forTupleMember_conditionHasToTuple(Context&& c, const MemberValueT& memberValue)
{
  return forEachMember(TransformationWithSwappedType<Context, MemberValueT>{}, memberValue);
}

template<size_t I, typename Context, typename... Ts>
typename enable_if<IthType<I, Ts...>::containerType == ContainerType::None, Context>::type
forTupleMember_conditionContainerSize(Context&& c, const tuple<Ts...>& t)
{
  c.applyMember(get<I>(t), forTupleMember_conditionHasToTuple(forward<Context>(c), get<I>(t).getValue()));
  return c;
}

template<size_t I, typename Context, typename... Ts>
typename enable_if<IthType<I, Ts...>::containerType == ContainerType::Vector, Context>::type
forTupleMember_conditionContainerSize(Context&& c, const tuple<Ts...>& t)
{
  using MemebeT = IthType<I, Ts...>;
  using Trans = TrasformationOfMemberT<Context, MemebeT>;
  vector<Trans> contexts;
  for (const auto& val : get<I>(t).getValue()) {
    contexts.emplace_back(forTupleMember_conditionHasToTuple(forward<Context>(c), val).transformed);
  }
  c.applyMember(get<I>(t), move(contexts));
  return c;
}

template<size_t I = 0, typename Context, typename... Ts>
typename enable_if<(I < sizeof...(Ts)), Context>::type
forEachTupleMember(Context&& c, const tuple<Ts...>& t)
{
  return forEachTupleMember<I + 1>(forTupleMember_conditionContainerSize<I>(forward<Context>(c), t), t);
}

template<typename Context, typename StructT>
Context
forEachMember(Context&& c, StructT&& s)
{
   c.transformed = forEachTupleMember(TransformationWithSwappedType<Context, typename remove_cvref_t<StructT>::TupleType>{}, forward<StructT>(s).toTuple()).transformed;
   return c;
}

} // namespace reflective

#endif // REFLECTIVE_REFLECTIVE_HPP
