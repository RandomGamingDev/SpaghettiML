#ifndef LINKED_LIST_HEADER
#define LINKED_LIST_HEADER

// A simple one way linked list that stores the head and tail of the list

namespace LinkedList {
	template <typename type>
	struct Node {
		type value;
		Node<type>* next;

		Node(type value = type(), Node<type>* next = nullptr) :
			value(value),
			next(next)
		{}
	};

	template <typename type>
	struct List {
		Node<type>* head = nullptr;
		Node<type>* tail = nullptr;
		size_t length = 0;

		List() {}

		~List() {
			this->clear();
		}

		bool operator==(List<type>& other) {
			if (this->length != other.length)
				return false;
			Node<type>* i = this->head;
			Node<type>* j = other.head;
			while (i != nullptr) {
				if (i->value != j->value)
					return false;
				i = i->next;
				j = j->next;
			}
			return true;
		}

		bool operator!=(List<type>& other) {
			return !(*this == other);
		}

		void push(type value) {
			this->length++;
			if (length == 1) {
				this->head = new Node<type>(value);
				this->tail = this->head;
				return;
			}
			this->tail->next = new Node<type>(value);
			this->tail = this->tail->next;
		}

		void insert_head(type value) {
			Node<type>* oldHead = this->head;
			this->head = new Node<type>(value);
			this->head->next = oldHead;
		}

		void insert(Node<type>* node, type value) {
			Node<type>* next = node->next;
			node->next = new Node<type>(value);
			node->next->next = next;
		}

		void append(List<type>* list) {
			for (Node<type>* i = list->head; i != nullptr; i = i->next)
				this->push(i->value);
		}

		void copy(List<type>* list) {
			this->clear();
			this->append(list);
		}

		void erase_head() {
			if (length == 1) {
				this->head = new Node<type>();
				this->tail = this->head;
			}
			else {
				Node<type>* newHead = this->head->next;
				delete this->head;
				this->head = newHead;
			}
			this->length--;
		}

		void erase_next(Node<type>* node) {
			Node<type>* nextNext = node->next->next;
			if (node->next == this->tail)
				this->tail = node;
			delete node->next;
			node->next = nextNext;
			this->length--;
		}

		void clear() {
			if (this->length == 0)
				return;
			while (this->head->next != nullptr)
				this->erase_next(this->head);
			this->erase_head();
			this->length = 0;
		}
	};
};

#endif