#include <iostream>
template<class T>
class UniquePointer
{
    T* m_pointer = nullptr;
public:
    // constructor and destructor
    constexpr explicit UniquePointer(T* pointer = nullptr) noexcept : m_pointer(pointer) {}
    constexpr ~UniquePointer() noexcept { delete m_pointer; }

    //copy constructor and assignment operator
    constexpr UniquePointer(const UniquePointer&) noexcept  = delete;
    constexpr UniquePointer& operator=(const UniquePointer&) noexcept = delete;

    //move constructor and assignment operator
    constexpr UniquePointer(UniquePointer&& other) noexcept : m_pointer(other.m_pointer) { other.m_pointer = nullptr; }
    constexpr UniquePointer& operator=(UniquePointer&& other)
    {
        reset(other.m_pointer);
        other.m_pointer = nullptr;
        return *this;
    }
    constexpr void reset(T* pointer=nullptr) noexcept
    {
        if ( m_pointer != nullptr )
            delete m_pointer ;
        m_pointer = pointer;
    }

    constexpr T* release() noexcept
    {
        T* temp = m_pointer;
        m_pointer = nullptr;
        return temp;
    }

    constexpr void swap(UniquePointer &other) noexcept { std::swap(m_pointer, other.m_pointer); }
    constexpr T* get()        const noexcept { return m_pointer;  }
    constexpr T* operator->() const noexcept { return m_pointer;  }
    constexpr T& operator*()  const noexcept { return *m_pointer; }

    constexpr operator bool() const noexcept { return m_pointer != nullptr; }
};

class Demo
{
    static int count;
    int id;
public:
    Demo()            { ++count; id = count; std::cout<<"Demo "<<id<<"\n";}
    Demo(const Demo&) { ++count; id = count; std::cout<<"Demo& "<<id<<"\n";}
    ~Demo()           { std::cout<<"~Demo "<<id<<"\n";}
    void print()      { std::cout<<"Demo::print "<<id<<"\n"; }
};
int Demo::count =0;

int main()
{
    UniquePointer<Demo> d_ptr1(new Demo());
    d_ptr1->print();
    d_ptr1.reset(new Demo());

    if(d_ptr1)
        std::cout<<"Pointer is not null\n";

    d_ptr1.reset();
    if(!d_ptr1)
        std::cout<<"Pointer is null\n";

    UniquePointer<int> i_ptr1(new int(10));
    std::cout<<"Value stored "<<*i_ptr1<<'\n';
    return 0;
}
