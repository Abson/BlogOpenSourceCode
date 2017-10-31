//
//  main.cpp
//  研究跳跃链表
//
//  Created by Abson on 2017/10/28.
//  Copyright © 2017 Abson. All rights reserved.
//

#include <iostream>
#include "skiplist.h"
#include <sstream>
#include <string>
#include <cstring>

int main(int argc, const char * argv[]) {

  SkipList<int , int > list;

  for (int i = 1; i < 50; ++i) {

//    char* str = (char*)malloc(sizeof(1024));

//    sprintf(str, "%d", i);

    list.Insert(i , i);
    list.Print();
  }

  std::cout << "Delete" << "\n";

  list.Delete(2);
  list.Delete(6);
  list.Delete(48);
  list.Print();

//  list.Insert("1", "1");
//  list.Print();
//  list.Insert("2", "2");
//  list.Print();

//  int i = list.Find(5);
//  printf("find: %d\n", i);

//  i = list.Find(9);
//  printf("find: %d\n", i);

  std::cout << "Hello, World!\n";
  return 0;

}