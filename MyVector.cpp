/*
    Here is my own implementation for std::vector class.
    Some differences listed below.
    
    Vector Class Overview and Features:

    - This Vector class provides dynamic array functionality similar to std::vector.
    - It manages its own dynamic memory using a raw pointer (_head), size (_size), and capacity (_capacity).
    - Memory allocation and reallocation happen automatically as the capacity changes.

    How It Works:
    - Elements can be dynamically added and removed using functions like push_back, pop_back, and resize.
    - Capacity grows by doubling when more space is needed, and shrinks when the vector is mostly empty.
    - Elements are accessed via operator[], with bounds checking that throws a custom ExceptionVector on invalid access.
    - Copy constructor and assignment operator perform deep copies.
    - Supports construction from initializer lists, size with default value, and copying from other Vectors.

    Additional Operators and Functions:
    - Vector + Vector: Concatenates two vectors into a new one.
    - Vector + T: Adds a value T to each element, returning a new Vector.
    - Compound assignment operators (+=) for both Vector and scalar addition.
    - Comparison operators (==, !=, <, <=, >, >=) perform lexicographical comparison.
    - prefixsum: Returns a Vector of prefix sums, with an optional modular version.
    - getsum: Returns the sum of elements in a given range, also with a modular version.
    - insert and insert_k: Insert single or multiple elements at a specified position.
    - begin() and end() return raw pointers, enabling pointer-based iteration.
    - Overloaded stream insertion and extraction operators facilitate easy I/O.

    Differences From std::vector:
    - Does not implement advanced features like move semantics, custom allocators, or full iterator support.
    - Exception handling is done via a simple custom ExceptionVector class.
    - Capacity shrinking is aggressive (halves capacity if vector is less than 25% full).
    - Bounds checking is always enabled on element access (unlike std::vector's unchecked operator[]).
    - Includes custom functions like modular prefix sums which std::vector doesn't provide.
*/

#include <initializer_list>
#include <string>
#include <ostream>
#include <algorithm>

class ExceptionVector{
public:
    ExceptionVector(std::string exception = "") : _text(exception)   {   }
    std::string what(){
        return _text;
    }
private:
    std::string _text;
};

template<typename T>
class Vector{
public:
    Vector() {
        _size = 0;
        _capacity = 0;
        _head = nullptr;
    }

    Vector(const size_t initial_size) {
        if(initial_size < 0){
            throw ExceptionVector("Initial size must be a non-negative integer.");
        }
        if(initial_size == 0){
            _size = 0;
            _capacity = 0;
            _head = nullptr;
            return;
        }
        _size = initial_size;
        _capacity = initial_size;
        _head = new T[_capacity];
    }

    Vector(const size_t initial_size, const T initial_value) {
        if(initial_size < 0){
            throw ExceptionVector("Initial size must be a non-negative integer.");
        }
        if(initial_size == 0){
            _size = 0;
            _capacity = 0;
            _head = nullptr;
            return;
        }
        _size = initial_size;
        _capacity = initial_size;
        _head = new T[_capacity];
        T each(initial_value);
        for(size_t i = 0; i < _size; i++){
            _head[i] = each;
        }
    }

    Vector(const Vector<T>& rhs) {
        if(rhs._size == 0){
            _size = 0;
            _capacity = 0;
            _head = nullptr;
            return;
        }
        _size = rhs._size;
        _capacity = rhs._capacity;
        _head = new T[_capacity];
        for(size_t i = 0; i < _size; i++){
            _head[i] = rhs._head[i];
        }
    }

    Vector(const std::initializer_list<T> il) {
        if(il.size() == 0){
            _size = 0;
            _capacity = 0;
            _head = nullptr;
            return;
        }
        _size = il.size();
        _capacity = _size;
        _head = new T[_size];
        const T* ptr = il.begin();
        for(size_t i = 0; i < _size; i++){
            _head[i] = *ptr;
            ++ptr;
        }
    }

    Vector<T> const operator+(const Vector<T> rhs) const {
        Vector<T> result(*this);
        for(size_t i = 0; i < rhs._size; i++){
            result.push_back(rhs._head[i]);
        }
        return result;
    }

    Vector<T> const operator+(const T rhs) const {
        Vector<T> result;
        for(size_t i = 0; i < _size; i++){
            result.push_back(_head[i] + rhs);
        }
        return result;
    }

    Vector<T>& operator+=(const Vector<T> rhs) {
        for(size_t i = 0; i < rhs._size; i++){
            this->push_back(rhs._head[i]);
        }
        return *this;
    }

    Vector<T>& operator+=(const T rhs) {
        for(size_t i = 0; i < _size; i++){
            _head[i] += rhs;
        }
        return *this;
    }

    Vector<T>& operator=(const Vector<T>& rhs) {
        if(&rhs == this) return *this;
        _size = rhs._size;
        _capacity = rhs._capacity;
        _change_capacity();
        for(size_t i = 0; i < _size; i++){
            _head[i] = rhs._head[i];
        }
        return *this;
    }

    bool operator==(const Vector<T>& rhs) const {
        if(_size != rhs._size) return false;
        for(size_t i = 0; i < _size; i++){
            if(!(_head[i] == rhs._head[i])) return false;
        }
        return true;
    }

    bool operator!=(const Vector<T>& rhs) const {
        if(_size != rhs._size) return true;
        for(size_t i = 0; i < _size; i++){
            if(_head[i] != rhs._head[i]) return true;
        }
        return false;
    }

    bool operator<(const Vector<T>& rhs) const {
        int _len = _size;
        if(_len < rhs._size) _len = rhs._size;
        for(size_t i = 0; i < _len; i++){
            if(_head[i] != rhs._head[i]){
                return _head[i] < rhs._head[i];
            }
        }
        return _size < rhs._size;
    }

    bool operator<=(const Vector<T>& rhs) const {
        int _len = _size;
        if(_len < rhs._size) _len = rhs._size;
        for(size_t i = 0; i < _len; i++){
            if(_head[i] != rhs._head[i]){
                return _head[i] <= rhs._head[i];
            }
        }
        return _size <= rhs._size;
    }

    bool operator>(const Vector<T>& rhs) const {
        int _len = _size;
        if(_len < rhs._size) _len = rhs._size;
        for(size_t i = 0; i < _len; i++){
            if(_head[i] != rhs._head[i]){
                return _head[i] > rhs._head[i];
            }
        }
        return _size > rhs._size;
    }

    bool operator>=(const Vector<T>& rhs) const {
        int _len = _size;
        if(_len < rhs._size) _len = rhs._size;
        for(size_t i = 0; i < _len; i++){
            if(_head[i] != rhs._head[i]){
                return _head[i] >= rhs._head[i];
            }
        }
        return _size >= rhs._size;
    }

    T& operator[](const size_t idx) const {
        if(idx >= _size){
            throw ExceptionVector("Vector index out of range");
        }
        return _head[idx];
    }

    ~Vector() {
        delete [] _head;
    }

    void push_back(const T value) {
        _size++;
        _change_capacity();
        _head[_size-1] = value;
    }

    void pop_back() {
        if(_size == 0){
            throw ExceptionVector("pop_back() function cannot be called for an empty vector");
        }
        _size--;
        _change_capacity();
    }

    size_t const size() const {
        return _size;
    }

    void resize(const size_t new_size) {
        if(new_size < 0){
            throw ExceptionVector("Vector size must be a non-negative integer");
        }
        _size = new_size;
        _change_capacity();
    }

    bool empty() const {
        return (_size == 0);
    }

    void clear() {
        resize(0);
    }

    void sort() {
        std::sort(begin(), end());
    }

    Vector<T> const prefixsum() const {
        if(_size == 0) return Vector<T>();
        Vector<T> result({_head[0]});
        for(size_t i = 1; i < _size; i++){
            result.push_back(result._head[i-1] + _head[i]);
        }
        return result;
    }

    template<typename ModType>
    Vector<T> prefixsum(const ModType& mod) const {
        if(_size == 0) return Vector<T>();
        Vector<T> result({_head[0]});
        for(size_t i = 1; i < _size; i++){
            result.push_back((result._head[i-1] + _head[i]) % mod);
        }
        return result;
    }

    T const getsum(const size_t i, const size_t j) const {
        T result(_head[j]);
        if(i>0) result -= _head[i-1];
        return result;
    }

    template<typename ModType>
    T getsum(const size_t i, const size_t j, const ModType& mod) const {
        T result(_head[j]);
        if(i>0){
            result -= _head[i-1];
            result += mod;
            result %= mod;
        }
        return result;
    }

    T* const begin() const {
        return _head;
    }

    T* const end() const {
        return _head + _size;
    }

    T const back() const {
        return *(end()-1);
    }

    void const insert(const T& x, const size_t index) {
        resize(_size + 1);
        for(size_t i = _size-1; i > index; i--){
            _head[i] = _head[i-1];
        }
        _head[index] = x;
    }

    
    void const insert_k(const T& x, const int index, const int cnt) {
        for(size_t i=0; i < cnt; i++){
            insert(x, index);
        }
    }

private:
    size_t _size;
    size_t _capacity;
    T* _head;

    void _change_capacity() {
        if(_size > _capacity and _capacity > 0){
            _capacity *= 2;
            T* new_head = new T[_capacity];
            for(size_t i = 0; i < _size; i++){
                new_head[i] = _head[i];
            }
            delete [] _head;
            _head = new_head;
        }
        else if(_size > _capacity and _capacity == 0){
            _capacity = 1;
            _head = new T[_capacity];
            _change_capacity();
        }
        else if(_size > 0 and _size*4 <= _capacity){
            T* new_head = new T[_capacity/2];
            for(size_t i = 0; i < _size; i++){
                new_head[i] = _head[i];
            }
            delete [] _head;
            _capacity /= 2;
            _head = new_head;
        }
        else if(_size == 0){
            _capacity = 0;
            delete [] _head;
            _head = nullptr;
        }
        else{
            return;
        }
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& x){
    for(const T& i : x){
        os << i << ' ';
    }
    return os;
}

template<typename T>
std::istream& operator>>(std::istream& is, Vector<T>& v){
    v.resize(v.size());
    for(T& item : v){
        is >> item;
    }
    return is;
}

