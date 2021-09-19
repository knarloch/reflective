#include "transformation/nlohmannJsonTransformation.hpp"
#include "Record.hpp"
#include "gtest/gtest.h"

TEST(toJsonTest, TransformRecord)
{
  auto json = reflective::toJson(Record{});

  EXPECT_EQ(json[Record::id_t::memberName], 0);
  EXPECT_EQ(json[Record::someNumber_t::memberName], 23);
  EXPECT_EQ(json[Record::someText_t::memberName], "defaultSomeText");
}

TEST(toJsonTest, TransformRecordInRecord)
{
  auto json = reflective::toJson(RecordInRecord{});

  EXPECT_EQ(json[RecordInRecord::firstRecord_t::memberName][Record::id_t::memberName], 0);
  EXPECT_EQ(json[RecordInRecord::firstRecord_t::memberName][Record::someNumber_t::memberName], 23);
  EXPECT_EQ(json[RecordInRecord::firstRecord_t::memberName][Record::someText_t::memberName], "defaultSomeText");

  EXPECT_EQ(json[RecordInRecord::secondRecord_t::memberName][Record::id_t::memberName], 0);
  EXPECT_EQ(json[RecordInRecord::secondRecord_t::memberName][Record::someNumber_t::memberName], 23);
  EXPECT_EQ(json[RecordInRecord::secondRecord_t::memberName][Record::someText_t::memberName], "defaultSomeText");
}

TEST(toJsonTest, TransformRecordVector)
{
  auto json = reflective::toJson(RecordVector{});

  EXPECT_EQ(json[RecordVector::singleRecord_t::memberName][Record::id_t::memberName], 0);
  EXPECT_EQ(json[RecordVector::singleRecord_t::memberName][Record::someNumber_t::memberName], 23);
  EXPECT_EQ(json[RecordVector::singleRecord_t::memberName][Record::someText_t::memberName], "defaultSomeText");

  EXPECT_EQ(json[RecordVector::recordVector_t ::memberName][0][Record::id_t::memberName], 0);
  EXPECT_EQ(json[RecordVector::recordVector_t::memberName][0][Record::someNumber_t::memberName], 23);
  EXPECT_EQ(json[RecordVector::recordVector_t::memberName][0][Record::someText_t::memberName], "defaultSomeText");

  EXPECT_EQ(json[RecordVector::recordVector_t ::memberName][1][Record::id_t::memberName], 0);
  EXPECT_EQ(json[RecordVector::recordVector_t::memberName][1][Record::someNumber_t::memberName], 23);
  EXPECT_EQ(json[RecordVector::recordVector_t::memberName][1][Record::someText_t::memberName], "defaultSomeText");
}

TEST(fromJsonTest, TransformRecord)
{

  nlohmann::json  json;
  json[Record::id_t::memberName] =  10;
  json[Record::someNumber_t::memberName] = 230;
  json[Record::someText_t::memberName] = "modifiedText";

  auto record = Record{};
//  auto record = reflective::toReflectiveStruct<Record>(json);

  EXPECT_EQ(record.id, 10);
  EXPECT_EQ(record.someNumber, 230);
  EXPECT_EQ(record.someText.getValue(), std::string{"modifiedText"});
}