#include "avl.hpp"
#include "hashtable.hpp"

struct ZSet
{
  AVLNode *tree = NULL;
  HMap hmap;
};

struct ZNode
{
  AVLNode tree;
  HNode hmap;
  double score = 0;
  size_t len = 0;
  char name[0];
};

static uint64_t str_hash(const uint8_t *data, size_t len)
{
  uint32_t h = 0x811C9DC5;
  for (size_t i = 0; i < len; i++)
  {
    h = (h + data[i]) * 0x01000193;
  }
  return h;
}
static size_t min(double a, double b)
{
  return (a > b) ? b : a;
}

static bool zless(
    AVLNode *lhs, double score, const char *name, size_t len)
{
  ZNode *zl = container_of(lhs, ZNode, tree);
  if (zl->score != score)
  {
    return zl->score < score;
  }
  int rv = memcmp(zl->name, name, min(zl->len, len));
  if (rv != 0)
  {
    return rv < 0;
  }
  return zl->len < len;
}

static bool zless(AVLNode *lhs, AVLNode *rhs)
{
  ZNode *zr = container_of(rhs, ZNode, tree);
  return zless(lhs, zr->score, zr->name, zr->len);
}

static ZNode *znode_new(const char *name, size_t len, double score)
{
  ZNode *node = (ZNode *)malloc(sizeof(ZNode) + len);
  assert(node);
  avl_init(&node->tree);
  node->hmap.next = NULL;
  node->hmap.hcode = str_hash((uint8_t *)name, len);
  node->score = score;
  node->len = len;
  memcpy(&node->name[0], name, len);
}

static void tree_add(ZSet *zset, ZNode *node)
{
  if (!zset->tree)
  {
    zset->tree = &node->tree;
    return;
  }
  AVLNode *cur = zset->tree;
  while (true)
  {
    AVLNode **from = zless(&node->tree, cur) ? &cur->left : &cur->right;
    if (!*from)
    {
      *from = &node->tree;
      node->tree.parent = cur;
      zset->tree = avl_fix(&node->tree);
      break;
    }
    cur = *from;
  }
}

static void zset_update(ZSet *zset, ZNode *node, double score)
{
  if (node->score == score)
  {
    return;
  }
  zset->tree = avl_del(&node->tree);
  node->score = score;
  avl_init(&node->tree);
  tree_add(zset, node);
}

struct HKey
{
  HNode node;
  const char *name = NULL;
  size_t len = 0;
};

static bool hcmp(HNode *node, HNode *key)
{
  ZNode *znode = container_of(node, ZNode, hmap);
  HKey *hkey = container_of(key, HKey, node);
  if (znode->len != hkey->len)
  {
    return false;
  }
  return 0 == memcmp(znode->name, hkey->name, znode->len);
}

ZNode *zset_lookup(ZSet *zset, const char *name, size_t len)
{
  if (!zset->tree)
  {
    return NULL;
  }

  HKey key;
  key.node.hcode = str_hash((uint8_t *)name, len);
  key.name = name;
  key.len = len;
  HNode *found = hm_lookup(&zset->hmap, &key.node, &hcmp);
  return found ? container_of(found, ZNode, hmap) : NULL;
}
bool zset_add(ZSet *zset, const char *name, size_t len, double score)
{
  ZNode *node = zset_lookup(zset, name, len);
  if (node)
  {
    zset_update(zset, node, score);
    return false;
  }
  else
  {
    node = znode_new(name, len, score);
    hm_insert(&zset->hmap, &node->hmap);
    tree_add(zset, node);
    return true;
  }
}

AVLNode *avl_offset(AVLNode *node, int64_t offset)
{
  int64_t pos = 0;
  while (offset != pos)
  {
    if (pos < offset && pos + avl_cnt(node->right) >= offset)
    {
      node = node->right;
      pos += avl_cnt(node->left) + 1;
    }
    else if (pos > offset && pos - avl_cnt(node->left) <= offset)
    {
      node = node->left;
      pos -= avl_cnt(node->right) + 1;
    }
    else
    {
      AVLNode *parent = node->parent;
      if (!parent)
      {
        return NULL;
      }
      if (parent->right == node)
      {
        pos -= avl_cnt(node->left) + 1;
      }
      else
      {
        pos += avl_cnt(node->right) + 1;
      }
      node = parent;
    }
  }
  return node;
}

ZNode *zset_query(
    ZSet *zset, double score, const char *name, size_t len, int64_t offset)
{
  AVLNode *found = NULL;
  AVLNode *cur = zset->tree;
  while (cur)
  {
    if (zless(cur, score, name, len))
    {
      cur = cur->right;
    }
    else
    {
      found = cur;
      cur = cur->left;
    }
  }
  if (found)
  {
    found = avl_offset(found, offset);
  }
  return found ? container_of(found, ZNode, tree) : NULL;
}
