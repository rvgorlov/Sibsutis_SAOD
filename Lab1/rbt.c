// Горлов Роман ИУ - 423
// Лабораторная №1 Красно-черные деревья. 

// Расход по памяти: О(n)
// Поиск, вставка, удаление, минимальное, максимальное: O(log n)

// 1. Каждый узел - red или black
// 2. Корень - black
// 3. Каждый лист - черный. 
// 4. Каждый red - имеет 2 black дочерних узла
// 5. У каждого узла левое и правое поддерево имеет одинаковые 
// числа черных узлов. 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define COLOR_RED 0
#define COLOR_BLACK 1

struct rbtree {
	int key;
	char *value;
	int color;
	
	struct rbtree *parent;
	struct rbtree *left;
	struct rbtree *right;
};

struct rbtree EmptyNode = {0, 0, COLOR_BLACK, NULL, NULL, NULL};
struct rbtree *NullNode = &EmptyNode;

struct rbtree *rbtree_add(struct rbtree *root, int key, char *value);

// Функции для востановления свойств К-Ч деревьев
struct rbtree *rbtree_fixup_add(struct rbtree *root, struct rbtree *node);
struct rbtree *rbtree_left_rotate(struct rbtree *root, struct rbtree *node);
struct rbtree *rbtree_right_rotate(struct rbtree *root, struct rbtree *node);

int main(int argc, char **argv){
	
	struct rbtree *tree = NULL;
	
	tree = rbtree_add(tree, 10, "10");
	tree = rbtree_add(tree, 5, "5");
	tree = rbtree_add(tree, 3, "3");
	tree = rbtree_add(tree, 11, "11");
	tree = rbtree_add(tree, 12, "12");
	tree = rbtree_add(tree, 6, "6");
	tree = rbtree_add(tree, 8, "8");
	tree = rbtree_add(tree, 9, "9");
	//rbtree_print(tree);
	//rbtree_free(tree);
	
	return 0;
}


// Добавление нового узла в дерево
struct rbtree *rbtree_add(struct rbtree *root, int key, char *value) { 
	struct rbtree *node, *parent = NullNode;
	
	// Поиск листа для нового элемента. 
	for (node = root; node != NullNode && node != NULL; )
	{
		parent = node;
		if (key < node->key)
			node = node->left;
		else if (key > node->key)
			node = node->right;
		else return root;
	}

	// Выделяем память под дерево
	node = malloc(sizeof(*node));
	
	// Проверка выделения памяти 
	if (node == NULL)
		return NULL;
	
	// Заполняем значения дерева	
	node->key = key;
	node->value = value;
	node->color = COLOR_RED;
	node->parent = parent;
	node->left = NullNode; 			// Листья пока пустые
	node->right = NullNode;			// Надо восстанавливать
									// свойства К-Ч деревьев.
	
	// Устанавливаем указатели из родительского
	// в дочерний (добавляемый)
	if (parent != NullNode) {
		if (key < parent->key)
			parent->left = node;
		else parent->right = node;
	} else root = node;
	
	// Востанавливаем свойства К-Ч деревьев, и его возвращаем. 
	return rbtree_fixup_add(root, node);
}

// Востановление свойств красно-черного дерева  
// после добавления нового узла
struct rbtree *rbtree_fixup_add(struct rbtree *root, struct rbtree *node) {

	struct rbtree *uncle;
	/* Current node is RED */
	while (node != root && node->parent->color == COLOR_RED) {
		if (node->parent == node->parent->parent->left) {

			/* Node in left tree of grandfather */
			uncle = node->parent->parent->right;

			if (uncle->color == COLOR_RED) {
				/* Case 1 - uncle is RED */
				node->parent->color = COLOR_BLACK;
				uncle->color = COLOR_BLACK;
				node->parent->parent->color = COLOR_RED;
				node = node->parent->parent;
			} else {
				/* Cases 2 & 3 - uncle is BLACK */
				if (node == node->parent->right) {
					/* Reduce case 2 to case 3 */
					node = node->parent;
					root = rbtree_left_rotate(root, node);
				}
				/* Case 3 */
				node->parent->color = COLOR_BLACK;
				node->parent->parent->color = COLOR_RED;
				root = rbtree_right_rotate(root,
				node->parent->parent);
			}
		} else {
/*
* Node in right tree of grandfather
*
* Cases 4, 5, 6 - node in right tree
* of grandfather
*/
			uncle = node->parent->parent->left;
			if (uncle->color == COLOR_RED) {
				/* Uncle is RED – case 4 */
				node->parent->color = COLOR_BLACK;
				uncle->color = COLOR_BLACK;
				node->parent->parent->color = COLOR_RED;
				node = node->parent->parent;
			} else {
			/* Uncle is BLACK */ 
			if (node == node->parent->left) {
				node = node->parent;
				root = rbtree_right_rotate(root, node);
			}
		
			node->parent->color = COLOR_BLACK;
			node->parent->parent->color = COLOR_RED;
			root = rbtree_left_rotate(root,
			node->parent->parent);
			}
		}
	}
	
	root->color = COLOR_BLACK;
	
	return root;
}

// Левый поворот поддерева. 
struct rbtree *rbtree_left_rotate(struct rbtree *root, struct rbtree *node) {
	
	struct rbtree *right = node->right;
	/* Create node->right link */
	node->right = right->left;
	if (right->left != NullNode)
		right->left->parent = node;
		/* Create right->parent link */
	if (right != NullNode)
		right->parent = node->parent;
	if (node->parent != NullNode) {
		if (node == node->parent->left)
			node->parent->left = right;
		else
			node->parent->right = right;
	} else root = right;

	right->left = node;
	if (node != NullNode)
		node->parent = right;
	return root;
}

// Правый поворот поддерева. 
struct rbtree *rbtree_right_rotate(struct rbtree *root, struct rbtree *node) {
	
	struct rbtree *left = node->left;
	/* Create node->left link */
	node->left = left->right;
	if (left->right != NullNode)
		left->right->parent = node;
		/* Create left->parent link */
	if (left != NullNode)
		left->parent = node->parent;
	if (node->parent != NullNode) {
		if (node == node->parent->right)
			node->parent->right = left;
		else node->parent->left = left;
	} else root = left;

	left->right = node;
	if (node != NullNode)
		node->parent = left;
	return root;
}

