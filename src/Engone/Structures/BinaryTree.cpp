#include "Engone/Structures/BinaryTree.h"

#include "Engone/Logger.h"

namespace engone{
    void BinaryTree::cleanup() {
		// m_mutex.lock();
		resize(0);
		// m_mutex.unlock();
	}
	bool BinaryTree::add(Value ptr) {
		// m_mutex.lock();
		bool quit = false;
		// make sure i can add another node if i need to.
		if (capacity < head + 1) {
			if (!data) {
				if (!resize(5)) {
					quit = true;
				}
			} else {
				if (!resize((head + 1) * 2)) {
					quit = true;
				}
			}
		}
		bool out = false;
		if (quit) {
			if (getRoot()) {
				out = getRoot()->add(this, ptr);
			} else {
				rootIndex = newNode();
				if (getRoot()) {
					memset(getRoot(), 0, sizeof(BinaryNode));
					getRoot()->m_value = ptr;
					out = true;
				} else
					out = false;
			}
		}
		// m_mutex.unlock();
		return out;
	}
	// returns true if ptr was removed, false if it didn't exist
	bool BinaryTree::remove(Value ptr) {
		// m_mutex.lock();
		bool out = false;
		if (getRoot())
			out = getRoot()->remove(this, rootIndex, ptr);
		// m_mutex.unlock();
		return out;
	}
	bool BinaryTree::find(Value value) {
		// m_mutex.lock();
		bool out = false;
		if (getRoot())
			out = getRoot()->find(this, value);
		// m_mutex.unlock();
		return out;
	}
	BinaryNode* BinaryTree::getNode(uint32_t index) const {
		if (index == 0) return nullptr;
		return data + index - 1;
	}
	uint32_t BinaryTree::newNode() {
		if (capacity < head + 1) {
			return 0; // failed, resizing here is dangerous
		}
		memset(data + head, 0, sizeof(BinaryNode));
		head++;
		return head;
	}
	BinaryNode* BinaryTree::getRoot() const {
		return getNode(rootIndex);
	}
	// size is the count of nodes.
	bool BinaryTree::resize(uint32_t size) {
		if (size == 0) {
			if (data) {
				Free(data, sizeof(BinaryNode) * capacity);
				//GetTracker().subMemory<BinaryNode>(sizeof(BinaryNode) * capacity);
				data = nullptr;
				capacity = 0;
				head = 0;
				rootIndex = 0;
			}
		} else {
			if (!data) {
				BinaryNode* newData = (BinaryNode*)Allocate(sizeof(BinaryNode) * size);
				if (!newData) return false;
				//GetTracker().addMemory<TrackNode>(sizeof(TrackNode) * size);
				data = newData;
				capacity = size;
			} else {
				BinaryNode* newData = (BinaryNode*)Reallocate(data, capacity * sizeof(BinaryNode), sizeof(BinaryNode) * size);
				if (!newData) return false;
				//GetTracker().subMemory<TrackNode>(sizeof(TrackNode) * (maxCount));
				//GetTracker().addMemory<TrackNode>(sizeof(TrackNode) * (size));
				data = newData;
				capacity = size;
				if (head > capacity) head = capacity;
			}
		}
		return true;
	}
	// returns true if added, false if ptr already exists, or if something failed
	bool BinaryNode::add(BinaryTree* tree, Value ptr) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		if (m_value == ptr) return false; // could not add ptr
		if (ptr < m_value) {
			if (a) {
				return a->add(tree, ptr);
			} else {
				left = tree->newNode();
				a = tree->getNode(left);
				if (a) {
					a->m_value = ptr;
					return true;
				} else return false;
			}
		} else {
			if (b) {
				return b->add(tree, ptr);
			} else {
				right = tree->newNode();
				b = tree->getNode(right);
				if (b) {
					b->m_value = ptr;
					return true;
				} else return false;
			}
		}
	}
	bool BinaryNode::find(BinaryTree* tree, Value ptr) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		if (m_value == ptr) return false; // could not add ptr
		if (ptr < m_value) {
			if (a) {
				return a->find(tree, ptr);
			} else {
				return false;
			}
		} else {
			if (b) {
				return b->find(tree, ptr);
			} else {
				return false;
			}
		}
	}
	// the returned node was detached when you broke a node. The other node replaced the removed node.
	// returned node is nullptr if no node was detached
	bool BinaryNode::remove(BinaryTree* tree, uint32_t& ref, Value ptr) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		if (m_value == ptr) {
			uint32_t loose1 = 0;
			uint32_t loose2 = 0;
			// ALL THIS SHOULD NOT FAIL IF THERE ARE NO LOOSE NODES

			BinaryNode* cpy = tree->getNode(tree->head);// head-1 is the actual location, getNode decrements index by one. so it adds up.

			// could be nullptr because ref = loose1
			// find the node that refers to the last node and change it to refer to this
			BinaryNode* root = tree->getRoot();
			if (root) {
				if (root->m_value == cpy->m_value) tree->rootIndex = ref;
				else root->replace(tree, cpy->m_value, ref);
			}
			if (ref % 2 == 0) { // for better distribution
				if (a) {
					loose1 = left;
					loose2 = right;
				} else if (b) {
					loose1 = right;
				}
			} else {
				if (b) {
					loose1 = right;
					loose2 = left;
				} else if (a) {
					loose1 = left;
				}
			}

			uint32_t oldRef = ref; // for debugging

			// attach a loose node to the parent
			ref = loose1;

			//*this = *(tree->data + tree->head - 1);
			// move last node to memory at this

			// remove last node
			tree->head--;

			BinaryNode old = *this; // copy for debugging

			m_value = cpy->m_value;
			left = cpy->left;
			right = cpy->right;

			if (loose2) // no need to attach nullptr
				tree->getRoot()->reattach(tree, loose2); // reattach to parent node is enough but i don't have access to it so root will do.

			return true;
		} else if (ptr < m_value) {
			if (a)
				return a->remove(tree, left, ptr);
		} else if (ptr > m_value) {
			if (b)
				return b->remove(tree, right, ptr);
		}
		return false;
	}
	void BinaryNode::replace(BinaryTree* tree, Value ptr, uint32_t index) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		if (ptr < m_value) {
			if (a) {
				if (a->m_value == ptr) {
					left = index;
				} else {
					a->replace(tree, ptr, index);
				}
			}
		} else if (ptr > m_value) {
			if (b) {
				if (b->m_value == ptr) {
					right = index;
				} else {
					b->replace(tree, ptr, index);
				}
			}
		} else {
			// do nothing, replacing yourself doesn't work
			log::out << log::RED << "replaced self\n"; // hasn't happened, hopefully won't
		}
	}
	void BinaryNode::reattach(BinaryTree* tree, uint32_t index) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		BinaryNode* node = tree->getNode(index);
		if (node->m_value < m_value) {
			if (a) {
				a->reattach(tree, index);
			} else {
				left = index;
			}
		} else if (node->m_value > m_value) {
			if (b) {
				b->reattach(tree, index);
			} else {
				right = index;
			}
		} else {
			//tree->print();
			log::out << log::RED << "Reattach, found an equal ptr, should not happen!\n";
		}
	}
}