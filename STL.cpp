#include <iostream>
#include <vector>
#include <limits.h>
#include <type_traits>
#include <algorithm>
using namespace std;

// allocator
namespace dyj01{
    template<class T>
    inline T* _allocate(ptrdiff_t size, T* t){
        set_new_handler(0);
        // set_new_handler函数的作用是设置new_p指向的函数为new操作或new[]操作失败时调用的处理函数。

        T* temp = (T*)(::operator new((size_t)(size * sizeof(T))));
        if (temp == nullptr)
        {
            cerr << "out of memory" << endl;
            exit(0);
        }
        return temp;        
    }

    template<class T>
    inline void _deallocate(T* buffer){
        ::operator delete(buffer);
    }

    template<class T1, class T2>
    inline void _construct(T1* p, const T2& value){
        new(p) T1(value);
        // placement new，指针p所指向的内存空间创建一个T1类型的对象，但是对象的内容是从T2类型的对象转换过来的
    }

    template<class T>
    inline void _destroy(T* ptr){
        ptr->~T();
    }

    template<class ForwardIterator, class T>
    inline void _destroy(ForwardIterator first, ForwardIterator last, T*){
        //using trivial_destructor = typename type_trait<T>::has_trivial_destructor;

    }

    template<class ForwardIterator>
    inline void _destroy_aux(ForwardIterator first, ForwardIterator last, __false_type){
        for (; first < last; first++){
            
        }
        
    }

    template <class T>
    class allocator{
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        template<class U>
        struct rebind{
            using other = allocator<U>;
        };

        pointer allocate(size_type n, const void* hint = 0){
            cout << "allocator" << endl;
            return _allocate(n, (pointer)hint);
            //无法从void*到pointer的隱式转换
        }

        void deallocate(pointer p, size_type n) { _deallocate(p); }

        void construct(pointer p, const T& value) { _construct(p, value); }

        //第一版本
        void destroy(pointer p) { _destroy(p); }

        pointer address(reference x) { return (pointer)&x; }

        const_pointer cosnt_address(const_reference x) { return (const_pointer)&x; }

        size_type max_size() const { return size_type(UINT_MAX / sizeof(T)); }
        // UINT_MAX包含于limits.h头文件中
    };
}

// alloc
namespace dyj02{
    template <int inst>
    class __malloc_alloc_template {
    private:
        //处理异常情况
        static void *oom_malloc(size_t);
        static void *oom_realloc(void *, size_t);
        static void (* __malloc_alloc_oom_handler)();

    public:
        static void *allocate(size_t n){
            void *result = malloc(n);
            if (nullptr == result) result = oom_malloc(n);
            return result;
        }

        static void deallocate(void *p,size_t){ free(p); }

        static void* reallocate(void *p, size_t, size_t new_sz){
            void *result = realloc(p, new_sz);
            if (nullptr == result) result = oom_realloc(p, new_sz);
            return result;
        }

        //设置自己的out-of-memory handler
        static void (* set_malloc_handler(void (*f)()))(){
            void (* old)() = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = f;
            return old;
        }
    };
    
    template<int inst>
    void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

    template<int inst>
    void * __malloc_alloc_template<inst>::oom_malloc(size_t n){
        void (* my_malloc_handler)();
        void *result;

        while (1) {
            my_malloc_handler = __malloc_alloc_oom_handler;
            if (nullptr == my_malloc_handler) { __throw_bad_alloc; }
            (*my_malloc_handler)();
            result = malloc(n);
            if (result) return(result);
        }
    }

    template<int inst>
    void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n){
        void (* my_malloc_handler)();
        void *result;

        while (1) {
            my_malloc_handler = __malloc_alloc_oom_handler;
            if (nullptr == my_malloc_handler) { __throw_bad_alloc; }
            (*my_malloc_handler)();
            result = realloc(p, n);
            if (result) return(result);
        }
    }
    
}

/*
namespace dyj03{
    template<class ForwardIterator, class Size, class T, bool condition>
    inline ForwardIterator __my_uninitialized_fill_n_aux
        (ForwardIterator first, Size n, const T& x){
        return fill_n(first, n, x);
    }

    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator __my_uninitialized_fill_n_aux<ForwardIterator, Size, T, false>
        (ForwardIterator first, Size n, const T& x){
        ForwardIterator cur = first;
        for (; n > 0; cur++)
        {
            _Construct(&*cur, x);
        }
        return cur;
    }

    template<class ForwardIterator, class Size, class T, class T1>
    inline ForwardIterator __my_uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*){
        // typedef typename type_traits<T1>::is_POD_type is_POD;
        return __my_uninitialized_fill_n_aux(first, n, x, is_pod<T1>::value);
    }

    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator my_uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
        return __my_uninitialized_fill_n(first, n, x, first);
    }
}
*/

namespace dyj04 // 手工實現List容器
{
    template <typename T>
    class List
    {
    public:

        List() : _end(nullptr), _front(nullptr), _size(0) {}
        void insert_front(T value);
        void insert_back(T value);
        void display(std::ostream& os = std::cout);
        size_t size() const { return _size; }
    private:
        struct ListItem
        {
            ListItem(T value) : _value(value), _next(nullptr) {}
            T value() const { return _value; }
            ListItem* next() const { return _next; }

            T _value;
            ListItem* _next;
            template <typename U>
            bool operator!=(U n) { return _value != n; }
        };

        ListItem* _end;
        ListItem* _front;
        int _size;
        
    public:
        struct ListIter
        {
            ListItem* ptr;
            ListIter(ListItem* p = 0) : ptr(p) { }
            ListItem operator*() const { return *ptr; }
            ListItem* operator->() const { return ptr; }
            ListIter& operator++() { ptr = ptr->next(); return ptr; }
            ListIter operator++(int) { ListIter temp = *this; ++*this; return temp; }
            bool operator==(const ListIter& i) const { return ptr == i.ptr; }
            bool operator!=(const ListIter& i) const { return ptr != i.ptr; }
        };

        ListItem* begin() { return _front; }
        ListItem* end() { return _end;}
    };

    template<typename T>
    void List<T>::insert_back(T value)
    {
        struct ListItem* temp = new ListItem(value);        
        if (_end == nullptr && _front == nullptr)
        {
            _end = temp;
            _front = temp;
            ++_size;
        }
        else
        {
            _end->_next = temp;
            _end = temp;
            ++_size;
        }  
    }

    template<typename T>
    void List<T>::insert_front(T value)
    {
        struct ListItem* temp = new ListItem(value);
        if (_end == nullptr && _front == nullptr)
        {
            _end = temp;
            _front = temp;
            ++_size;
        }
        else
        {
            temp->_next = _front;
            _front = temp;
            ++_size;
        }
    }

    template<typename T>
    void List<T>::display(std::ostream& os) // 模版默认参数必须在第一次声明的时候指出，后序无需再出现
    {
        struct ListItem* temp = _front;
        for (int i = 1; i <= size(); i++)
        {
            if (i == size())
            {
                os << temp->value() << "]" << endl;
            }
            else
            {
                if (i == 1)
                {
                    os << '[' << temp->value() << ',';
                }
                else
                {
                    os << temp->value() << ",";
                }
                temp = temp->_next;
            }
        }

        // os << temp->_next << endl;
        // os << temp->_next->_next << endl;
        // os << temp->_next->_next->_next << endl;
        // os << temp->_value << endl;
    }
} // namespace dyj04

int main(){
    // int ia[5] = {0,1,2,3,4};
    // unsigned int i;
    // vector<int, dyj01::allocator<int> >iv(ia, ia + 5);
    // for (auto temp : iv){
    //     cout << temp << " ";
    // }
    // vector<int> vec_int(5);
    // dyj03::my_uninitialized_fill_n(vec_int.begin(), 5, 20);
    dyj04::List<int> list;
    list.insert_back(123);
    list.insert_back(456);
    list.insert_back(789);
    list.insert_back(999);
    // cout << list.size() << endl;
    list.display();
    dyj04::List<int>::ListIter begin(list.begin());
    dyj04::List<int>::ListIter end(list.end());
    dyj04::List<int>::ListIter iter;
    iter = std::find(begin, end, 123);
    if (iter == end)
    {
        cout << "not found" << endl;
    }
    else
    {
        cout << "found:" << iter->value() << endl;
    }
}