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
struct Field : public Tag
{
  using value_type = T;
  T value;

  Field(T tt)
    : value{ move(tt) }
  {}

  T& operator()() { return value; }

  const T& operator()() const { return value; }

  // cast operators to enable static_cast<T>()
  operator T&() { return value; }

  operator const T&() const { return value; }

  // Another possible interfaces (draft):
  template<typename TT>
  void SetValue(TT& tt)
  {
    value = forward<TT>(tt);
  }
  auto toTuple() const { return make_tuple(*this); }

  const T& Value() const { return value; }
};

template<typename Struct, typename... FieldsT>
auto
toTupleOfMembers(const Struct& s, FieldsT&&... fields)
{
  return make_tuple(static_cast<remove_cv_t<remove_reference_t<FieldsT>>>(s)...);
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define DECLARE_FIELD(type, name, defaultValue)                                                                                            \
  struct name##Tag                                                                                                                         \
  {                                                                                                                                        \
    constexpr static char const* fieldName{ TOSTRING(name) };                                                                              \
    constexpr static char const* GetFieldName() { return fieldName; };                                                                     \
  };                                                                                                                                       \
  using name##_t = struct reflective::Field<type, name##Tag>;                                                                              \
  name##_t name{ defaultValue };                                                                                                           \
  explicit operator name##_t() const { return name; }

#define ADD_TUPLE_CONVERSION(...)                                                                                                          \
  auto toTuple() const { return reflective::toTupleOfMembers(*this, __VA_ARGS__); }

} // namespace reflective
