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
#include <sys/time.h> // Для измерения времени работы. 

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

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
	
	double t;

    t = wtime();// Измерим время
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
	t = wtime() - t;

    printf("Elapsed time: %.6f sec.\n", t);
	
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
// На вход получаем два дерева. Изначальное и новое. 
struct rbtree *rbtree_fixup_add(struct rbtree *root, struct rbtree *node) {

	// Создаем дерево для хранения
	// дяди. 
	struct rbtree *uncle;
	// Если родительский узел красный
	while (node != root && node->parent->color == COLOR_RED) {
		// Если родительский элемент текущего 
		// находится слева от дедушки
		if (node->parent == node->parent->parent->left) {

			// Получаем узел "Дядя"
			uncle = node->parent->parent->right;
			
			// Случай 1 - Если дядя красный 
			// В 1 случае необходимо сменить цвет предков
			if (uncle->color == COLOR_RED) {
				
				// Меняем цвет отца и дяди на черный
				node->parent->color = COLOR_BLACK; 		// родительский
				uncle->color = COLOR_BLACK;				// Дядин
				node->parent->parent->color = COLOR_RED;// дедушки
				node = node->parent->parent;
			} else { // Случай 2 и 3 - Если Дядя черный
				// Если текущее дерево справа от родительского
				if (node == node->parent->right) {
					// Сменить случай со 2 на 3. 
					// Текущий узел становится родителем.
					// Родительский становится левым дочерним.
					node = node->parent; // Происзводим левый поворот.
					root = rbtree_left_rotate(root, node);
				}
				// Случай 3 - текущий 
				node->parent->color = COLOR_BLACK;
				node->parent->parent->color = COLOR_RED;
				// Правый поворот. 
				root = rbtree_right_rotate(root,node->parent->parent);
			}
		} else { // Если родительский элемент текущего
				 // находится справа от дедушки
			uncle = node->parent->parent->left;
			if (uncle->color == COLOR_RED) {
				// Случай 4 - Дядя - красный
				// Меняемм цвета дяди, отца и деда.
				node->parent->color = COLOR_BLACK;
				uncle->color = COLOR_BLACK;
				node->parent->parent->color = COLOR_RED;
				node = node->parent->parent;
			} else {
			// Дядя - черынй
			if (node == node->parent->left) { // Случай 5
				node = node->parent;
				// Правый поворот - Случай 5 превращаем в 6
				root = rbtree_right_rotate(root, node);
			}
			// Случай 6 - Меняем цвета и делаем левый поворот
			node->parent->color = COLOR_BLACK;
			node->parent->parent->color = COLOR_RED;
			root = rbtree_left_rotate(root, node->parent->parent);
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

