// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6

/*
 * Compile this test against your .h files to make sure they compile. Note how
 * the eecs281 priority queues can be constructed with the different types. We
 * suggest adding to this file or creating your own test cases to test your
 * priority queue implementations more thoroughly.  If you do./ not call a
 * function from here, it is NOT compiled due to templates!  So for instance,
 * if you don't add code to call updatePriorities(), you could have compiler
 * errors that you don't even know about.
 *
 * Our makefile will build an executable named testPQ if you type 'make testPQ'
 * or 'make alltests' (without the quotes).
 *
 * Notice that testPairing() tests the range-based constructor but main() and
 * testPriorityQueue() do not.  Make sure to test the range-based constructor
 * for other PQ types, and also test the PairingPQ-specific member functions.
 *
 * This is NOT a complete test of your priority queues, you have to add code
 * to do more testing!
 *
 * You do not have to submit this file, but it won't cause problems if you do.
 */

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "Eecs281PQ.h"
#include "BinaryPQ.h"
#include "UnorderedPQ.h"
#include "PairingPQ.h"
#include "SortedPQ.h"

using namespace std;


// Used to test a priority queue containing pointers to integers.
struct IntPtrComp {
    bool operator() (const int *a, const int *b) const {
        return *a < *b;
    }
};


// TODO: Make sure that you're using this-compare() properly, and everywhere
// that you should.  Complete this function by adding a functor that compares
// two HiddenData structures, create a PQ of the specified type, and call
// this function from main().
void testHiddenData(const string &pqType) {
    struct HiddenData {
        int data;
    };
    struct HiddenDataComp {
        bool operator()(const HiddenData &/*a*/, const HiddenData &/*b*/) const {
            // TODO: Finish this functor; when you do, uncomment the
            // parameters in the line above
            return false;
        }
    };

    cout << "Testing " << pqType << " with hidden data" << endl;

    // TODO: Add code here to actually test with the HiddenData type.
} // testHiddenData()


// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePrioritiesHelper(Eecs281PQ<int *, IntPtrComp> *pq) {
    vector<int> data;
    data.reserve(100);
    data.push_back(1);
    data.push_back(5);
    for (int i = 0; i < 13; i++) data.push_back(i+3);

    // NOTE: If you add more data to the vector, don't push the pointers
    // until AFTER the vector stops changing size!  Think about why.
    // You can add up to 100 values, or change the reserve if you want more.

    for (size_t i = 0; i < data.size(); ++i) {
        pq->push(&data[i]);
    } // for

    for (int i = 0; i < 15; i++) data[i] = data[i]+3;

    // Change the first value (which is pointed to by the pq), and check it.
    data[0] = 10;
    pq->updatePriorities();
    assert(*pq->top() == 10);
    for (int i = 0; i < 3; i++) pq->pop();
    assert(*pq->top() == 16);
    std::cout << "It finished TEST UPDATE PRIORITY HELPER\n";
} // testUpdatePrioritiesHelper()


// TODO: Add more code to this function to test if updatePriorities()
// is working properly.
void testUpdatePriorities(const string &pqType) {
    Eecs281PQ<int *, IntPtrComp> *pq = nullptr;
    cout << "Testing updatePriorities() on " << pqType << endl;

    if (pqType == "Unordered") {
        pq = new UnorderedPQ<int *, IntPtrComp>;
    } // if
    // TODO: Add more types here inside 'else if' statements, like in main().
    else if(pqType == "Pairing")
        pq = new PairingPQ<int*, IntPtrComp>;
    else if(pqType == "Binary")
        pq = new BinaryPQ<int*, IntPtrComp>;
    else if (pqType == "Sorted")
        pq = new SortedPQ<int*, IntPtrComp>;
    if (!pq) {
        cout << "Invalid pq pointer; did you forget to create it?" << endl;
        return;
    } // if

    testUpdatePrioritiesHelper(pq);
    delete pq;
    std::cout << "it finished TEST UPDATE PRIORITIES\n";
} // testUpdatePriorities()


// Very basic testing.
void testPriorityQueue(Eecs281PQ<int> *pq, const string &pqType) {
    cout << "Testing priority queue: " << pqType << endl;

    pq->push(3);
    pq->push(4);
    assert(pq->size() == 2);
    assert(pq->top() == 4);

    pq->pop();
    assert(pq->size() == 1);
    assert(pq->top() == 3);
    assert(!pq->empty());

    pq->pop();
    assert(pq->size() == 0);
    assert(pq->empty());

    // TODO: Add more testing here!
    for (int i = 0; i < 5; i++){
        pq->push(i);
        assert(pq->top() == i);
    }
    int aa [] = {12,19,17,55,99};
    for (int x: aa) pq->push(x);
    assert(pq->top() == 99);
    for (int i = 0; i != 2; i++) pq->pop();
    assert(pq->top() == 19);

    cout << "testPriorityQueue() succeeded!" << endl;
} // testPriorityQueue()


// Test the pairing heap's range-based constructor, copy constructor,
// and operator=().
//
void testPairing(vector<int> & vec) {
    cout << "Testing Pairing Heap separately" << endl;
    Eecs281PQ<int> * pq1 = new PairingPQ<int>(vec.begin(), vec.end());
    Eecs281PQ<int> * pq2 = new PairingPQ<int>(*((PairingPQ<int> *)pq1));
    // This line is different just to show two different ways to declare a
    // pairing heap: as an Eecs281PQ and as a PairingPQ. Yay for inheritance!
    PairingPQ<int> * pq3 = new PairingPQ<int>();
    *pq3 = *((PairingPQ<int> *)pq2);

    pq1->push(3);
    pq2->pop();
    assert(pq1->size() == 3);
    assert(!pq1->empty());
    assert(pq1->top() == 3);
    pq2->push(pq3->top());
    assert(pq2->top() == pq3->top());

    cout << "Basic tests done." << endl;
    // TODO: Add more code to test addNode(), updateElt(), etc.

    //another ranged based construction
    vector<int> ranger;
    for (int i = 1; i != 1001; i++) ranger.push_back(i);
    Eecs281PQ<int> * pqr = new PairingPQ<int>(ranger.begin(), ranger.end());
    
    assert(pqr->size() == 1000); assert(pqr->top() == 1000);
    for (int i = 100; i != 0; i--) pqr->pop();
    assert(pqr->size() == 900); assert(pqr->top() == 900);
    std::cout << "PQR SIZE: " << pqr->size() << "\n";
    std::cout << "PQR TOP: " << pqr->top() << "\n";
    std::cout << "FINISHED TESTING SOME RANGED BASED \n";

    //general stuff for testing
    PairingPQ<int>* a = new PairingPQ<int>();
    PairingPQ<int>* b = new PairingPQ<int>();
    PairingPQ<int>* c = new PairingPQ<int>();
    PairingPQ<int>* d = new PairingPQ<int>();

    // TESTING addNode()
    a->push(9); a->push(8); a->push(7);
    assert(a->size() == 3); assert(a->top() == 9);

    auto *ba = b->addNode(9); auto *bb = b->addNode(8);
    assert(b->size() == 2); assert(b->top() == 9);

    c->push(9); auto *ca = c->addNode(88);
    assert(c->size() == 2); assert(c->top() == 88);

    //TEST updateElt()
    b->updateElt(ba, 9000);
    assert(b->size() == 2); assert(b->top() == 9000);
    b->updateElt(bb, 8999);
    assert(b->size() == 2); assert(b->top() == 9000);

    c->updateElt(ca, 990);
    assert(c->size() == 2); assert(c->top() == 990);

    //TEST pop()
    a->pop();
    assert(a->size() == 2); assert(a->top() == 8);
    b->pop();
    assert(b->size() == 1); assert(b->top() == 8999);
    b->pop(); assert(b->empty());

    //TEST BIG POP TOP AND PUSH
    for (int i = 1; i < 101; i++) d->push(i);
    d->push(9998);
    for (int i = 101; i < 200; i++) d->push(i);
    assert(d->size() == 200); assert(d->top() == 9998);
    std::cout << "This is d size: " << d->size() << endl;
    std::cout << "This is d top: " << d->top() << endl;
    d->push(19999); d->push(823718); d->push(923180);
    std::cout << "This is d top2: " << d->top() << " should be 923180" << endl;
    std::cout << "d size2: " << d->size() << " should be 203" << endl;
    d->pop();
    std::cout << "This is d top3: " << d->top() << " should be 823718" << endl;
    std::cout << "d size3: " << d->size() << " should be 202" << endl;
    cout << "Calling destructors" << endl;
    delete pq1;
    delete pq2;
    delete pq3;
    delete a;
    delete b;
    delete c;
    delete d;
    delete pqr;
    cout << "testPairing() succeeded" << endl;
} // testPairing()

void testBinary(vector<int> & vec){
    cout << "Testing Binary PQ\n";
    Eecs281PQ<int> * pq = new BinaryPQ<int>(vec.begin(), vec.end());

    // cout << "this the size: " << pq->size() << endl;
    // pq->pop();
    // cout << "Top should be 1: " << pq->top() << endl;
    // for (int i = 2; i < 11; i++) {pq->push(i); assert(pq->top() == i);}
    // cout << "Top2 should be 10: " << pq->top() << endl;
    // vector<int> x = {9032,3215,35348,32144,948321};
    // for (int e : x) pq->push(e);
    // cout << "Top3 should be 948321: " << pq->top() << endl;
    // for (int i = 3; i != 0; i--) pq->pop();
    // cout << "Top4 should be 9032: " << pq->top() << endl;

    cout << "this is size: " << pq->size() << endl;
    for (int i = 2; i < 100; i++) pq->push(i);
    cout << "This is size2: " << pq->size() << endl;
    cout << "this is top: " << pq->top() << endl;
    vector<int> aa = {48329,3429890,432432,43289};
        //43289, 48329, 432432, 3429890
    for (int q : aa) pq->push(q);
    cout << "This is size: " << pq->size() << endl;
    cout << "This is top2: " << pq->top() << endl; // 3429890
    pq->pop(); pq->pop(); pq->pop();
    cout << "This is size2: " << pq->size() << endl;
    cout << "This is top3: " << pq->top() << endl; //43289

    
}


int main() {
    // Basic pointer, allocate a new PQ later based on user choice.
    Eecs281PQ<int> *pq;
    vector<string> types{ "Unordered", "Sorted", "Binary", "Pairing" };
    unsigned int choice;

    cout << "PQ tester" << endl << endl;
    for (size_t i = 0; i < types.size(); ++i)
        cout << "  " << i << ") " << types[i] << endl;
    cout << endl;
    cout << "Select one: ";
    cin >> choice;

    if (choice == 0) {
        pq = new UnorderedPQ<int>;
    } // if
    else if (choice == 1) {
        pq = new SortedPQ<int>;
    } // else if
    else if (choice == 2) {
        pq = new BinaryPQ<int>;
    } // else if
    else if (choice == 3) {
        pq = new PairingPQ<int>;
    } // else if
    else {
        cout << "Unknown container!" << endl << endl;
        exit(1);
    } // else
   
    testPriorityQueue(pq, types[choice]);
    testUpdatePriorities(types[choice]);

    if (choice == 3) {
        vector<int> vec;
        vec.push_back(0);
        vec.push_back(1);
        testPairing(vec);
    } // if

    if (choice == 2){
        vector<int> vec;
        vec.push_back(0);
        vec.push_back(1);
        testBinary(vec);
    }

    // Clean up!
    delete pq;

    return 0;
} // main()
