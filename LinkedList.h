/**
 * @file LinkedList.h
 * @author Jan Wielgus
 * @brief One-way, dynamically allocated linked list class.
 * @date 2020-09-01
 * 
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "IList.h"

// Issues:
// FIXME: Removing the last element is not effective. I don't know if there is anything that can be done in one-way linked list.

// TODO: think about using sentinel (wartownik) in this implementation.



template <class T>
class LinkedList;



template <class T>
class Node
{
public:
    T data;
    Node<T>* next = nullptr;
};



template <class T>
class LinkedListIterator : public RemovingIterator<T>
{
private:
    Node<T> predecessorNode;
    Node<T>* currentNode = &predecessorNode; // currentNode HAVE TO ALWAYS BE NOT NULL
    LinkedList<T>* linkedList;
    T nullElement;

public:
    /**
     * @brief Construct the linked list iterator for specified linked list.
     * @param linkedList pointer to the linked list 
     */
    explicit LinkedListIterator(LinkedList<T>* linkedList);

    LinkedListIterator(const LinkedListIterator& other) = delete; // do not allow copying this class
    LinkedListIterator& operator=(const LinkedListIterator& other) = delete;


    /**
     * @return true if iterator is not at the end and next() method can be used.
     * If returned false, don't use next method().
     */
    bool hasNext() override
    {
        return currentNode->next != nullptr;
    }


    /**
     * @return pointer to next data or nullptr if there is no next data.
     * Check if there are any data using hasNext() method first!
     */
    T& next() override
    {
        if (currentNode->next == nullptr)
            return nullElement;
        
        currentNode = currentNode->next;
        return currentNode->data;
    }


    /**
     * @brief Remove last element returned by next() method.
     * Cannot be used several times in a row. next() have to be always used before removing an element.
     * @return false if used without next() before, list is empty or there is no elements to remove.
     * Returns true if element was removed.
     */
    bool remove() override
    {
        if (currentNode == &predecessorNode) // trying to remove element before calling next()
            return false;
        
        Node<T>* nextNodeBackup = currentNode->next;
            
        linkedList->removeNode(currentNode);

        predecessorNode.next = nextNodeBackup;
        currentNode = &predecessorNode;
        return true;
    }


    /**
     * @brief Sets the iterator to the linked list beginning (if is empty, thats ok).
     */
    void reset();


    friend class LinkedList<T>;
};



template <class T>
class LinkedList : public IList<T>
{
private:
    Node<T>* root = nullptr;
    Node<T>* tail = nullptr;
    size_t size = 0;
    LinkedListIterator<T> iteratorInstance;

    T nullElement; // element returned for example when used get() on empty list

    friend class LinkedListIterator<T>;


public:
    LinkedList()
        : iteratorInstance(this)
    {
    }


    // TODO: implement copy constructor and assignment operator
    LinkedList(const LinkedList& other) = delete;
    LinkedList& operator=(const LinkedList& other) = delete;


    bool add(const T& item) override
    {
        if (root == nullptr)
        {
            root = new Node<T>();
            root->data = item;
            tail = root;
        }
        else
        {
            tail->next = new Node<T>();
            tail = tail->next;
            tail->data = item;
        }
        
        size++;
        return true;
    }

    
    bool add(const T& item, size_t index) override
    {
        if (index > size)
            return false;
        
        bool returnFlag = true;

        if (root == nullptr || index == size)
            returnFlag = add(item);
        else // neither first nor last element
        {
            Node<T>* preceding = getNode(index - 1);
            Node<T>* newNode = new Node<T>();
            newNode->data = item;
            newNode->next = preceding->next;
            preceding->next = newNode;
            size++;
        }
        
        return returnFlag;
    }


    /**
     * @brief Remove element at specified index.
     * The fastest is removing the first element, slowest is removing the last one.
     * 
     * @param index Index of element to be removed from the linked list.
     * @return true if element was removed. Return false if list is empty
     * or index is out of bounds.
     */
    bool remove(size_t index) override
    {
        if (root == nullptr || index >= size)
            return false;
        
        Node<T>* toDelete;

        if (index == 0)
        {
            toDelete = root;
            root = root->next;

            if (root == nullptr)
                tail = nullptr;
        }
        else
        {
            Node<T>* preceding = getNode(index - 1);
            toDelete = preceding->next;
            preceding->next = toDelete->next;

            if (toDelete->next == nullptr) // if this was a tail node
                tail = preceding;
        }
        
        delete toDelete;
        size--;
        iteratorInstance.reset();
        return true;
    }

    
    T& get(size_t index) override
    {
        Node<T>* toReturn = getNode(index);
        return toReturn == nullptr ? nullElement : toReturn->data;
    }

    
    const T& get(size_t index) const override
    {
        Node<T>* toReturn = getNode(index);
        return toReturn == nullptr ? nullElement : toReturn->data;
    }

    
    T& operator[](size_t index) override
    {
        Node<T>* toReturn = getNode(index);
        return toReturn == nullptr ? nullElement : toReturn->data;
    }

    
    const T& operator[](size_t index) const override
    {
        Node<T>* toReturn = getNode(index);
        return toReturn == nullptr ? nullElement : toReturn->data;
    }


    Iterator<T>* getIterator() override
    {
        iteratorInstance.reset();
        return &iteratorInstance;
    }

    
    bool replace(const T& newItem, size_t index) override
    {
        Node<T>* toReplace = getNode(index);
        
        if (toReplace == nullptr)
            return false;
        
        toReplace->data = newItem;
        return true;
    }


    bool contain(const T& itemToFind) const override
    {
        for (Node<T>* node = root; node != nullptr; node = node->next)
            if (node->data == itemToFind)
                return true;
            
        return false;
    }

    
    size_t getSize() const override
    {
        return size;
    }

    
    bool isEmpty() const override
    {
        return root == nullptr;
    }


    void clear() override
    {
        Node<T>* current = root;
        while (current != nullptr)
        {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        root = nullptr;
        tail = nullptr;
        size = 0;
        iteratorInstance.reset();
    }




    RemovingIterator<T>* getRemovingIterator()
    {
        iteratorInstance.reset();
        return &iteratorInstance;
    }



private:
    Node<T>* getNode(size_t index) const
    {
        if (index >= size)
            return nullptr;

        if (index == size - 1)
            return tail;

        Node<T>* lookedFor = root;
        for (size_t i = 0; i < index; i++)
            lookedFor = lookedFor->next;
        
        return lookedFor;
    }


    /**
     * @brief Return node that is before the node passed in the parameter
     * (or nullptr if passed root or preceding node was not found).
     * @param node Node which predecessor we are looking for.
     */
    Node<T>* getPrecedingNode(const Node<T>* node)
    {
        if (node == root)
            return nullptr;

        Node<T>* precedingNode = root;
        while (precedingNode != nullptr && precedingNode->next != node)
            precedingNode = precedingNode->next;
        
        return precedingNode;
    }


    // TODO: check two remove and one getNode methods and try to simplify by merging / modifying them
    bool removeNode(const Node<T>* nodeToRemove)
    {
        if (root == nullptr || nodeToRemove == nullptr)
            return false;
        
        if (nodeToRemove == root)
        {
            root = root->next;
            
            if (root == nullptr)
                tail = nullptr;
        }
        else
        {
            Node<T>* precedingNode = getPrecedingNode(nodeToRemove);
            if (precedingNode == nullptr) // node was not found
                return false;
            
            precedingNode->next = nodeToRemove->next;

            if (nodeToRemove->next == nullptr) // if this was a tail node
                tail = precedingNode;
        }
        
        delete nodeToRemove;
        size--;
        iteratorInstance.reset();
        return true;
    }
};






template <class T>
LinkedListIterator<T>::LinkedListIterator(LinkedList<T>* linkedList)
{
    this->linkedList = linkedList;
    predecessorNode.next = nullptr;
    reset();
}


template <class T>
void LinkedListIterator<T>::reset()
{
    predecessorNode.next = linkedList->root;
    currentNode = &predecessorNode;
}


#endif
