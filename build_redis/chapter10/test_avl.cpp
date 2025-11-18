#include "avl.hpp"
#include <set>
#include <cassert>

static void avl_verify(AVLNode *parent, AVLNode *node)
{
  if (!node)
  {
    return;
  }

  assert(node->parent == parent);
  avl_verify(node, node->left);
  avl_verify(node, node->right);

  assert(node->cnt == 1 + avl_cnt(node->left) + avl_cnt(node->right));
  uint32_t l = avl_depth(node->left);
  uint32_t r = avl_depth(node->right);
  assert(l == r || l + 1 == r || l == r + 1);
  assert(node->depth == 1 + max(l, r));
  uint32_t val = container_of(node, Data, node)->val;
  if (node->left)
  {
    assert(node->left->parent == node);
    assert(container_of(node->left, Data, node)->val <= val);
  }
  if (node->right)
  {
    assert(node->right->parent == node);
    assert(container_of(node->right, Data, node)->val >= val);
  }
}

static void extract(AVLNode *node, std::multiset<uint32_t> &extracted)
{
  if (!node)
  {
    return;
  }
  extract(node->left, extracted);
  extracted.insert(container_of(node, Data, node)->val);
  extract(node->right, extracted);
}

static void container_verify(
    Container &c, const std::multiset<uint32_t> &ref)
{
  avl_verify(NULL, c.root);
  assert(avl_cnt(c.root) == ref.size());
  std::multiset<uint32_t> extracted;
  extract(c.root, extracted);
  assert(extracted == ref);
}
static void dispose(Container &c)
{
  while (c.root)
  {
    AVLNode *node = c.root;
    c.root = avl_del(c.root);
    delete container_of(node, Data, node);
  }
}

void general_tests()
{
  Container c;
  container_verify(c, {});
  add(c, 123);
  container_verify(c, {123});
  assert(!del(c, 124));
  assert(del(c, 123));
  container_verify(c, {});
  std::multiset<uint32_t> ref;
  for (uint32_t i = 0; i < 1000; i += 3)
  {
    add(c, i);
    ref.insert(i);
    container_verify(c, ref);
  }

  for (uint32_t i = 0; i < 100; i++)
  {
    uint32_t val = (uint32_t)rand() % 1000;
    add(c, val);
    ref.insert(val);
    container_verify(c, ref);
  }

  for (uint32_t i = 0; i < 200; i++)
  {
    uint32_t val = (uint32_t)rand() % 1000;
    auto it = ref.find(val);
    if (it == ref.end())
    {
      assert(!del(c, val));
    }
    else
    {
      assert(del(c, val));
      ref.erase(it);
    }
    container_verify(c, ref);
  }
}

static void test_insert(uint32_t sz)
{
  for (uint32_t val = 0; val < sz; ++val)
  {
    Container c;
    std::multiset<uint32_t> ref;
    for (uint32_t i = 0; i < sz; ++i)
    {
      if (i == val)
      {
        continue;
      }
      add(c, i);
      ref.insert(i);
    }
    container_verify(c, ref);
    add(c, val);
    ref.insert(val);
    container_verify(c, ref);
    dispose(c);
  }
}

static void test_remove(uint32_t sz)
{
  for (uint32_t val = 0; val < sz; ++val)
  {
    Container c;
    std::multiset<uint32_t> ref;
    for (uint32_t i = 0; i < sz; ++i)
    {
      add(c, i);
      ref.insert(i);
    }
    container_verify(c, ref);
    assert(del(c, val));
    ref.erase(val);
    container_verify(c, ref);
    dispose(c);
  }
}

int main()
{
  general_tests();
  for (uint32_t i = 0; i < 200; ++i)
  {
    test_insert(i);
    test_remove(i);
  }
}
