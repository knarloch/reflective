#ifndef REFLECTIVE_RECORD_HPP
#define REFLECTIVE_RECORD_HPP

#include "reflective.hpp"
#include <vector>

struct Record
{
  DEFINE_MEMBER(int, id, 0);
  DEFINE_MEMBER(int, someNumber, 23);
  DEFINE_MEMBER(std::string, someText, "defaultSomeText");
  DEFINE_TO_TUPLE(id, someNumber, someText);
};

struct RecordArray
{
  DEFINE_MEMBER(Record, singleRecord, {});
  DEFINE_MEMBER(std::vector<Record>, recordVector, {});
  DEFINE_TO_TUPLE(singleRecord, recordVector)
};


#endif // REFLECTIVE_RECORD_HPP
