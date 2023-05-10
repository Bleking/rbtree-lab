#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

#include <stdlib.h>

rbtree *new_rbtree(void) {
	rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

	node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));

	nil_node->color = RBTREE_BLACK;
	p->nil = nil_node;
	p->root = nil_node;

	return p;
}

void right_rotation(rbtree *t, node_t *y) {
  // 1. x를 y의 왼쪽 자식으로 설정
  node_t *x = y->left;
  // 2. x의 right의 부모를 y로 연결
  y->left = x->right;
  if (x->right != t->nil)
    x->right->parent = y;
  // 3. y의 부모를 x의 부모와 연결(3가지 경우)
  x->parent = y->parent;
  if (y->parent == t->nil)
    t->root = x;
  else if (y == y->parent->left)
    y->parent->left = x;
  else
    y->parent->right = x;
  // 4. x와 y를 연결
  x->right = y;
  y->parent = x;
}
void left_rotation(rbtree *t, node_t *x) {
  // 1. y를 x의 오른쪽 자식으로 설정
  node_t *y = x->right;
  // 2. y의 left의 부모를 x로 연결
  x->right = y->left;
  if (y->left != t->nil)
    y->left->parent = x;
  // 3. x의 부모를 y의 부모와 연결(3가지 경우)
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  // 4. x와 y를 연결
  y->left = x;
  x->parent = y;
}

void free_node(rbtree *tree, node_t *x) {
  // 후위 순회 방식으로 RB Tree 내의 노드 메모리 반환
  if (x->left != tree->nil)
    free_node(tree, x->left);
  if (x->right != tree->nil)
    free_node(tree, x->right);
  free(x);
}

void delete_rbtree(rbtree *tree) {
  if (tree->root != tree->nil)
    free_node(tree, tree->root);
  free(tree->nil);
  free(tree);
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  node_t *y;
  // z의 부모가 red일 경우 문제 해결을 위해 반복
  while (z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left) {  // z의 부모가 z의 할아버지의 왼쪽 자식이면
      y = z->parent->parent->right;  // y는 z의 부모의 오른쪽 형제 노드
      // case 1: 입력 노드 z가 red이고, 부모와 y가 red
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;  // z의 부모의 색을 black으로
        y->color = RBTREE_BLACK;  // z의 부모의 형제인 y도 black으로
        z->parent->parent->color = RBTREE_RED;  // z의 할아버지는 red로
        z = z->parent->parent;  // z의 할아버지에서부터 점검 시작
      }
      else {
        // case 2: z가 부모의 오른쪽 자식이고, 부모가 할아버지의 왼쪽 자식이고, 부모 형제는 black이면
        if (z == z->parent->right) {  // z가 부모의 오른쪽 자식
          z = z->parent;  // z의 부모 기준으로
          left_rotation(t, z);  // 왼쪽으로 회전
        }
        // case 3: z의 부모의 형제가 black, z는 왼쪽 자식, 부모는 red고 왼쪽 자식이면
        z->parent->color = RBTREE_BLACK;  // 부모의 색을 black으로
        z->parent->parent->color = RBTREE_RED;  // 할아버지의 색은 red로
        right_rotation(t, z->parent->parent);  // z의 할아버지 기준으로 오른쪽으로 회전
      }
    }
    else {  // right와 left를 바꾼 것
      y = z->parent->parent->left;
      // case 1: 입력 노드 z가 red이고, 부모와 부모 형제가 red
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        // 다음 반복 하게 되면 z->p->p에서 check
        z = z->parent->parent;
      }
      // case 2: z가 왼쪽 자식이고, 부모는 오른쪽 자식이면
      else {
        if (z == z->parent->left) {
          z = z->parent;
          right_rotation(t, z);
        }
        // case 3: z는 오른쪽 자식
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotation(t, z->parent->parent);
      }
    }
  }
  // root의 자녀들이 부모, 삼촌인 경우 root가 red가 될 수 있음!
  t->root->color = RBTREE_BLACK;
}

void rbtree_transplant(rbtree *tree, node_t *u, node_t *v) {  // u의 부모와 u의 자녀를 연결
  if (u->parent == tree->nil)
    tree->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *tree, node_t *x) {
  node_t *w;
  while (x != tree->root && x->color == RBTREE_BLACK) {  // 삭제하려는 노드 x는 black
    if (x == x->parent->left) {  // x는 왼쪽 자식
      node_t *w = x->parent->right;  // w는 x의 오른쪽 형제
      
      // case 1: x의 형제 w가 red
      if (w->color == RBTREE_RED) {  // w가 red
        w->color = RBTREE_BLACK;  // w의 색을 black으로 바꿔줌; w의 자식이 black이어야 하기 때문
        x->parent->color = RBTREE_RED;  // 부모의 색도 바꿔줌; w와 부모가 색을 서로 바꿈
        left_rotation(tree, x->parent);  // 부모 기준으로 왼쪽으로 회전 -> x의 형제가 바뀜
        w = x->parent->right;  // 형제 w를 업데이트
      }

      // case 2: x의 형제 w가 black이고, w의 자녀도 모두 black
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;  // black인 형제의 색을 red로; w에서 black을 제거
        x = x->parent;  // x와 형제에서 제거한 black을 보충하기 위해 부모에게 여분의 black을 추가하기 위함
      }
      
      // case 3: x의 형제 w가 black, w의 왼쪽 자녀가 red & 오른쪽 자녀가 black
      else {  // w의 왼쪽 자녀와 오른쪽 자녀가 black이 아닌 경우
        if (w->right->color == RBTREE_BLACK) {  // 하지만 w의 오른쪽 자녀가 black인 경우
          w->left->color = RBTREE_BLACK;  // w의 왼쪽 자식을 black으로
          w->color = RBTREE_RED;  // w를 red로; w와 왼쪽 자식의 색을 서로 바꿈
          right_rotation(tree, w);  // w 기준으로 오른쪽으로 회전 -> x의 형제가 바뀜
          w = w->parent;  // 형제 w를 업데이트
        }
        // case 4: x의 형제 w가 black, w의 오른쪽 자녀가 red (black이 아닌 경우)
				w->color = x->parent->color;  // w는 부모의 색으로(무슨 색이든 상관없이)
				x->parent->color = RBTREE_BLACK;  // 부모는 black으로; w와 부모의 색을 서로 바꿈
				w->right->color = RBTREE_BLACK;  // w의 오른쪽 자녀는 black으로
				left_rotation(tree, x->parent);  // 부모 기준으로 왼쪽으로 회전
				x = tree->root;  // x가 root가 되면 while문을 종료
      }
    }
    // right와 left 바꾼 경우
    else {  // x는 오른쪽 자식
      w = x->parent->left;

      // case 1: x의 형제 w가 red
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotation(tree, x->parent);
        w = x->parent->left;
      }

      // case 2: x의 형제 w가 black이고, w의 자녀도 모두 black
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      
      // case 3: x의 형제 w가 black, w의 오른쪽 자녀가 red & 왼쪽 자녀가 black
      else {
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotation(tree, w);
          w = x->parent->left;
        }
        // case 4: x의 형제 w가 black, w의 왼쪽 자녀가 red
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotation(tree, x->parent);
        x = tree->root;
      }
    }
  }
  x->color = RBTREE_BLACK;  // x가 root가 되면 while문을 종료
}

void subtree_to_array(const rbtree *tree, node_t *curr, key_t *arr, size_t n, size_t *count)
{
  if (curr == tree->nil)
    return;

  subtree_to_array(tree, curr->left, arr, n, count);
  if (*count < n)
    arr[(*count)++] = curr->key;
  else
    return;
  subtree_to_array(tree, curr->right, arr, n, count);
}

node_t *rbtree_insert(rbtree *tree, const key_t key){
  node_t *y = tree->nil;
  node_t *x = tree->root;
  node_t *z = (node_t *)calloc(1, sizeof(node_t));

  z->key = key;

  // x가 값이 존재하는 한 값을 비교하며 삽입될 자리 찾기
  while (x != tree->nil) {
    // y는 nil이 아닌 마지막 x
    y = x;
    if (z->key < x->key)
      x = x->left;
    else
      x = x->right;
  }

  // z와 부모의 관계 연결
  z->parent = y;
  if (y == tree->nil)
    tree->root = z;
  else if (z->key < y->key)
    y->left = z;
  else
    y->right = z;

  // z의 자녀와 색상 설정
  z->left = tree->nil;
  z->right = tree->nil;
  z->color = RBTREE_RED;

  // 삽입으로 인한 속성 위배 수정
  rbtree_insert_fixup(tree, z);

  return z;
}

node_t *rbtree_find(const rbtree *tree, const key_t key) {
  node_t *current = tree->root;

  while (current != tree->nil) {
    if (current->key == key)
      return current;

    if (current->key < key)
      current = current->right;
    else
      current = current->left;
  }

  return NULL;
}

node_t *rbtree_min(const rbtree *tree) {
  // x가 root 일 때부터 탐색 시작
  node_t *y;
  node_t *x = tree->root;

  // root가 nil인 경우 NULL 반환
  if (x == tree->nil)
    return NULL;

  // nil이 나올 때까지 왼쪽자녀 탐색, nil이 아닌 마지막 x값을 return값
  while (x != tree->nil) {
    y = x;
    x = x->left;
  }

  return y;
}

node_t *rbtree_max(const rbtree *tree) {
  // x가 root일 때부터 탐색 시작
  node_t *y;
  node_t *x = tree->root;

  // root가 nil인 경우 NULL반환
  if (x == tree->nil)
    return NULL;

  // nil이 나올 때까지 오른쪽 자녀 탐색, nil이 아닌 마지막 x값을 return
  while (x != tree->nil) {
    y = x;
    x = x->right;
  }
  return y;
}

int rbtree_erase(rbtree *tree, node_t *z) {
  node_t *y;
  node_t *x;
  color_t y_original_color;

  y = z;
  y_original_color = y->color;

  // z 보다 작은 노드 없을 경우 x(z->right)를 z->parent와 직접 연결
  if (z->left == tree->nil) {
    x = z->right;
    rbtree_transplant(tree, z, x);
  }
  // z 보다 큰 노드 없을 경우 x(z->left)를 z->parent와 직접 연결
  else if (z->right == tree->nil) {
    x = z->left;
    rbtree_transplant(tree, z, x);
  }
  // 그 외의 경우(z가 자녀 둘은 가진 경우)
  else {
    // successor y 찾기
    y = z->right;
    while (y->left != tree->nil)
      y = y->left;

    // y_original_color 변수에 successor의 color 저장
    y_original_color = y->color;
    // x에 successor 오른쪽 자녀 저장
    x = y->right;

    // z와 successor가 원래 부모-자녀관계 였으면 successor의 right를 z에 연결
    if (y->parent == z)
      x->parent = y;
    // successor의 부모와 successor의 오른쪽 자녀 연결
    else {
      rbtree_transplant(tree, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    // z의 부모와 successor 연결,
    // 원래 z에 연결되어 있던 왼쪽 서브트리를 y의 왼쪽 자녀로 연결
    // z->left 였던 y->left의 부모로 y연결
    // successor의 color에 기존 z의 color 연결
    rbtree_transplant(tree, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  // 기존 successor의 색이 black이었으면 fixup!
  if (y_original_color == RBTREE_BLACK)
    rbtree_delete_fixup(tree, x);

  // 삭제된 z 메모리 free
  free(z);

  return 0;
}

int rbtree_to_array(const rbtree *tree, key_t *arr, const size_t n) {
  if (tree->root == tree->nil)
    return 0;

  size_t count = 0;
  subtree_to_array(tree, tree->root, arr, n, &count);

  return 0;
}
