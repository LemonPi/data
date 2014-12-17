#pragma once
#include "tree.h"

namespace sal {

template <typename T>
class Order_tree {
	struct Order_node {
		T key;
		Order_node *parent, *left, *right;
		size_t size;	// # of descendent nodes including itself = left->size + right->size + 1
		Color color;
		Order_node() : size{0}, color{Color::BLACK} {}	// sentinel construction
		Order_node(T val, Color col = Color::RED) : key{val}, parent{nil}, left{nil}, right{nil}, size{1}, color{col} {}
	};

	using NP = typename Order_tree::Order_node*;
	NP root {nil};
	static NP nil;

	// order statistics operations
	NP os_select(NP start, size_t rank) const {
		while (rank > 0) {
			size_t cur_rank {start->left->size + 1};
			if (cur_rank == rank) return start;
			else if (rank < cur_rank) start = start->left;
			else {
				start = start->right;
				rank -= cur_rank;
			}
		}
		return nil;
	}

	size_t os_rank(NP node) {
		size_t rank {node->left->size + 1};
		while (node != root) {
			if (node == node->parent->right) 
				rank += node->parent->left->size + 1;
			node = node->parent;
		}
		return rank;
	}

	// core rb utilities
	NP tree_find(NP start, T key) const {
		while (start != nil && start->key != key) {
			if (key < start->key) start = start->left;
			else start = start->right;
		}
		return start;
	}
	NP tree_min(NP start) const {
		while (start->left != nil) start = start->left;
		return start;
	}
	NP tree_max(NP start) const {
		while (start->right != nil) start = start->right;
		return start;
	}
	// successor is node with smallest key greater than start
	NP tree_successor(NP start) const {
		if (start->right) return tree_min(start->right);
		// else go up until a node that's the left child of parent
		NP parent {start->parent};
		while (parent != nil && start == parent->right) {
			start = parent;
			parent = parent->parent;
		}
		return parent;
	}
	NP tree_predecessor(NP start) const {
		if (start->left) return tree_max(start->left);
		// else go up until a node that's the right child of parent
		NP parent {start->parent};
		while (parent != nil && start == parent->left) {
			start = parent;
			parent = parent->parent;
		}
		return parent;
	}

	// assume node is colored RED
	void rb_insert(NP node) {
		tree_insert(root, node);
		rb_insert_fixup(node);
	}
	void rb_insert_fixup(NP node) {
		// node is red, either its parents are red or it is the root
		while (node->parent->color == Color::RED) {
			NP parent {node->parent};
			// grandparent is black since parent is red
			if (parent == parent->parent->left) {
				NP uncle {parent->parent->right};
				// case 1, simply recolor so parents and uncle are black while grandparent red
				if (uncle->color == Color::RED) {
					parent->color = Color::BLACK;
					uncle->color = Color::BLACK;
					parent->parent->color = Color::RED;
					// now violation (if any) is in grandparent layer
					node = parent->parent;	
				}
				// case 2 and 3, uncle is black, cannot recolor layers
				else { 
					if (node == parent->right) {
						node = parent;
						rotate_left(node);
					}
					// case 2 reduces to case 3
					parent->color = Color::BLACK;
					parent->parent->color = Color::RED;
					rotate_right(parent->parent);
					// properties after rotation must be restored
				}
			}
			// else parent is a right child, mirror the cases above
			else {
				// uncle is now the other child of grandparent
				NP uncle {parent->parent->left};
				if (uncle->color == Color::RED) {
					parent->color = Color::BLACK;
					uncle->color = Color::BLACK;
					parent->parent->color = Color::RED;
					node = parent->parent;	
				}
				else { 
					if (node == parent->left) {
						node = parent;
						rotate_right(node);
					}
					parent->color = Color::BLACK;
					parent->parent->color = Color::RED;
					rotate_left(parent->parent);
				}				
			}
		}
		// fix the terminating case of a red root, which can always be turned black
		root->color = Color::BLACK;
	}

	void rb_delete_fixup(NP successor) {
		// successor starts black-black, always has 1 extra black
		// move extra black up tree until 
		// 1. successor is red-black
		// 2. successor is root, where extra black is removed
		while (successor != root && successor->color == Color::BLACK) {
			NP parent {successor->parent};
			if (successor == parent->left) {
				// sibling cannot be nil since successor is black (so bh is at least 1)
				NP sibling {parent->right};
				// case 1, red sibling is made black, and reduces to other cases
				if (sibling->color == Color::RED) {
					// parent must be black to avoid parent-sibling both red
					sibling->color = Color::BLACK;
					parent->color = Color::RED;
					rotate_left(parent);
					sibling = parent->right;	// previously sibling's left child
				}
				// cases 2,3,4 with sibling black, depending on its children's color
				// case 2, remove a level of black off both sibling and successor
				if (sibling->left->color == Color::BLACK && sibling->right->color == Color::BLACK) {
					sibling->color = Color::RED;
					successor = parent;	// parent now double black
				}
				else {
					// case 3, sibling's left is red, switch sibling and its left child's color
					if (sibling->right->color == Color::BLACK) {
						sibling->left->color = Color::BLACK;
						sibling->color = Color::RED;
						rotate_right(sibling);
						sibling = parent->right;	// previously sibling's left child
					}
					// case 4, sibling's right is red
					sibling->color = parent->color;
					parent->color = Color::BLACK;
					sibling->right->color = Color::BLACK;
					rotate_left(parent);
					successor = root;
				}
			}
			// successor is right child, mirror above
			else {
				NP sibling {parent->left};

				if (sibling->color == Color::RED) {
					sibling->color = Color::BLACK;
					parent->color = Color::RED;
					rotate_right(parent);
					sibling = parent->left;
				}

				if (sibling->right->color == Color::BLACK && sibling->left->color == Color::BLACK) {
					sibling->color = Color::RED;
					successor = parent;	// parent now double black
				}
				else {
					if (sibling->left->color == Color::BLACK) {
						sibling->right->color = Color::BLACK;
						sibling->color = Color::RED;
						rotate_left(sibling);
						sibling = parent->left;	
					}
					sibling->color = parent->color;
					parent->color = Color::BLACK;
					sibling->left->color = Color::BLACK;
					rotate_right(parent);
					successor = root;
				}
			}
		}
		successor->color = Color::BLACK;
	}
	// moves one subtree to replace another one
	void transplant(NP old, NP moved) {
		if (old->parent == nil) root = moved;
		else if (old == old->parent->left) old->parent->left = moved;
		else old->parent->right = moved;
		// can assign to parent unconditionally due to sentinel
		moved->parent = old->parent;
		// updating moved's children is up to the caller
	}


	// rb operations modified for order statistics 
	void tree_insert(NP start, NP node) {
		NP parent {nil};
		while (start != nil) {
			++start->size;	// simply increment size of each ancestor going down
			parent = start;
			if (node->key < start->key) start = start->left;
			else start = start->right;
		}
		node->parent = parent;
		if (parent == nil) root = node;
		else if (node->key < parent->key) parent->left = node;
		else parent->right = node;
	}

	void rb_delete(NP node) {
		NP old {node};
		NP moved {node};
		NP successor;
		Color moved_original_color {moved->color};
		if (node->left == nil) {
			successor = node->right;
			transplant(node, node->right);
		}
		else if (node->right == nil) {
			successor = node->left;
			transplant(node, node->left);
		}
		else {
			moved = tree_min(node->right);
			moved_original_color = moved->color;
			successor = moved->right;
			if (moved->parent == node) successor->parent = moved;
			else {
				transplant(moved, moved->right);
				moved->right = node->right;
				moved->right->parent = moved;
			}

			transplant(node, moved);
			moved->left = node->left;
			moved->left->parent = moved;
			moved->color = node->color;
		}
		// decrement size of ancestors of moved
		moved = moved->parent;
		while (moved != nil) {
			--moved->size;
			moved = moved->parent;
		}
		if (moved_original_color == Color::BLACK) rb_delete_fixup(successor);
		delete old;
	}

	// rotations, augmented by changing child and node's sizes
	void rotate_left(NP node) {
		NP child {node->right};

		node->right = child->left;
		if (child->left != nil) child->left->parent = node;

		child->parent = node->parent;
		if (node->parent == nil) root = child;
		else if (node == node->parent->left) node->parent->left = child;
		else node->parent->right = child;

		child->left = node;
		node->parent = child;

		child->size = node->size;
		node->size = node->left->size + node->right->size + 1;	
	}
	void rotate_right(NP node) {
		NP child {node->left};

		node->left = child->right;
		if (child->right != nil) child->right->parent = node;

		child->parent = node->parent;
		if (node->parent == nil) root = child;
		else if (node == node->parent->left) node->parent->left = child;
		else node->parent->right = child;

		child->right = node;
		node->parent = child;	

		child->size = node->size;
		node->size = node->left->size + node->right->size + 1;
	}

public:
	Order_tree() = default;
	Order_tree(std::initializer_list<T> l) {
		for (const auto& v : l) insert(v);
	}
	~Order_tree() {
		postorder_walk(root, [](NP node){delete node;});
	}

	void insert(T data) {
		NP node {new Order_node(data)};
		rb_insert(node);
	};

	void erase(T data) {
		NP node {tree_find(root, data)};
		if (node != nil) rb_delete(node);
	}

	NP find(T key) {
		return tree_find(root, key);
	}

	// order statistics methods interface
	NP select(size_t rank) const {
		return os_select(root, rank);
	}
	size_t rank(NP node) {
		return os_rank(node);
	}

	// traversals, Op is a function that performs a function on a NP
	template <typename Op>
	friend void preorder_walk(NP start, Op op) {
		if (start != nil) {
			op(start);
			inorder_walk(start->left, op);
			inorder_walk(start->right, op);
		}
	}
	template <typename Op>
	friend void inorder_walk(NP start, Op op) {
		if (start != nil) {
			inorder_walk(start->left, op);
			op(start);
			inorder_walk(start->right, op);
		}
	}
	template <typename Op>
	friend void postorder_walk(NP start, Op op) {
		if (start != nil) {
			inorder_walk(start->left, op);
			inorder_walk(start->right, op);
			op(start);
		}
	}


	void print() const {
		inorder_walk(root, [](NP node){std::cout << node->key << '(' << node->size << ')' << ' ';});
		std::cout << "root: " << root->key << '(' << root->size << ')' << std::endl; 
	}
	const static NP get_nil() {
		return nil;
	}
};

template <typename T>
typename Order_tree<T>::NP Order_tree<T>::nil {new Order_node{}};

}	// end namespace sal