#include "reflective.hpp"
#include "gtest/gtest.h"

struct Record
{
  DECLARE_FIELD(int, id, 0);

  DECLARE_FIELD(int, someNumber, 23);

  DECLARE_FIELD(std::string, someText, "defaultSomeText");

  ADD_TUPLE_CONVERSION(id, someNumber, someText);
};

TEST(RecordTest, CanGetSetValue_bultinType)
{
  auto c = Record{};
  EXPECT_EQ(c.id, 0);
  c.id = 3;
  EXPECT_EQ(c.id, 3);
  EXPECT_EQ(c.id.GetFieldName(), "id");
}

TEST(RecordTest, CanGetSetValue_stringType)
{
  auto c = Record{};
  EXPECT_EQ(static_cast<std::string>(c.someText), std::string{ "defaultSomeText" });
  c.someText = { "someSomeText" }; // does not work with c-style string, must be std::string (or initializer_list}
  c.someText() = "someSomeText";
  EXPECT_EQ(static_cast<decltype(c.someText)::value_type>(c.someText), "someSomeText");
  EXPECT_EQ(c.someText(), "someSomeText");
  EXPECT_EQ(c.someText.GetFieldName(), "someText");
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
  EXPECT_EQ(std::string{ "defaultSomeText" }, std::get<2>(t).Value());
}

struct RecordArray
{
  DECLARE_FIELD(Record, singleRecord, {});
  DECLARE_FIELD(std::vector<Record>, recordVector, {});

  ADD_TUPLE_CONVERSION(singleRecord, recordVector)
};

TEST(RecordArrayTest, canAccessComposedTypes)
{
  auto ca = RecordArray{};
  ca.singleRecord().id() = 4;
  EXPECT_EQ(ca.singleRecord().id(), 4);
  EXPECT_EQ(ca.singleRecord.GetFieldName(), "singleRecord");

  auto c = Record{};
  c.someText() = "superSomeText";
  ca.recordVector().push_back(c);
  EXPECT_EQ(ca.recordVector()[0].someText(), "superSomeText");
  EXPECT_EQ(ca.recordVector.GetFieldName(), "recordVector");
}

TEST(RecordArrayTest, CanConvertToTupleOfDeclaredFields)
{
  RecordArray ra;
  auto t = ra.toTuple();
  EXPECT_EQ(2, std::tuple_size<decltype(t)>());
  EXPECT_EQ(typeid(RecordArray::singleRecord_t), typeid(std::get<0>(t)));
  EXPECT_EQ(typeid(RecordArray::recordVector_t), typeid(std::get<1>(t)));
}