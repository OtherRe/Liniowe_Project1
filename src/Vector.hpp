#ifndef AISDI_LINEAR_VECTOR_H
#define AISDI_LINEAR_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace aisdi
{

template <typename Type>
class Vector
{
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

  Vector() : _array(nullptr), _capacity(0), _size(0) {}
  Vector(std::initializer_list<Type> l) : _size(0)
  {
    size_t size = l.size();
    _array = new Type[size];
    _capacity = size;

    for (auto &&elem : l)
      append(elem); //&& somehow(rvalue move)?
  }
  Vector(const Vector &other) : _array(new Type[other._size]), _capacity(other._capacity), _size(0)
  {
    for (const auto &elem : other)
      append(elem);
  }
  Vector(Vector &&other) : _array(other._array), _capacity(other._capacity), _size(other._size)
  {
    other._array = nullptr;
    // other._capacity = 0;??
    // other._size = 0;??
  }
  ~Vector()
  {
    delete[] _array;
  }

  Vector &operator=(const Vector &other)
  {
    if (this == &other)
      return *this;

    delete[] _array;
    _array = new Type[other._capacity];
    _size = 0;

    for (auto &elem : other)
      append(elem);

    return *this;
  }
  Vector &operator=(Vector &&other)
  {
    if (this == &other)
      return *this;

    delete[] _array;

    _array = other._array;
    _capacity = other._capacity;
    _size = other._size;

    other._array = nullptr;

    return *this;
  }
  Type &operator[](const size_type index)
  {
    //something might happen when using move assignment or constructor
    if (_array == nullptr)
      throw std::runtime_error("Referencing deleted vector(std::move?)");

    if (index < 0 || index >= _size)
      throw std::out_of_range("Index out of range");

    return _array[index];
  }

  bool isEmpty() const { return _size == 0; }
  size_type getSize() const { return _size; }
  size_type getCapacity() const { return _capacity; }

  void append(const Type &item)
  {
    if (_size == _capacity)
      increaseCapacityBy(2);

    _array[_size++] = item;
  }
  void prepend(const Type &item)
  {
    if (_size == _capacity)
      increaseCapacityBy(2);

    moveElementsRight(0);

    _array[0] = item;
    ++_size;
  }
  void insert(const const_iterator &insertPosition, const Type &item)
  {
    //cannot dereference end() iterator
    size_type index = insertPosition == end() ? _size : &(*insertPosition) - &(*begin());

    if (_size == _capacity)
      increaseCapacityBy(2);

    moveElementsRight(index);
    _array[index] = item;
    ++_size;
  }

  Type popFirst()
  {
    if (_size == 0)
      throw std::length_error("Popped empty vector");

    Type temp = _array[0];
    moveElementsLeft(1);
    --_size;

    if (_capacity > _defaultCapacity && _size < _capacity / 4)
      decreaseCapacityBy(2);

    return temp;
  }

  Type popLast()
  {
    if (_size == 0)
      throw std::length_error("Popped empty vector");

    if (_capacity > _defaultCapacity && _size < _capacity / 4)
      decreaseCapacityBy(2);

    return _array[--_size];
  }

  void erase(const const_iterator &possition)
  {
    if (_size == 0)
      throw std::out_of_range("Erasing empty vector");

    size_type index = &(*possition) - &(*begin());
    moveElementsLeft(index + 1);
    --_size;

    if (_size < _capacity / 4 && _capacity > _defaultCapacity)
      decreaseCapacityBy(2);
  }
  void erase(const const_iterator &firstIncluded, const const_iterator &lastExcluded)
  {

    if (firstIncluded == lastExcluded)
    {
      //?????
      return;
    }

    size_type index = &(*firstIncluded) - &(*begin());
    size_type nElements = lastExcluded == end() ? _size - index : &(*lastExcluded) - &(*firstIncluded);

    //when? i guess might never happen...
    if (_size < nElements)
      throw std::out_of_range("Not enough elments");

    moveElementsLeft(index + nElements, nElements);

    _size -= nElements;
  }

  iterator       begin()        { return iterator(&(_array[0]), 0, this); }
  iterator       end()          { return iterator(&_array[_size], _size, this); }
  const_iterator cbegin() const { return const_iterator(&_array[0], 0, this); }
  const_iterator cend()   const { return const_iterator(&_array[_size], _size, this); }
  const_iterator begin()  const { return cbegin(); }
  const_iterator end()    const { return cend(); }

private:
  Type*     _array;
  size_type _capacity;
  size_type _size;

  static const size_type _defaultCapacity = 8;

  /////////////////////////////////////////////
  ///PRIVATE METHODS//////////////////////////
  ////////////////////////////////////////////
  void increaseCapacityBy(int factor)
  {
    if (_capacity == 0)
    {
      _array = new Type[8];
      _capacity = 8;
      return;
    }
    _capacity = _capacity * factor;
    changeCapacity();
  }
  void decreaseCapacityBy(int factor)
  {
    _capacity = _capacity / factor;
    changeCapacity();
  }
  void changeCapacity()
  {
    Type *newArray = new Type[_capacity];
    for (size_type i = 0; i < _size; i++)
    {
      newArray[i] = _array[i];
    }

    delete[] _array;
    _array = newArray;
  }
  void moveElementsRight(size_type from, int jump = 1)
  {
    if (_size == 0)
      return;
      
    assert(from >= 0);

    size_type to = _size - 1;
    while (true)
    {
      _array[to + jump] = _array[to];
      if (to <= from)
        return;
      --to;
    }
  }

  void moveElementsLeft(size_type from, int jump = 1)
  {
    if (_size == 0)
      return;

    assert(from - jump >= 0);
    for (size_type i = from; i <= _size - 1; ++i)
      _array[i - jump] = _array[i];
    
  }
};

template <typename Type>
class Vector<Type>::ConstIterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  using pointer = typename Vector::const_pointer;
  using reference = typename Vector::const_reference;

  explicit ConstIterator() : _elem(nullptr) {}
  explicit ConstIterator(const_pointer elem, size_type pos, const Vector<Type> *v) : _elem(elem), _position(pos), vec(v) {}
  ConstIterator(const ConstIterator &other) : _elem(other._elem), _position(other._position), vec(other.vec) {}

  reference operator*() const
  {
    if (_elem == nullptr)
      throw std::out_of_range("Dereferencing uninitialized iterator");
    if (_position >= vec->getSize())
      throw std::out_of_range("Dereferencing end iterator");
    
    return *_elem;   
  }

  ConstIterator &operator++()
  {
    if (_position + 1 > vec->getSize())
      throw std::out_of_range("Incrementing end iterator");

    ++_position;  
    ++_elem;
    return *this;
  }

  ConstIterator operator++(int)
  {

    if (_position + 1 > vec->getSize())
      throw std::out_of_range("Incrementign end iterator");

    auto temp = *this;

    ++_position;
    ++_elem;
    return temp;
  }

  ConstIterator &operator--()
  {
    if (_position == 0)
      throw std::out_of_range("Decrementing begin iterator");

    --_elem;
    --_position;
    return *this;
  }

  ConstIterator operator--(int)
  {
    if (_position == 0)
      throw std::out_of_range("Decrementing begin iterator");

    auto temp = *this;

    --_position;
    --_elem;
    return temp;
  }

  ConstIterator operator+(difference_type d) const
  {
    if (_position + d > vec->getSize())
      throw std::out_of_range("Adding to iterator passed the end");

    return ConstIterator(_elem + d, _position + d, vec);
  }

  ConstIterator operator-(difference_type d) const
  {
    if (_position - d < 0)
      throw std::out_of_range("Substracting iterator pass zero");

    return ConstIterator(_elem - d, _position - d, vec);
  }

  bool operator==(const ConstIterator &other) const
  {
    return _elem == other._elem;
  }

  bool operator!=(const ConstIterator &other) const
  {
    return !(*this == other);
  }

protected:
  size_type _position;
  const_pointer _elem;
  const Vector<Type> *vec;
};

template <typename Type>
class Vector<Type>::Iterator : public Vector<Type>::ConstIterator
{
public:
  using pointer = typename Vector::pointer;
  using reference = typename Vector::reference;
  using size_type = typename Vector::size_type;

  explicit Iterator() : ConstIterator()
  {
  }

  Iterator(pointer elem, size_type pos, Vector<Type> *v) : ConstIterator(elem, pos, v) {}

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
};

} // namespace aisdi

#endif // AISDI_LINEAR_VECTOR_H
