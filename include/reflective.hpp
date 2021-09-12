#include <tuple>
#include <utility>

namespace reflective {

namespace import {
using std::forward;
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
  auto toTuple() const { return make_tuple(*this); }

  const ValueType& getValue() const { return value; }
};

template<typename Struct, typename... MemberTs>
auto
toTupleOfMembers(const Struct& s, MemberTs&&...)
{
  return make_tuple(static_cast<remove_cv_t<remove_reference_t<MemberTs>>>(s)...);
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define DEFINE_MEMBER(type, name, defaultValue)                                                                                            \
  struct name##Tag                                                                                                                         \
  {                                                                                                                                        \
    constexpr static char const* fieldName{ TOSTRING(name) };                                                                              \
    constexpr static char const* getFieldName() { return fieldName; };                                                                     \
  };                                                                                                                                       \
  using name##_t = struct reflective::Member<type, name##Tag>;                                                                             \
  name##_t name{ defaultValue };                                                                                                           \
  explicit operator name##_t() const { return name; }

#define DEFINE_TO_TUPLE(...)                                                                                                               \
  auto toTuple() const { return reflective::toTupleOfMembers(*this, __VA_ARGS__); }

} // namespace reflective
