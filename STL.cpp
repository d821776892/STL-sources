#include <iostream>
#include <vector>
#include <limits.h>
#include <type_traits>
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
int main(){
    // int ia[5] = {0,1,2,3,4};
    // unsigned int i;
    // vector<int, dyj01::allocator<int> >iv(ia, ia + 5);
    // for (auto temp : iv){
    //     cout << temp << " ";
    // }
    vector<int> vec_int(5);
    dyj03::my_uninitialized_fill_n(vec_int.begin(), 5, 20);
}