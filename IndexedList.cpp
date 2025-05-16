/*
This data structure is my own idea.
It is not a common or well-known one.

IndexedList is a data structure designed to maintain a list of elements with efficient 
indexed access, insertion, and removal operations. It internally uses a linked list of blocks, 
where each block contains a linked list of nodes, allowing the structure to balance itself 
and maintain roughly O(sqrt(N)) complexity for insertions, removals, and indexed accesses.

- get(index): O(sqrt(N))
- set(index, value): O(sqrt(N))
- insert(index, value): O(sqrt(N))
- remove(index): O(sqrt(N))
- printall(): O(N)
- size(): O(1)

This makes IndexedList suitable for scenarios where frequent insertions/removals occur 
at arbitrary positions and fast indexed access is required, better than a simple linked list.

@fortesting
https://algoleague.com/problem/indexing-problem/detail
*/

#include <iostream>
#include <string>
#include <cmath>
#include <cassert>

template<typename T>
class IndexedList{
private:
    class ILException{
    public:
        ILException(std:: string exception = "") : _text(exception)   {   
        std :: cout << _text << std :: endl;
        }
        std:: string _text;
    };

    class ILNode{
    public:
        T data;
        ILNode *prev, *next;
        ILNode(){
            data = 0;
            prev = NULL;
            next = NULL;
        }
        ILNode(T data){
            this->data = data;
            prev = NULL;
            next = NULL;
        }
        ILNode(T data, ILNode *prev, ILNode *next){
            this->data = data;
            this->prev = prev;
            this->next = next;
        }
    };

    class ILBlock{
    public:
        ILBlock(){
            size = 0;
            prev = NULL;
            next = NULL;
            head = NULL;
        }
        void clear(){
            ILNode *temp;
            while(head!=NULL){
                temp = head->next;
                delete head;
                head = temp;
            }
        }
        ILBlock *prev, *next;
        ILNode *head;
        size_t size;
    };
    
public:
    IndexedList(){
        total_size = 0;
        head = new ILBlock;
    }
    
    ~IndexedList(){
        ILBlock *block = head, *temp;
        while(block != NULL){
            temp = block->next;
            block->clear();
            delete block;
            block = temp;
        }
    }
    
    // Returns the element at the specified index.
    // Throws an exception if index is out of bounds.
    T get(size_t index){
        if(index > total_size or index < 0){
            throw ILException("get error: List index out of range");
        }
        if(index == 0){
            return head->head->data;
        }
        else{
            size_t cur = 0;
            ILBlock *block = head;
            while(cur + block->size - 1 < index){
                cur += block->size;
                block = block->next;
            }
            ILNode *node = block->head;
            while(cur < index){
                node = node -> next;
                cur++;
            }
            return node->data;
        }
    }
    
    // Sets the element at the specified index to the given data.
    // Throws an exception if index is out of bounds.
    void set(size_t index, T data){
        if(index > total_size or index < 0){
            throw ILException("set error: List index out of range");
        }
        if(index == 0){
            head->head->data = data;
            return;
        }
        else{
            size_t cur = 0;
            ILBlock *block = head;
            while(cur + block->size - 1 < index){
                cur += block->size;
                block = block->next;
            }
            ILNode *node = block->head;
            while(cur < index){
                node = node -> next;
                cur++;
            }
            node->data = data;
            return;
        }
    }
    
    // Inserts a new element with the given data at the specified index.
    // Throws an exception if index is out of bounds.
    void insert(size_t index, T data){
        if(index > total_size or index < 0){
            throw ILException("insert error: List index out of range");
        }
        size_t cur = 0;
        ILBlock *block = head;
        if(index == 0){
            block->head = new ILNode(data,NULL,block->head);
        }
        else{
            while(cur + block->size < index){
                cur += block->size;
                block = block->next;
            }
            ILNode *node = block->head;
            while(cur < index-1){
                node = node -> next;
                cur++;
            }
            node->next = new ILNode(data, node, node->next);
            node = node->next;
            if(node->next) node->next->prev = node;
        }
        block->size ++;
        total_size ++;
        balance();
    }
    
    // Removes the element at the specified index.
    // Throws an exception if index is out of bounds.
    void remove(size_t index){
        if(index > total_size or index < 0){
            throw ILException("remove error: List index out of range");
        }
        
        if(index == 0){
            ILNode *node = head->head->next;
            delete head->head;
            head->head = node;
            if(node) node->prev = NULL;
            head->size --;
        }
        else{
            size_t cur = 0;
            ILBlock *block = head;
            while(cur + block->size - 1 < index){
                cur += block->size;
                block = block->next;
            }
            if(cur == index){
                ILNode *node = block->head->next;
                delete block->head;
                block->head = node;
                if(node)node->prev = NULL;
                block->size --;
            }
            else{
                ILNode *node = block->head;
                while(cur < index){
                    node = node -> next;
                    cur++;
                }
                if(node->prev){
                    node->prev->next = node->next;
                }
                if(node->next){
                    node->next->prev = node->prev;
                }
                delete node;
                block->size --;
            }
        }
        total_size--;
        balance();
    }
    
    // Prints all elements in the list.
    // If split is true, prints blocks separately with block info.
    void printall(bool split = false){
        ILBlock *block = head;
        ILNode *node;
        size_t block_num = 0;
        while(block != NULL){
            node = block->head;
            if(split) std :: cout << block_num << " ("<< block->size << ") -> ";
            while(node != NULL){
                std :: cout << node->data << " ";
                node = node->next;
            }
            if(split) std :: cout << '\n';
            block = block->next;
            block_num++;
        }
        if(!split) std :: cout << '\n';
    }
    
    // Returns the current total number of elements in the list.
    size_t size(){
        return total_size;
    }
    
private:
    size_t total_size;
    ILBlock *head;
    
    void balance(){
        size_t tresh = sqrt(total_size);
        if(tresh < 2) tresh = 2;
        while(head != NULL and head->size == 0){
            ILBlock *temp = head->next;
            delete head;
            head = temp;
        }
        if(head == NULL){
            head = new ILBlock;
            return;
        }
        ILBlock *block = head;
        head -> prev = NULL;
        while(block != NULL){
            if(block->size >= 2*tresh){
                ILNode *node = block->head;
                size_t cur = 1;
                while(cur<tresh){
                    node = node->next;
                    cur ++;
                }
                node->next->prev = NULL;
                ILBlock *new_block = new ILBlock;
                new_block->head = node->next;
                new_block->size = block->size - tresh;
                node->next = NULL;
                new_block->prev = block;
                new_block->next = block->next;
                if(new_block->next) new_block->next->prev = new_block;
                block->next = new_block;
                block->size = tresh;
                block = block->next;
            }
            else if(block->next and block->next->size == 0){
                ILBlock *temp = block->next->next;
                delete block->next;
                block->next = temp;
            }
            else if(block->next and block->size + block->next->size < tresh){
                block->size += block->next->size;
                ILNode *node = block->head;
                ILBlock *del = block->next;
                while(node->next != NULL){
                    node = node->next;
                }
                node->next = del->head;
                if(node->next) node->next->prev = node;
                block->next = block->next->next;
                if(block->next) block->next->prev = block;
                delete del;
            }
            else{
                block = block->next;
            }
        }
    }
};

