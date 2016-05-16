#include <tools/Comparator.h>

#include <stdio.h>

using namespace Comnet;

namespace TestComparator {

class Test {
private:
  int32_t number;
  char letter;

  friend class TestCompare;
  
public:
  Test() : number(0), letter('a') { }
  Test(int32_t number) : number(number), letter('a') { }
  Test(int32_t number, char letter) : number(number), letter(letter) { }
};

class TestCompare : public Comparator<Test> {
public:
  
  TestCompare() { }
  ~TestCompare() { }
  // Compare object.
  int32_t compare(const Test& obj1, const Test& obj2) {
    if (obj1.number > obj2.number) {
      return 1;
    } else if (obj1.number < obj2.number) {
      return -1;
    } else {
      return 0;
    }
  }

  //  int32_t operator()(const Test& obj1, const Test& obj2) {
  //  return compare(obj1, obj2);
  // }
};
} // TestComparator

using namespace TestComparator;

int main(int c, char** args) {
  Test t1(10);
  Test t2(20);
  //TestCompare tC;
  //Comparator<Test>& _cmp = tC;
  TestCompare _cmp;
  int32_t result = _cmp(t1, t2);
  printf("%d\n", result);
  return 0;
}
