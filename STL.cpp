#include <iostream>
#include <vector>
#include <limits.h>
using namespace std;

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

        void destroy(pointer p) { _destroy(p); }

        pointer address(reference x) { return (pointer)&x; }

        const_pointer cosnt_address(const_reference x) { return (const_pointer)&x; }

        size_type max_size() const { return size_type(UINT_MAX / sizeof(T)); }
        // UINT_MAX包含于limits.h头文件中
    };
}

int main(){
    int ia[5] = {0,1,2,3,4};
    unsigned int i;
    vector<int, dyj01::allocator<int> >iv(ia, ia + 5);
    for (auto temp : iv){
        cout << temp << " ";
    }
}