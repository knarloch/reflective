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