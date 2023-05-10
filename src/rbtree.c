#include "rbtree.h"

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

void free_node(rbtree* tree, node_t* x) {
	if (x->left != tree->nil)
		free_node(tree, x->left);
	if (x->right != tree->nil)
		free_node(tree, x->right);
	free(x);
	// x = NULL;
}

void delete_rbtree(rbtree *tree) {
  // TODO: reclaim the tree nodes's memory
  if (tree->root != tree->nil)
		free_node(tree, tree->root);
	free(tree->nil);
  free(tree);
}

void left_rotate(rbtree* tree, node_t* x) {
	node_t* y;

	y = x->right;
	x->right = y->left;

	if (y->left != tree->nil)
		y->left->parent = x;

	y->parent = x->parent;

	if (x->parent == tree->nil)
		tree->root = y;
	else if (x == x->parent->left)
		x->parent->left = y;
	else
		x->parent->right = y;

	y->left = x;
	x->parent = y;
}
void right_rotate(rbtree* tree, node_t* x) {
	node_t* y;

	y = x->left;
	x->left = y->right;

	if (y->right != tree->nil)
		y->right->parent = x;

	y->parent = x->parent;

	if (x->parent == tree->nil)
		tree->root = y;
	else if (x == x->parent->right)
		x->parent->right = y;
	else
		x->parent->right = y;

	y->right = x;
	x->parent = y;
}

void rbtree_insert_fixup(rbtree* tree, node_t* z) {
	node_t* y;

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
			// case 2: z가 부모의 오른쪽 자식이고, 부모가 할아버지의 왼쪽 자식이고, 부모 형제는 black이면
			else {
				if (z == z->parent->right) {  // z가 부모의 오른쪽 자식
					z = z->parent;  // z의 부모 기준으로
					left_rotate(tree, z);  // 왼쪽으로 회전
				}
				// case 3: z의 부모의 형제가 black, z는 왼쪽 자식, 부모는 red고 왼쪽 자식이면
				z->parent->color = RBTREE_BLACK;  // 부모의 색을 black으로
				z->parent->parent->color = RBTREE_RED;  // 할아버지의 색은 red로
				right_rotate(tree, z->parent->parent);  // z의 할아버지 기준으로 오른쪽으로 회전
			}
		}
		else {  // right와 left를 바꾼 것
			y = z->parent->parent->left;

			// case 1: 입력 노드 z가 red이고, 부모와 부모 형제가 red
			if (y->color == RBTREE_RED) {
				z->parent->color = RBTREE_BLACK;
				y->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				z = z->parent->parent;
			}
			// case 2: z가 왼쪽 자식이고, 부모는 오른쪽 자식이면
			else {
				if (z == z->parent->left) {
					z = z->parent;
					right_rotate(tree, z);
				}
				// case 3: z는 오른쪽 자식
				z->parent->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				left_rotate(tree, z->parent->parent);
			}
		}
	}
	tree->root->color = RBTREE_BLACK;
}
node_t *rbtree_insert(rbtree *tree, const key_t key) {
  node_t *z = (node_t *)calloc(1, sizeof(node_t));

	node_t *y = tree->nil;
	node_t *x = tree->root;

	z->key = key;

	while (x != tree->nil) {
		y = x;
		if (z->key < x->key)
			x = x->left;
		else
			x = x->right;
	}
	z->parent = y;

	if (y == tree->nil)
		tree->root = z;
	else if (z->key < y->key)
		y->left = z;
	else
		y->right = z;
	z->left = tree->nil;
	z->right = tree->nil;
	z->color = RBTREE_RED;

	rbtree_insert_fixup(tree, z);

	return z;
  // return t->root;
}

node_t *rbtree_find(const rbtree *tree, const key_t key) {
  node_t* current = tree->root;

	while (current != tree->nil) {
		if (current->key == key)  // if (curr->key == key)
			return current;

		if (current->key < key)  // 값이 크면 오른쪽으로
			current = current->right;
		else  // 값이 작으면 왼쪽으로
			current = current->left;
	}
	return NULL;
  // return t->root;
}

node_t *rbtree_min(const rbtree *tree) {
  if (tree->root == tree->nil)
		return NULL;

	node_t* curr = tree->root;

	while (curr->left != tree->nil)
		curr = curr->left;

	return curr;
  // return t->root;
}

node_t *rbtree_max(const rbtree *tree) {
  if (tree->root == tree->nil)
		return NULL;

	node_t* curr = tree->root;

	while (curr->right != tree->nil)
		curr = curr->right;

	return curr;
  // return t->root;
}

void rbtree_transplant(rbtree *tree, node_t *u, node_t *v) {
	if (u->parent == tree->nil)
		tree->root = v;
	else if (u == u->parent->left)
		u->parent->left = v;
	else
		u->parent->right = v;
	v->parent = u->parent;
}
void rbtree_remove_fixup(rbtree* tree, node_t* x) {
	while (x != tree->root && x->color == RBTREE_BLACK) {  // 삭제하려는 노드 x는 black
		if (x == x->parent->left) {  // x는 왼쪽 자식
			node_t *w = x->parent->right;  // w는 x의 오른쪽 형제
			
			// case 1: x의 형제 w가 red
			if (w->color == RBTREE_RED) {  // w가 red
				w->color = RBTREE_BLACK;  // w의 색을 black으로 바꿔줌; w의 자식이 black이어야 하기 때문
				x->parent->color = RBTREE_RED;  // 부모의 색도 바꿔줌; w와 부모가 색을 서로 바꿈
				left_rotate(tree, x->parent);  // 부모 기준으로 왼쪽으로 회전 -> x의 형제가 바뀜
				w = x->parent->right;  // 형제 w를 업데이트
			}
			// case 2: x의 형제 w가 black이고, w의 자녀도 모두 black
			if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
				w->color = RBTREE_RED;  // black인 형제의 색을 red로; w에서 black을 제거
				x = x->parent;  // x와 형제에서 제거한 black을 보충하기 위해 부모에게 여분의 black을 추가하기 위함
			}
			// case 3: x의 형제 w가 black, w의 왼쪽 자녀가 red & 오른쪽 자녀가 black
			else {  // w의 왼쪽 자녀와 오른쪽 자녀가 black이 아닌 경우
				if (w->right->color == RBTREE_BLACK) {  // 하지만 w가 black인 경우
					w->left->color = RBTREE_BLACK;  // w의 왼쪽 자식을 black으로
					w->color = RBTREE_RED;  // w를 red로; w와 왼쪽 자식의 색을 서로 바꿈
					right_rotate(tree, w);  // w 기준으로 오른쪽으로 회전 -> x의 형제가 바뀜
					w = x->parent->right;  // 형제 w를 업데이트
				}
				// case 4: x의 형제 w가 black, w의 오른쪽 자녀가 red (black이 아닌 경우)
				w->color = x->parent->color;  // w는 부모의 색으로(무슨 색이든 상관없이)
				x->parent->color = RBTREE_BLACK;  // 부모는 black으로; w와 부모의 색을 서로 바꿈
				w->right->color = RBTREE_BLACK;  // w의 오른쪽 자녀는 black으로
				left_rotate(tree, x->parent);  // 부모 기준으로 왼쪽으로 회전
				x = tree->root;  // x가 root가 되면 while문을 종료
			}
		}
		
		// right와 left 바꾼 경우
		else {  // x는 오른쪽 자식
			node_t* w = x->parent->left;

			// case 1: x의 형제 w가 red
			if (w->color == RBTREE_RED) {
				w->color = RBTREE_BLACK;
				x->parent->color = RBTREE_RED;
				left_rotate(tree, x->parent);
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
					right_rotate(tree, w);
					w = x->parent->left;
				}
				// case 4: x의 형제 w가 black, w의 왼쪽 자녀가 red
				w->color = x->parent->color;
				x->parent->color = RBTREE_BLACK;
				w->left->color = RBTREE_BLACK;
				right_rotate(tree, x->parent);
				x = tree->root;  // x가 root가 되면 while문을 종료
			}
		}
	}
	x->color = RBTREE_BLACK;
}
int rbtree_remove(rbtree* tree, node_t* z) {
	node_t* y;
	node_t* x;
	color_t y_original_color;

	y = z;
	y_original_color = y->color;

	if (z->left == tree->nil) {
		x = z->right;
		rbtree_transplant(tree, z, z->right);
	}
	else if (z->right == tree->nil) {
		x = z->left;
		rbtree_transplant(tree, z, z->left);
	}
	else {
		y = z->right;
		while (y->left != tree->nil)
			y = y->left;
		y_original_color = y->color;
		x = y->right;

		if (y->parent == z)
			x->parent = y;
		else {
			rbtree_transplant(tree, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		rbtree_transplant(tree, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}

	if (y_original_color == RBTREE_BLACK)
		rbtree_remove_fixup(tree, x);

	free(z);

	return 0;
}

void subtree_to_array(const rbtree *tree, node_t *curr, key_t *arr, size_t n, size_t *count) {
	if (curr == tree->nil)
		return;

	subtree_to_array(tree, curr->left, arr, n, count);
	if (*count < n)
		arr[(*count)++] = curr->key;
	else
		return;
	subtree_to_array(tree, curr->right, arr, n, count);
}
int rbtree_to_array(const rbtree *tree, key_t *arr, const size_t n) {
  if (tree->root == tree->nil)
		return 0;

	size_t count = 0;
	subtree_to_array(tree, tree->root, arr, n, &count);
  
  return 0;
}
