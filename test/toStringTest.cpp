#include "toString.hpp"
#include "Record.hpp"
#include "gtest/gtest.h"

TEST(toStringTest, Record)
{
  Record r;
  auto x = reflective::toString(r);
  EXPECT_EQ(typeid(x), typeid(std::string));
  EXPECT_NE(x.find(r.id.getMemberName()), std::string::npos);
  EXPECT_NE(x.find(std::to_string(r.id)), std::string::npos);
  EXPECT_NE(x.find(std::to_string(r.someNumber)), std::string::npos);
  EXPECT_NE(x.find(r.someText.getValue()), std::string::npos);
}

