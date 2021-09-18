#ifndef REFLECTIVE_TRANSFORMATION_HPP
#define REFLECTIVE_TRANSFORMATION_HPP

// This file contains Transformation interface.
//
// This class is as empty transformation which does nothing, but may be used as example on how write your own
// Transformation class.
//
// This class is used as template parameter in reflective library. Because static polymorphism is used, virtual methods are not required.

namespace reflective {

template<typename maybeUnused = void>
struct Transformation
{

  template<typename maybeUnused_>
  using TrasformationType = Transformation<maybeUnused_>;
  // probably some members are required to hold transformed reflective structure.

  Transformation() = default;

  // A set of constructors matching every type of data used in DEFINE_MEMBER macros.
  // Constructors matching reflective structs (structs that have DEFINE_TO_TUPLE macro) are not required.
  //  Transformation class must be constructible from data types used in structure. During construction, value of member shall
  //  be transformed to implementation-defined internal representation.
  template<typename ValueT>
  explicit Transformation(const ValueT& value)
  {}

  // Transformation class must implement applyMember. The method applyMember accepts already constructed Transformation
  // of member value
  template<typename MemberT>
  void applyMember(MemberT&& member, TrasformationType<typename MemberT::ValueType> context)
  {}

  // Transformation class must implement applyMember. The method applyMember accepts already constructed container of Transformations
  // of values defined in DEFINE_MEMBER_VECTOR macro.
  // An override for each container template used in reflective structure that is meant to be used with this transformation is required.

  template<typename MemberT>
  void applyMember(MemberT&& member, vector<TrasformationType<typename MemberT::ValueType>> contexts)
  {}
};
}

#endif // REFLECTIVE_TRANSFORMATION_HPP
