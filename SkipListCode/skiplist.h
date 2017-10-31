//
// Created by Abson on 2017/10/28.
// Copyright (c) 2017 Abson. All rights reserved.
//

#ifndef 研究跳跃链表_SKIP_LIST_H
#define 研究跳跃链表_SKIP_LIST_H


#include <cstdlib>
#include <assert.h>
#include <iostream>

uint32_t Next( uint32_t& seed);

template <typename Key, typename Value>
class SkipList {

private:
  struct Node;

public:
  explicit SkipList();

  Value Find(const Key& key);

  bool Insert(const Key& k, const Value& v);

  void Print();

  bool Delete(const Key&k);
private:
  enum {kMaxLevel = 12};

  Node* NewNode(int level, const Key& key,  const Value& value);

  Node* FindGreaterOrEqual(const Key& key, Node** prev) const;

  // 插入元素的时候元素所占有的层数完全是随机的，通过随机算法产生
  int randomLevel();

  int level_;
  Node* head_;
  uint32_t rnd_;

  SkipList(const SkipList&);
  void operator=(const SkipList&);
};

template <typename Key, typename Value>
struct SkipList<Key, Value>::Node {

  explicit Node(const Key& k, const Value& v) : key(k), value(v) {}

  Key   key;
  Value  value;

  void SetNext(int i, Node* x) {
    assert(i >= 0);

    forward_[i] = x;
  }

  Node* Next(int i) {
    assert(i >= 0);

    return forward_[i];
  }

private:
  struct Node* forward_[1];
};

template <typename Key, typename Value>
typename SkipList<Key, Value>::Node*
SkipList<Key, Value>::NewNode(int level, const Key& key,  const Value& value)
{
  size_t men = sizeof(Node) + level * sizeof(Node*);
  Node* node = (Node*)malloc(men);
  node->key = key;
  node->value = value;
  return node;
}

template <typename Key, typename Value>
SkipList<Key, Value>::SkipList() : head_(NewNode( kMaxLevel, 0, 0)), rnd_(0xdeadbeef)
{
  for (int i = 0; i < kMaxLevel; ++i) {
    head_->SetNext(i, nullptr);
  }
}

template <typename Key, typename Value>
int SkipList<Key, Value>::randomLevel() {

  static const unsigned int kBranching = 4;
  int height = 1;
  while (height < kMaxLevel && ((::Next(rnd_) % kBranching) == 0)) {
    height++;
  }
  assert(height > 0);
  assert(height <= kMaxLevel);
  return height;
}

template <typename Key, typename Value>
bool SkipList<Key, Value>::Insert(const Key& key, const Value& value) {

  Node* prev[kMaxLevel];

  int level;

  Node* next = FindGreaterOrEqual(key, prev);

  // 不能插入相同的key
  if ( next && next->key == key ) {
    return false;
  }

  // 产生一个随机层数 k
  // 新建一个待插入节点q
  // 一层一层插入
  level = randomLevel();
  // 更新跳表 level
  if (level > level_) {
    for (int i = level_; i < level; ++i) {
      prev[i] = head_; // 新增的层数初始化
    }
    level_ = level;
  }

  next = NewNode(level, key, value);
  // 逐层更新节点的指针
  for (int j = 0; j < level; ++j) {
    next->SetNext(j, prev[j]->Next(j));
    prev[j]->SetNext(j, next);
  }

  return true;
}

template <typename Key, typename Value>
typename SkipList<Key, Value>::Node*
SkipList<Key, Value>::FindGreaterOrEqual(const Key& key, Node** prev) const
{
  Node* x = head_, *next = nullptr;

  int level = level_ - 1;

//  printf("--------\n");
  // 从最高层往下查找需要插入的位置
  // 填充 prev
  for (int i = level; i >= 0 ; --i) {
    while ( (next = x->Next(i)) && next->key < key) {
      x = next;
    }
    if (NULL != prev) {prev[i] = x;}
  }
  return next;
};

template <typename Key, typename Value>
Value
SkipList<Key, Value>::Find(const Key &key) {
  Node* node = FindGreaterOrEqual(key, NULL);
  if (node) {
    return node->value;
  }
  return NULL;
}

template <typename Key, typename Value>
bool
SkipList<Key, Value>::Delete(const Key&key)
{
  Node* prev[kMaxLevel];
  Node* next = FindGreaterOrEqual(key, prev);

  int level = level_;
  if (next && next->key == key) {
    // 将每层跳点链表设置到 next 节点所指向的每层的链表
    for (int i = 0; i < level; ++i) {
      if (prev[i]->Next(i) && prev[i]->Next(i)->key == next->key) {
        prev[i]->SetNext(i, next->Next(i));
      }
    }

    // 释放该节点数组的所有内存
    free(next);

    //如果删除的是最大层的节点，那么需要重新维护跳表的
    for (int j = level_-1; j >= 0 ; --j) {
      if (head_->Next(j) == NULL) {
        level_--;
      }
    }
    return true;
  }

  return false;
};

template <typename Key, typename Value>
void
SkipList<Key, Value>::Print()
{
  Node* next, *x = head_;

  printf("--------\n");
  for (int i = level_ - 1; i >= 0; --i) {
    x = head_;
    while ((next = x->Next(i))) {
      x = next;
      std::cout << "key: " << next->key << " -> ";
    }
    printf("\n");
  }
  printf("--------\n");
}

uint32_t Next( uint32_t& seed) {
  seed = seed & 0x7fffffffu;

  if (seed == 0 || seed == 2147483647L) {
    seed = 1;
  }

  static const uint32_t M = 2147483647L;   // 2^31-1
  static const uint64_t A = 16807;  // bits 14, 8, 7, 5, 2, 1, 0
  // We are computing
  //       seed_ = (seed_ * A) % M,    where M = 2^31-1
  //
  // seed_ must not be zero or M, or else all subsequent computed values
  // will be zero or M respectively.  For all other values, seed_ will end
  // up cycling through every number in [1,M-1]
  uint64_t product = seed * A;

  // Compute (product % M) using the fact that ((x << 31) % M) == x.
  seed = static_cast<uint32_t>((product >> 31) + (product & M));
  // The first reduction may overflow by 1 bit, so we may need to
  // repeat.  mod == M is not possible; using > allows the faster
  // sign-bit-based test.
  if (seed > M) {
    seed -= M;
  }
  return seed;
}

#endif //研究跳跃链表_SKIP_LIST_H
