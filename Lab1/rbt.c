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

double wtime();

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
struct rbtree *rbtree_min(struct rbtree *root);
struct rbtree *rbtree_max(struct rbtree *root);
struct rbtree *rbtree_delete(struct rbtree *root, int key); 
struct rbtree *rbtree_lookup(struct rbtree *root, int key); 
void rbtree_print_dfs(struct rbtree *root, int level);
void rbtree_free(struct rbtree *root);  

// Функции для востановления свойств К-Ч деревьев
struct rbtree *rbtree_fixup_add(struct rbtree *root, struct rbtree *node);
struct rbtree *rbtree_left_rotate(struct rbtree *root, struct rbtree *node);
struct rbtree *rbtree_right_rotate(struct rbtree *root, struct rbtree *node);

int main(int argc, char **argv){
	
	struct rbtree *tree = NULL;
	struct rbtree *temp = NULL;
	
	double t;
	printf("Добавляем в дерево узлы\n\n");
    t = wtime();// Измерим время
	tree = rbtree_add(tree, 10, "10");
	tree = rbtree_add(tree, 5, "5");
	tree = rbtree_add(tree, 3, "3");
	tree = rbtree_add(tree, 11, "11");
	tree = rbtree_add(tree, 12, "12");
	tree = rbtree_add(tree, 6, "6");
	tree = rbtree_add(tree, 8, "8");
	tree = rbtree_add(tree, 9, "9");
	
	// ////////////////////////////////////
	tree = rbtree_add(tree, 100, "100");
	tree = rbtree_add(tree, -101, "-101");
	tree = rbtree_add(tree, 2, "2");
	tree = rbtree_add(tree, 7, "7");
	tree = rbtree_add(tree, -2, "-2");
	tree = rbtree_add(tree, -20, "-20");
	tree = rbtree_add(tree, 23, "23");
	tree = rbtree_add(tree, -1, "-1");
	tree = rbtree_add(tree, 1, "1");

	
	printf("Тестируем поиск минимума и максимума\n\n");
	temp = rbtree_min(tree); 
	printf("Минимальный узел имеет ключ: %d \n", temp->key);
	temp = rbtree_max(tree); 
	printf("Максимальный узел имеет ключ: %d \n", temp->key);
	printf("Корневой узел имеет ключ: %d \n", tree->key);
	
	printf("\nУдалим узел с ключем 100\n");
	//tree = rbtree_delete(tree, 2);
	tree = rbtree_delete(tree, 10);
	tree = rbtree_delete(tree, -20);
	tree = rbtree_delete(tree, 11);
	tree = rbtree_delete(tree, 12);
	tree = rbtree_delete(tree, 3);
	tree = rbtree_delete(tree, 23);
	tree = rbtree_delete(tree, 8);
	if (tree == NullNode)
		printf("После удаления элемента массив пуст");
	else printf("После удаления элемента в массиве остались узлы");
	
	printf("\nТестируем поиск элемента с ключем 100 \n");
	temp = rbtree_lookup(tree, 100); 
	if (temp == NullNode)
		printf("Искомого узла нет в дереве");
	else if (temp->color == COLOR_RED) 
		printf("Искомый элемент найден, он красный");
	else printf("Искомый элемент найден, он черный");
	
	printf("\nТестируем поиск элемента с ключем 120 \n");
	temp = rbtree_lookup(tree, 120); 
	if (temp == NullNode)
		printf("Искомого узла нет в дереве");
	else if (temp->color == COLOR_RED) 
		printf("Искомый элемент найден, он красный");
	else printf("Искомый элемент найден, он черный");
	
	rbtree_print_dfs(tree, 0); 
	
	printf("\n\nУдалим дерево полностью\n");
	rbtree_free(tree); 
	//rbtree_print(tree);
	t = wtime() - t;
    printf("Время работы алгоритма: %.6f секунды.\n", t);
	return 0;
}

double wtime(){
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

// Добавление нового узла в дерево
struct rbtree *rbtree_add(struct rbtree *root, int key, char *value) { 
	struct rbtree *node, *parent = NullNode;
	
	// Поиск листа для нового элемента. 
	for (node = root; node != NullNode && node != NULL; ){ 
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
	// Меняем цвет корня на черный (востанавливаем свойство)
	root->color = COLOR_BLACK;
	return root;
}

// Левый поворот поддерева. 
// Передаем текущий узел и дедушку
struct rbtree *rbtree_left_rotate(struct rbtree *root, struct rbtree *node) {
	struct rbtree *right = node->right;
	node->right = right->left;
	
	// Если левый существует
	if (right->left != NullNode)
		right->left->parent = node;
		// Создаем связь с родительским
	if (right != NullNode) 
		right->parent = node->parent;
		// Создаем связь с родительским
	if (node->parent != NullNode) {
		// меняем местами
		if (node == node->parent->left)
			node->parent->left = right;
		else
			node->parent->right = right;
	} else root = right;
	// 
	right->left = node;
	if (node != NullNode)
		node->parent = right;
	return root;
}

// Правый поворот поддерева. 
struct rbtree *rbtree_right_rotate(struct rbtree *root, struct rbtree *node) {
	struct rbtree *left = node->left;
	node->left = left->right;
	if (left->right != NullNode)
		left->right->parent = node;
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

// Поиск Минимального.
struct rbtree *rbtree_min(struct rbtree *root) {
	if (root == NullNode)
		return NullNode; 
	struct rbtree *min = root; 
	while (min->left != NullNode)
		min = min->left; 
	return min;
}

// Поиск Максимального
struct rbtree *rbtree_max(struct rbtree *root) {
	if (root == NullNode)
		return NullNode; 
	struct rbtree *max = root; 
	while (max->right != NullNode)
		max = max->right; 
	return max;
}

// Функция востановления свойств красно-черных деревьев 
// после уаления. На вход подается дерево и узел.
void RBTree_Delete_Fixup (struct rbtree *root, struct rbtree *x) {
	
	struct rbtree *w; 
	
	while ((x != root) && (x->color == COLOR_BLACK)) {
		if (x == x->parent->left) { 
			w = x->parent->right;
			// Если дядя красный
			if (w->color == COLOR_RED) {// Случай 1
				w->color = COLOR_BLACK;
				x->parent->color = COLOR_RED; 
				rbtree_left_rotate(root, x->parent);
				w = x->parent->right; 
			}
			// Если оба черные
			if ((w->left->color == COLOR_BLACK) && // Случай 2
				(w->right->color == COLOR_BLACK)) {
				w->color = COLOR_RED; 
				x = x->parent; 
			} else {
				if (w->right->color == COLOR_BLACK) { // Случай 3
					w->left->color = COLOR_BLACK; 
					w->color = COLOR_RED; 
					rbtree_right_rotate (root, w);
					w = x->parent->right;
				}
				// Случай 4
				w->color = x->parent->color; 
				x->parent->color = COLOR_BLACK; 
				w->right->color = COLOR_BLACK;
				rbtree_left_rotate(root, x->parent); 
				x = root;  
			}
			
		} 
		else { // Случаи 5-8
			w = x->parent->left;
			if (w->color == COLOR_RED) {// Случай 5
				w->color = COLOR_BLACK;
				x->parent->color = COLOR_RED; 
				rbtree_right_rotate(root, x->parent);
				w = x->parent->left; 
			}
			if ((w->right->color == COLOR_BLACK) && // Случай 6
				(w->left->color == COLOR_BLACK)) {
				w->color = COLOR_RED; 
				x = x->parent; 
			}
			else { 
				if (w->left->color == COLOR_BLACK) { // Случай 7
					w->right->color = COLOR_BLACK; 
					w->color = COLOR_RED; 
					rbtree_left_rotate (root, w);
					w = x->parent->left;
				}
				// Случай 8
				w->color = x->parent->color; 
				x->parent->color = COLOR_BLACK; 
				w->left->color = COLOR_BLACK;
				rbtree_right_rotate(root, x->parent); 
				x = root;
			}
		}	
	}
	x->color = COLOR_BLACK; 
} 

// Функция перемещения узлов
// на вход передаем корень и два узла. 
// Функция меняет местами u & v
void RBTree_Transplant(struct rbtree *root, struct rbtree *u, struct rbtree *v) { 
	// Если u - корень
	if (u->parent == NullNode)
		root = v; 
	else if (u == u->parent->left) // u - левое поддерево
		u->parent->left = v; 
	else u->parent->right = v; // u - правое поддерево 
	// Меняем родителя
	v->parent = u->parent;
}

// Удаление узла по ключу. 
struct rbtree *rbtree_delete(struct rbtree *root, int key) {
	
	// Ищем удаляемый узел, создаем ссылку и сохраняем цвет
	struct rbtree *z = rbtree_lookup(root, key), *y = z, *x; 
	int y_color = y->color; 
	
	if (z->left == NullNode) {
		// Если у удаляемого элемента нет левого
		x = z->right; 
		RBTree_Transplant (root, z , z->right); 
	} else if (z->right == NullNode) {
		// Если у удаляемого элемента нет правого
		x = z->left;
		RBTree_Transplant (root, z , z->left);
	} else { 
		// Если удаляемый имеет оба поддерева 
		y = rbtree_min(z->right);
		y_color = y->color; 
		x = y->right; 
		
		if (y->parent == z) 
			x->parent = y; 
		else { 
			RBTree_Transplant(root, y , y->right); 
			y->right = z->right;
			y->right->parent = y; 
		} 
	RBTree_Transplant(root, z, y);
	y->left = z->left; 
	y->left->parent = y; 
	y->color = z->color; 
	}
	if (y_color == COLOR_BLACK)
		RBTree_Delete_Fixup(root, x); 
	return root; 
}

// Поиск элемента
struct rbtree *rbtree_lookup(struct rbtree *root, int key) {
	struct rbtree* search = NullNode;
	
	// Если дерево пустое
	if (root == NullNode)
		return NullNode; 
	// Если значение совпало
	else if (root->key == key)
		return root; 
	else if (root->key > key) 
		search = rbtree_lookup(root->left, key);
	else if (root->key < key) 
		search = rbtree_lookup(root->right, key); 
	return search; 
}

// Очистка памяти.
void rbtree_free(struct rbtree *root){
	// Проверка на пустое дерево
	if (root == NULL || root == NullNode) 
		return; 
	// Рекурсивно запускается очистка 
	if (root->right != NullNode)
		rbtree_free(root->right);
	if (root->left != NullNode)
		rbtree_free(root->left);
		free(root);
}

// Пишем в файл все узлы и связи. 
void graph_color_print (struct rbtree *root, FILE *inputfile) {
	
	// Пишем ключ
	fprintf(inputfile, "%d", root->key);
	if (root == NullNode) return; 
	// Пишем цвет
	if (root->color == COLOR_RED) {
		fprintf(inputfile, "[color=red]\n");
	} else fprintf(inputfile, "[color=black]\n");
	// Пишем рекурсивно вместе с листьями
	if (root->left != NullNode) {
		graph_color_print(root->left,inputfile);
		fprintf(inputfile, "%d -> %d\n", root->key, root->left->key);
	} else fprintf(inputfile, "null[color=black]\n%d -> null\n", root->key);
	if (root->right != NullNode) {
		graph_color_print(root->right,inputfile);
		fprintf(inputfile, "%d -> %d\n", root->key, root->right->key);
	}else fprintf(inputfile, "null[color=black]\n%d -> null\n", root->key);
}

void rbtree_print_dfs(struct rbtree *root, int level) {
	FILE *inputfile;
	char *name = "tree.dot"; 
	inputfile = fopen(name, "w");
	
	// Готовим файл в формате tree.dot
	fprintf(inputfile, "digraph rbtree {\nnode [style=filled,fontcolor=white]\n");
	// Вызов рекурсивной функции для написания всех узлов
	graph_color_print (root, inputfile);
	fprintf(inputfile, "}");
	popen("dot -Tpng tree.dot > tree.png","r");
}


