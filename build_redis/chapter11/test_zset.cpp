#include "avl.hpp"
#include "zset.hpp"

static void test_case(uint32_t sz)
{
  Container c;
  for (uint32_t i = 0; i < sz; ++i)
  {
    add(c, i);
  }
  AVLNode *min = c.root;
  while (min->left)
  {
    min = min->left;
  }
  for (uint32_t i = 0; i < sz; ++i)
  {
    AVLNode *node = avl_offset(min, (int64_t)i);
    assert(container_of(node, Data, node)->val == i);

    for (uint32_t j = 0; j < sz; ++j)
    {
      int64_t offset = (int64_t)j - (int64_t)i;
      AVLNode *n2 = avl_offset(node, offset);
      assert(container_of(n2, Data, node)->val == j);
    }
    assert(!avl_offset(node, -(int64_t)i - 1));
    assert(!avl_offset(node, sz - i));
  }
}

int main()
{
  test_case(100);
}
