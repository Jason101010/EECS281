// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

using namespace std;
// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure
            // to initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr}
            {}

            // Description: Allows access to the element at that Node's
            // position.  There are two versions, getElt() and a dereference
            // operator, use whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
            // TODO: Add one extra pointer (parent or previous) as desired.
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{nullptr}, numNode{0} {
        // TODO: Implement this function.
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{nullptr}, numNode{0} {
        // TODO: Implement this function.
        while(start != end){
            push(*start);
            ++start;
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare }, root{nullptr}, numNode{0} {
        // TODO: Implement this function.
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid pairing heap.
        deque<Node*> dq;
        Node* temp;
        if(other.numNode == 0){
            return;
        }
        dq.push_back(other.root);
        while(!dq.empty()){
            temp = dq.front();
            if(temp->sibling != nullptr){
                dq.push_back(temp->sibling);
            }
            if(temp->child != nullptr){
                dq.push_back(temp->child);
            }
            push(temp->elt);
            dq.pop_front();
        }
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        // lecture.
        if(&rhs == this){
            return *this;
        }
        auto temp{rhs};
        swap(this->root, temp.root);
        swap(this->numNode, temp.numNode);
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        deque<Node*> dq;
        Node* temp;
        if(numNode == 0){
            return;
        }
        dq.push_back(root);
        while(!dq.empty()){
            temp = dq.front();
            if(temp->sibling != nullptr){
                dq.push_back(temp->sibling);
            }
            if(temp->child != nullptr){
                dq.push_back(temp->child);
            }
            dq.pop_front();
            --numNode;
            delete temp;
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.deque<Node*> dq;
        Node* temp;
        deque<Node*> dq;
        if(numNode == 0){
            return;
        }
        dq.push_back(root);
        root = nullptr;
        while(!dq.empty()){
            temp = dq.front();
            if(temp->sibling != nullptr){
                dq.push_back(temp->sibling);
            }
            if(temp->child != nullptr){
                dq.push_back(temp->child);
            }
            temp->parent = nullptr;
            temp->sibling = nullptr;
            temp->child = nullptr;
            root = meld(temp, root);
            dq.pop_front();
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        Node* p = root->child;
        delete root;
        deque<Node*> dq;
        while(p != nullptr){
            dq.push_back(p);
            p = p->sibling;
        }
        while(dq.size() > 1){
            dq[0]->parent = nullptr;
            dq[0]->sibling = nullptr;
            dq[1]->parent = nullptr;
            dq[1]->sibling = nullptr;
            p = meld(dq[0], dq[1]);
            dq.pop_front();
            dq.pop_front();
            dq.push_back(p);
        }
        root = dq[0];
        --numNode;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function
        return root->elt;
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function
        return numNode;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function
        if(numNode == 0){
            return true;
        }
        return false;
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        // TODO: Implement this function
        if(node == nullptr){
            return;
        }
        node->elt = new_value;
        if(node == root){
            if(numNode == 1){
                return;
            }
            Node* placeHolder = node->child;
            node->child = node->child->sibling;
            placeHolder->sibling = nullptr;
            placeHolder->parent = nullptr;
            root = meld(placeHolder, root);
        }else if(node->parent->child == node){
            node->parent->child = node->sibling;
            node->sibling = nullptr;
            node->parent = nullptr;
            root = meld(node, root);
        }else{
            Node* temp;
            temp = node->parent->child;
            while(temp->sibling != node){
                temp = temp->sibling;
            }
            temp->sibling = node->sibling;
            node->sibling = nullptr;
            node->parent = nullptr;
            root = meld(node, root);
        }
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node* addNode(const TYPE &val) {
        // TODO: Implement this function
        Node* temp = new Node(val);
        if(numNode == 0){
            root = temp;
        }else{
            root = meld(root, temp);
        }
        ++numNode;
        return temp;
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    // require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    // papers).
    Node* root;
    uint32_t numNode;

    Node* meld(Node* a, Node* b){
        if(a == nullptr){
            return b;
        }
        if(b == nullptr){
            return a;
        }
        if(this->compare(a->elt, b->elt)){
            a->sibling = b->child;
            b->child = a;
            a->parent = b;
            return b;
        }else{
            b->sibling = a->child;
            a->child = b;
            b->parent = a;
            return a;
        }
    }
    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
};


#endif // PAIRINGPQ_H
