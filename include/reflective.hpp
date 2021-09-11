#include <utility>

namespace reflective {

using std::forward;
using std::move;

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

  const T& Value() const { return value; }
};

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define DECLARE_FIELD(type, name, defaultValue)                                                                                            \
  struct name##Tag                                                                                                                         \
  {                                                                                                                                        \
    constexpr static char const* GetFieldName() { return TOSTRING(name); };                                                                \
  };                                                                                                                                       \
  using name##_t = struct reflective::Field<type, name##Tag>;                                                                              \
  name##_t name{ defaultValue };

} // namespace reflective