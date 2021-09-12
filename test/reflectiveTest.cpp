#include "reflective.hpp"
#include "gtest/gtest.h"

struct Record
{
  DEFINE_MEMBER(int, id, 0);
  DEFINE_MEMBER(int, someNumber, 23);
  DEFINE_MEMBER(std::string, someText, "defaultSomeText");
  DEFINE_TO_TUPLE(id, someNumber, someText);
};

TEST(RecordTest, CanGetSetValue_bultinType)
{
  auto c = Record{};
  EXPECT_EQ(c.id, 0);
  c.id = 3;
  EXPECT_EQ(c.id, 3);
  EXPECT_EQ(c.id.getFieldName(), "id");
}

TEST(RecordTest, CanGetSetValue_stringType)
{
  auto c = Record{};
  EXPECT_EQ(static_cast<std::string>(c.someText), std::string{ "defaultSomeText" });
  c.someText = { "someSomeText" }; // does not work with c-style string, must be std::string (or initializer_list}
  c.someText() = "someSomeText";
  EXPECT_EQ(static_cast<decltype(c.someText)::ValueType>(c.someText), "someSomeText");
  EXPECT_EQ(c.someText(), "someSomeText");
  EXPECT_EQ(c.someText.getFieldName(), "someText");
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

TEST(RecordWithCtorsCounterTest, toTupleConversionIsOneCastAndOneMovePerMember)
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

struct RecordArray
{
  DEFINE_MEMBER(Record, singleRecord, {});
  DEFINE_MEMBER(std::vector<Record>, recordVector, {});
  DEFINE_TO_TUPLE(singleRecord, recordVector)
};

TEST(RecordArrayTest, canAccessComposedTypes)
{
  auto ca = RecordArray{};
  ca.singleRecord().id() = 4;
  EXPECT_EQ(ca.singleRecord().id(), 4);
  EXPECT_EQ(ca.singleRecord.getFieldName(), "singleRecord");

  auto c = Record{};
  c.someText() = "superSomeText";
  ca.recordVector().push_back(c);
  EXPECT_EQ(ca.recordVector()[0].someText(), "superSomeText");
  EXPECT_EQ(ca.recordVector.getFieldName(), "recordVector");
}

TEST(RecordArrayTest, CanConvertToTupleOfDeclaredFields)
{
  RecordArray ra;
  auto t = ra.toTuple();
  EXPECT_EQ(2, std::tuple_size<decltype(t)>());
  EXPECT_EQ(typeid(RecordArray::singleRecord_t), typeid(std::get<0>(t)));
  EXPECT_EQ(typeid(RecordArray::recordVector_t), typeid(std::get<1>(t)));
}