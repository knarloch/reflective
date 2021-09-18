#include "Record.hpp"
#include "transformation/treelikeString.hpp"
#include "gtest/gtest.h"

TEST(toTreelikeMultilineStringTest, Record)
{
  Record r;
  auto x = reflective::toTreelikeMultilineString(r);
  EXPECT_EQ(typeid(x), typeid(std::string));
  EXPECT_NE(x.find(r.id.getMemberName()), std::string::npos);
  EXPECT_NE(x.find(std::to_string(r.id)), std::string::npos);
  EXPECT_NE(x.find(std::to_string(r.someNumber)), std::string::npos);
  EXPECT_NE(x.find(r.someText.getValue()), std::string::npos);
}

TEST(toTreelikeMultilineStringTest, RecordInRecord)
{
  RecordInRecord r;
  auto x = reflective::toTreelikeMultilineString(r);

  EXPECT_EQ(typeid(x), typeid(std::string));

  auto cursor = 1u + x.find(r.firstRecord.getMemberName());
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.firstRecord().id), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.firstRecord().someNumber), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(r.firstRecord().someText(), cursor);
  EXPECT_NE(cursor, std::string::npos);

  cursor = 1u + x.find(r.secondRecord.getMemberName(), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.secondRecord().id), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.secondRecord().someNumber), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(r.secondRecord().someText(), cursor);
  EXPECT_NE(cursor, std::string::npos);
}

TEST(toTreelikeMultilineStringTest, RecordVector)
{
  RecordVector r;
  auto x = reflective::toTreelikeMultilineString(r);

  EXPECT_EQ(typeid(x), typeid(std::string));

  auto cursor = 1u + x.find(r.singleRecord.getMemberName());
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.singleRecord().id), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.singleRecord().someNumber), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(r.singleRecord().someText(), cursor);
  EXPECT_NE(cursor, std::string::npos);

  cursor = 1u + x.find(r.recordVector.getMemberName(), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.recordVector()[0].id), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.recordVector()[0].someNumber), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(r.recordVector()[0].someText(), cursor);
  EXPECT_NE(cursor, std::string::npos);

  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.recordVector()[1].id), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(std::to_string(r.recordVector()[1].someNumber), cursor);
  EXPECT_NE(cursor, std::string::npos);
  cursor = 1u + x.find(r.recordVector()[1].someText(), cursor);
  EXPECT_NE(cursor, std::string::npos);
}