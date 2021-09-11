#include "reflective.hpp"
#include "gtest/gtest.h"

struct Record
{
  DECLARE_FIELD(int, id, 0);
  DECLARE_FIELD(int, someNumber, 0);
  DECLARE_FIELD(std::string, someText, "defaultSomeText");
};

TEST(CourierTest, CanGetSetValue_bultinType)
{
  auto c = Record{};
  EXPECT_EQ(c.id, 0);
  c.id = 3;
  EXPECT_EQ(c.id, 3);
  EXPECT_EQ(c.id.GetFieldName(), "id");
}

TEST(CourierTest, CanGetSetValue_stringType)
{
  auto c = Record{};
  EXPECT_EQ(static_cast<std::string>(c.someText), std::string{ "defaultSomeText" });
  c.someText = { "someSomeText" }; // does not work with c-style string, must be std::string (or initializer_list}
  c.someText() = "someSomeText";
  EXPECT_EQ(static_cast<decltype(c.someText)::value_type>(c.someText), "someSomeText");
  EXPECT_EQ(c.someText(), "someSomeText");
  EXPECT_EQ(c.someText.GetFieldName(), "someText");
}

struct RecordArray
{
  DECLARE_FIELD(Record, singleRecord, {});
  DECLARE_FIELD(std::vector<Record>, recordVector, {});
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
