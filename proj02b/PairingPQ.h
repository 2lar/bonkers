// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure to
            // initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, prev{ nullptr}
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *prev;
            // TODO: Add one extra pointer (parent or previous) as desired.
    }; // Node


    // Description: Construct an empty pairing heap with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root(nullptr), ncount(0) {
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root(nullptr), ncount(0) {
        while (start != end)
            push(*start++);
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare }, root(nullptr), ncount(0) {
        if (other.ncount != 0){
            std::deque<Node*> copier;
            Node* aux = other.root;
            copier.push_back(aux);

            while (!copier.empty()){
                aux = copier.front();
                copier.pop_front();

                if (aux->sibling) copier.push_back(aux->sibling);
                if (aux->child) copier.push_back(aux->child);

                push(aux->elt);
            }
        }

    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ &operator=(const PairingPQ & rhs) {
        PairingPQ aux(rhs);
        std::swap(ncount, aux.ncount);
        std::swap(root, aux.root);
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        if (empty()) return;
        if (ncount == 1){
            ncount = 0;
            delete root;
            root = nullptr;
            return;
        }
        std::deque<Node*> cont;
        cont.push_back(root);
        Node* aux = nullptr;
        while(!cont.empty()){
            aux = cont.front();
            cont.pop_front();
            if (aux->sibling){
                cont.push_back(aux->sibling);
            }
            if (aux->child){
                cont.push_back(aux->child);
            }
            delete aux;
            --ncount;
        }

    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out of order and
    //              'rebuilds' the pairing heap by fixing the pairing heap invariant.
    //              You CANNOT delete 'old' nodes and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        if (!root) return;
        if (ncount == 2){
            root = meld(root, root->child);
            return;
        }

        std::deque<Node*> cont;
        cont.push_back(root);
        root = nullptr;

        while (!cont.empty()){
            Node *aux = cont.front();
            cont.pop_front();
            if (aux->child){
                cont.push_back(aux->child);
                aux->child = nullptr;
            }
            if (aux->sibling){
                cont.push_back(aux->sibling);
                aux->sibling = nullptr;
            }
            aux->prev = nullptr;
            updateElt(aux, aux->elt);
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already done.
    //              You should implement push functionality entirely in the addNode()
    //              function, and this function calls addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the pairing heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the pairing heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        std::deque<Node*> cont;
        Node* aux = root->child;
        if (aux){
            aux->prev = nullptr;
            cont.push_back(aux);
        }
        delete root;
        root = nullptr;
        if (--ncount == 0) return;
        
        while(aux->sibling){
            aux = aux->sibling;
            cont.push_back(aux);
        }
        while (cont.size() > 1){
            cont[0]->sibling = nullptr; cont[1]->sibling = nullptr;
            cont[0]->prev = nullptr; cont[1]->prev = nullptr;
            cont.push_back(meld(cont[0], cont[1]));
            cont.pop_front(); cont.pop_front();
        }
        root = cont.front();
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return root->elt;
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return ncount;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return (ncount == 0);
    } // empty()


    // Description: Updates the priority of an element already in the pairing heap by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE & new_value) {
        if (this->compare(node->elt, new_value))
            node->elt = new_value;

        if (node->sibling)
            node->sibling->prev = node->prev;
        
        if (node->prev){
            if (node->prev->child == node){
                if (node->sibling)
                    node->prev->child = node->sibling;
                else
                    node->prev->child = nullptr;
            }else{
                if (node->sibling)
                    node->prev->sibling = node->sibling;
                else
                    node->prev->sibling = nullptr;
            }
        }
        node->prev = nullptr;
        node->sibling = nullptr;
        root = meld(root, node);

    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE & val) {
        Node *thenode = new Node(val);
        root = meld(thenode, root);
        ncount++;
        return thenode;
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).

    // NOTE: For member variables, you are only allowed to add a "root pointer"
    //       and a "count" of the number of nodes.  Anything else (such as a deque)
    //       should be declared inside of member functions as needed.

    Node* root = nullptr;
    size_t ncount = 0;

    Node *meld(Node *x, Node *y){
        if (y == nullptr) return x;
        if (x == nullptr) return y;
        if (x == y) return x;
        if (this->compare(x->elt, y->elt)){
            if (y->child){
                x->sibling = y->child;
                y->child->prev = x;
                
            }
            x->prev = y;
            y->child = x;
            return y;
        }else{
            if (x->child){
                y->sibling = x->child;
                x->child->prev = y;
                
            }
            y->prev = x;
            x->child = y;
            return x;
        }
    }
};


#endif // PAIRINGPQ_H
