#include "Record.hpp"
#include "gtest/gtest.h"
#include <array>
#include <transformation/Transformation.hpp>

TEST(RecordTest, CanGetSetValue_bultinType)
{
  auto c = Record{};
  EXPECT_EQ(c.id, 0);
  c.id = 3;
  EXPECT_EQ(c.id, 3);
  EXPECT_EQ(c.id.getMemberName(), "id");
}

TEST(RecordTest, CanGetSetValue_stringType)
{
  auto c = Record{};
  EXPECT_EQ(static_cast<std::string>(c.someText), std::string{ "defaultSomeText" });
  c.someText = { "someSomeText" }; // does not work with c-style string, must be std::string (or initializer_list}
  c.someText() = "someSomeText";
  EXPECT_EQ(static_cast<decltype(c.someText)::ValueType>(c.someText), "someSomeText");
  EXPECT_EQ(c.someText(), "someSomeText");
  EXPECT_EQ(c.someText.getMemberName(), "someText");
}

TEST(RecordTest, CanConvertToTupleOfDeclaredFields)
{
  Record r;
  auto t = r.toTuple();
  EXPECT_EQ(3, std::tuple_size<decltype(t)>());
  EXPECT_EQ(typeid(Record::id_t), typeid(std::get<0>(t)));
  EXPECT_EQ(0, std::get<0>(t));
  EXPECT_EQ(typeid(Record::someNumber_t), typeid(std::get<1>(t)));
  EXPECT_EQ(23, std::get<1>(t));
  EXPECT_EQ(typeid(Record::someText_t), typeid(std::get<2>(t)));
  EXPECT_EQ(std::string{ "defaultSomeText" }, std::get<2>(t).getValue());
}

struct CtorsCounter
{
  static int defCtorCallCount;
  static int copyCtorCallCount;
  static int moveCtorCallCount;
  static int copyAssignCallCount;
  static int moveAssignCallCount;
  static void resetCounters()
  {
    defCtorCallCount = 0;
    copyCtorCallCount = 0;
    moveCtorCallCount = 0;
    copyAssignCallCount = 0;
    moveAssignCallCount = 0;
  }
  CtorsCounter() { defCtorCallCount++; }
  CtorsCounter(const CtorsCounter&) { copyCtorCallCount++; }
  CtorsCounter(CtorsCounter&&) { moveCtorCallCount++; }
  CtorsCounter& operator=(const CtorsCounter&)
  {
    copyAssignCallCount++;
    return *this;
  }
  CtorsCounter& operator=(CtorsCounter&&)
  {
    moveAssignCallCount++;
    return *this;
  }
};
decltype(CtorsCounter::defCtorCallCount) CtorsCounter::defCtorCallCount;
decltype(CtorsCounter::copyCtorCallCount) CtorsCounter::copyCtorCallCount;
decltype(CtorsCounter::moveCtorCallCount) CtorsCounter::moveCtorCallCount;
decltype(CtorsCounter::copyAssignCallCount) CtorsCounter::copyAssignCallCount;
decltype(CtorsCounter::moveAssignCallCount) CtorsCounter::moveAssignCallCount;

struct RecordWithCtorsCounter
{
  DEFINE_MEMBER(CtorsCounter, m0, {});
  DEFINE_MEMBER(CtorsCounter, m1, {});
  DEFINE_MEMBER(CtorsCounter, m2, {});
  DEFINE_TO_TUPLE(m0, m1, m2);
};

TEST(RecordWithCtorsCounterTest, lvalueToTupleConversionIsOneCastAndOneMovePerMember)
{
  RecordWithCtorsCounter r;
  CtorsCounter::resetCounters();
  auto t = r.toTuple();
  (void)t;
  EXPECT_EQ(3, CtorsCounter::copyCtorCallCount);
  EXPECT_EQ(3, CtorsCounter::moveCtorCallCount);
  EXPECT_EQ(0, CtorsCounter::defCtorCallCount);
  EXPECT_EQ(0, CtorsCounter::copyAssignCallCount);
  EXPECT_EQ(0, CtorsCounter::moveAssignCallCount);
}

TEST(RecordWithCtorsCounterTest, rvalueToTupleConversionIsTwoMovesPerMember)
{
  auto t =
    []() {
      auto r = RecordWithCtorsCounter{};
      CtorsCounter::resetCounters();
      return r;
    }()
      .toTuple();
  (void)t;
  EXPECT_EQ(0, CtorsCounter::copyCtorCallCount);
  EXPECT_EQ(6, CtorsCounter::moveCtorCallCount);
  EXPECT_EQ(0, CtorsCounter::defCtorCallCount);
  EXPECT_EQ(0, CtorsCounter::copyAssignCallCount);
  EXPECT_EQ(0, CtorsCounter::moveAssignCallCount);
}

TEST(RecordArrayTest, canAccessComposedTypes)
{
  auto ca = RecordVector{};
  ca.singleRecord().id() = 4;
  EXPECT_EQ(ca.singleRecord().id(), 4);
  EXPECT_EQ(ca.singleRecord.getMemberName(), "singleRecord");

  auto c = Record{};
  c.someText() = "superSomeText";
  ca.recordVector().push_back(c);
  EXPECT_EQ(ca.recordVector().crbegin()->someText(), "superSomeText");
  EXPECT_EQ(ca.recordVector.getMemberName(), "recordVector");
}

TEST(RecordArrayTest, CanConvertToTupleOfDeclaredFields)
{
  RecordVector ra;
  auto t = ra.toTuple();
  EXPECT_EQ(2, std::tuple_size<decltype(t)>());
  EXPECT_EQ(typeid(RecordVector::singleRecord_t), typeid(std::get<0>(t)));
  EXPECT_EQ(typeid(RecordVector::recordVector_t), typeid(std::get<1>(t)));
}

TEST(HasToTupleTest, whenTypeDefinesToTuple_HasToTupleIsTrue)
{
  EXPECT_TRUE(reflective::HasToTupleMethod<Record>::value);
  EXPECT_FALSE(reflective::HasToTupleMethod<Record::id_t>::value);
  EXPECT_FALSE(reflective::HasToTupleMethod<Record::id_t::ValueType>::value);
}

TEST(HasIterator, _)
{
  EXPECT_FALSE(reflective::HasIterator<int>::value);
  EXPECT_FALSE(reflective::HasIterator<Record>::value);
  EXPECT_FALSE(reflective::HasIterator<Record::id_t>::value);

  EXPECT_TRUE(reflective::HasIterator<std::vector<int>>::value);
  EXPECT_TRUE(reflective::HasIterator<std::vector<Record>>::value);
  EXPECT_TRUE(reflective::HasIterator<std::vector<Record::id_t>>::value);

  EXPECT_TRUE((reflective::HasIterator<std::array<int, 1u>>::value));
  EXPECT_TRUE((reflective::HasIterator<std::array<Record, 1u>>::value));
  EXPECT_TRUE((reflective::HasIterator<std::array<Record::id_t, 1u>>::value));

  EXPECT_TRUE((reflective::HasIterator<std::initializer_list<int>>::value));
  EXPECT_TRUE((reflective::HasIterator<std::initializer_list<Record>>::value));
  EXPECT_TRUE((reflective::HasIterator<std::initializer_list<Record::id_t>>::value));
}
//
//TEST(TransformationTest, TransformationCanBeUsedWithReflectiveStruct)
//{
//  EXPECT_NO_THROW(reflective::forEachMember(reflective::Transformation{}, Record{}));
//  EXPECT_NO_THROW(reflective::forEachMember(reflective::Transformation{}, RecordInRecord{}));
//  EXPECT_NO_THROW(reflective::forEachMember(reflective::Transformation{}, RecordVector{}));
//}