#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
namespace aisdi
{

template <typename Type>
class LinkedList
{
  struct Node
  {
    Node() = default;
    Node(const Type &elem) : elem(elem), next(nullptr), prev(nullptr){};

    /**
     * @brief inserts itself between two other nodes, assumes that
     *        other nodes are connected
     * 
     *             ----                        ->  ----  <----
     *            |this|                       |  |this|-     |
     *             ----                        |   ----  |    |
     *                                         |  |      |    |
     *                                         |  |      |    |
     *                                         |  \/     \/   |
     *        ----   --->  -----              ----         -----
     *       |left|       |right|   =====>   |left|       |right|
     *        ----   <---  -----              ----         -----
     * 
     * @param left 
     * @param right 
     */
    void insertInBetween(Node *left, Node *right)
    {
      left->next = this;
      this->prev = left;

      right->prev = this;
      this->next = right;
    }

    void connectWith(Node *other)
    {
      next = other;
      other->prev = this;
    }

    /**
     * @brief disconnects itself from two other nodes, prev and next
     *  ->  ----  <----                          ---- 
     *  |  |this|-     |                        |this|
     *  |   ----  |    |                         ----
     *  |  |      |    |
     *  |  |      |    |        =======>
     *  |  \/     \/   |                    -----   --->  -----
     *  ----         -----                  |left|       |right|
     *  |left|       |right|                -----   <---  -----
     *  ----         -----
     * 
     */
    void disconnect()
    {
      auto left = prev, right = next;
      prev = nullptr;
      next = nullptr;
      if (left)
        left->connectWith(right);
      else if (right)
        right->connectWith(left);
    }

    Type elem;
    Node *next = nullptr;
    Node *prev = nullptr;
  };

public:
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using value_type = Type;
  using pointer = Type *;
  using reference = Type &;
  using const_pointer = const Type *;
  using const_reference = const Type &;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

  LinkedList() :guard_(new Node), _size(0)
  {
    guard_->next = guard_;
    guard_->prev = guard_;
  }

  LinkedList(std::initializer_list<Type> l) : LinkedList()
  {
    for (const auto &elem : l)
    {
      append(elem);
    }
  }

  LinkedList(const LinkedList &other) : LinkedList()
  {
    for (const auto &elem : other)
    {
      append(elem);
    }
  }

  LinkedList(LinkedList &&other) : guard_(other.guard_), _size(other._size)
  {
    other.guard_ = nullptr;
  }

  ~LinkedList()
  {
    if (guard_)
    {
      deleteNodesFrom(guard_->next, guard_);
      delete guard_;
    }
  }

  LinkedList &operator=(const LinkedList &other)
  {
    if (this == &other)
      return *this;

    deleteNodesFrom(guard_->next, guard_);
    guard_->connectWith(guard_);
    _size = 0;

    for (const auto &item : other)
      append(item);

    return *this;
  }

  LinkedList &operator=(LinkedList &&other)
  {
    if (this == &other)
      return *this;

    deleteNodesFrom(guard_->next, guard_);
    guard_->connectWith(guard_);

    std::swap(guard_, other.guard_);
    _size = other._size;

    return *this;
  }

  bool isEmpty() const { return _size == 0; }
  size_type getSize() const { return _size; }

  Type &operator[](int pos) //to delete
  {
    if (!guard_)
      throw std::runtime_error("Referencing deleted object");
    int i = 0;
    for (auto it = guard_->next; it && it != guard_; it = it->next, ++i)
    {
      if (pos == i)
        return it->elem;
    }
    throw std::out_of_range("Index out of range0");
  }

  void append(const Type &item)
  {
    Node *newElem = new Node(item);
    newElem->insertInBetween(guard_->prev, guard_);
    ++_size;
  }

  void prepend(const Type &item)
  {
    Node *newElem = new Node(item);
    newElem->insertInBetween(guard_, guard_->next);
    ++_size;
  }

  void insert(const const_iterator &insertPosition, const Type &item)
  {
    Node *newElem = new Node(item);

    auto right = iterator(insertPosition).node();
    auto left = right->prev;

    newElem->insertInBetween(left, right);
    _size++;
  }

  Type popFirst()
  {
    if (_size == 0)
      throw std::out_of_range("Popped empty list");

    auto value = pop(guard_->next);
    --_size;

    return value;
  }

  Type popLast()
  {
    if (_size == 0)
      throw std::out_of_range("Popped empty list");

    auto value = pop(guard_->prev);
    --_size;

    return value;
  }

  void erase(const const_iterator &possition)
  {
    if (_size == 0)
      throw std::out_of_range("Erasing empty list");
    if (possition == end())
      throw std::out_of_range("Erasing end iterator");

    pop(iterator(possition).node());
    --_size;
  }

  void erase(const const_iterator &firstIncluded, const const_iterator &lastExcluded)
  {
    auto itFirst = iterator(firstIncluded); //we need to non const iterator
    auto itLast = iterator(lastExcluded);

    auto first = itFirst.node(), last = itLast.node();
    first->prev->connectWith(last);

    _size -= deleteNodesFrom(first, last);
  }

  iterator begin() { return iterator(guard_->next, guard_); }
  iterator end() { return iterator(guard_, guard_); }
  const_iterator cbegin() const { return ConstIterator(guard_->next, guard_); }
  const_iterator cend() const { return const_iterator(guard_, guard_); }
  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }

private:
  Node *guard_;
  size_type _size;

  /**
 * @brief this method starts from node 'fromIncluded' and deletes
 *        every node that it encounters. Next elements are selected
 *        by calling node->next. Stops when 'toExcluded' is encountered.
 *        Returns number of elements deleted.
 * 
 * @param fromIncluded node that method starts from
 * @param toExcluded  node that
 * @return int Number of elements deleted
 */
  static int deleteNodesFrom(Node *fromIncluded, Node *toExcluded)
  {
    int elementsDeleted = 0;
    auto it = fromIncluded;
    while (it && it != toExcluded)
    {
      auto next = it->next;
      delete it;
      elementsDeleted++;
      it = next;
    }
    return elementsDeleted;
  }

  /**
   * @brief deletes a node from a list and returns it value
   * 
   * @param nodeToPop 
   * @return value of the deleted node
   */
  Type pop(Node *nodeToPop)
  {
    auto value = nodeToPop->elem;

    nodeToPop->disconnect();
    delete nodeToPop;

    return value;
  }
};

template <typename Type>
class LinkedList<Type>::ConstIterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename LinkedList::value_type;
  using difference_type = typename LinkedList::difference_type;
  using pointer = typename LinkedList::const_pointer;
  using reference = typename LinkedList::const_reference;

  explicit ConstIterator() : itr(nullptr)
  {
  }

  explicit ConstIterator(const Node *item, const Node *guard) : itr(item), guard(guard) {}

  ConstIterator(const ConstIterator &other) : itr(other.itr) {}

  reference operator*() const
  {
    if (!itr)
      throw std::out_of_range("Dereferencing empty list");
    if (itr == guard)
      throw std::out_of_range("Dereferencing end pointer");

    return itr->elem;
  }

  ConstIterator &operator++()
  {
    if (itr->next == guard->next)
      throw std::out_of_range("Incrementing end pointer");

    itr = itr->next;
    return *this;
  }

  ConstIterator operator++(int)
  {
    if (itr->next == guard->next)
      throw std::out_of_range("Incrementing end pointer");

    auto temp = *this;
    itr = itr->next;

    return temp;
  }

  ConstIterator &operator--()
  {
    if (itr->prev == guard)
      throw std::out_of_range("Decrementing end pointer");

    itr = itr->prev;
    return *this;
  }

  ConstIterator operator--(int)
  {
    if (itr->prev == guard)
      throw std::out_of_range("Decrementing end pointer");

    auto temp = *this;
    itr = itr->prev;

    return temp;
  }

  ConstIterator operator+(difference_type d) const
  {
    auto temp = itr;
    for (int i = 0; i < d; i++)
    {
      if (itr->next == guard->next)
        throw std::out_of_range("Adding iterator pass the end");

      temp = temp->next;
    }

    return ConstIterator(temp, guard);
  }

  ConstIterator operator-(difference_type d) const
  {
    auto temp = itr;
    for (int i = 0; i < d; i++)
    {
      if (itr->prev == guard)
        throw std::out_of_range("Substracting iterator pass the begining");

      temp = temp->prev;
    }

    return ConstIterator(temp, guard);
  }

  bool operator==(const ConstIterator &other) const
  {
    return itr == other.itr;
  }

  bool operator!=(const ConstIterator &other) const
  {
    return !(*this == other);
  }

  const Node *node() const
  {
    return itr;
  }

private:
  const Node *itr;
  const Node *guard;
};

template <typename Type>
class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator
{
public:
  using pointer = typename LinkedList::pointer;
  using reference = typename LinkedList::reference;

  explicit Iterator(const Node *item, const Node *guard) : ConstIterator(item, guard)
  {
  }

  Iterator(const ConstIterator &other)
      : ConstIterator(other)
  {
  }

  Iterator &operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator &operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  Iterator operator+(difference_type d) const
  {
    return ConstIterator::operator+(d);
  }

  Iterator operator-(difference_type d) const
  {
    return ConstIterator::operator-(d);
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }

  Node *node()
  {
    return const_cast<Node *>(ConstIterator::node());
  }
};

} // namespace aisdi

#endif // AISDI_LINEAR_LINKEDLIST_H
