#include <iostream>
#include <memory>
#include <stack>

template<typename T>
class ObjectPool
{
private: 
    using ObjType = std::shared_ptr<T>;
    std::stack<T*> m_pool;
public:

    void release(T* ptr)
    {
        ptr->~T();
        m_pool.push(ptr);
        std::cout<<"  Released memory to pool\n";
    }

    ObjType acquire()
    {
        ObjType obj = nullptr;
        if(m_pool.empty())
        {
            obj = ObjType( new T, [this](T* ptr) { this->release(ptr);});
            std::cout<<"Acquired momory from heap\n";
        }
        else
        {
            auto pool = m_pool.top(); m_pool.pop();
            obj = ObjType( new(pool) T, [this](T* ptr) { this->release(ptr);});
            std::cout<<"Acquired memory from pool\n";
        }
        return obj;
    }

    ~ObjectPool()
    {
        while(!m_pool.empty())
        {
            auto ptr = m_pool.top(); m_pool.pop();
            delete ptr;
        }
        std::cout<<"ObjectPool free up the memory\n";
    }
};

struct Order
{
    static int count;
    int order_id;
    Order(){ ++count;  order_id = count; std::cout<<"order id "<<order_id<<" created\n"; }
    ~Order(){                            std::cout<<"order id "<<order_id<<" removed\n"; }
};
int Order::count = 0;

void Test1ObjPool()
{
    std::cout<<"\nTest1\n";
    ObjectPool<int> obj_pool;
    {
        auto a = obj_pool.acquire();
        { auto b = obj_pool.acquire(); }
        obj_pool.acquire();
    }
}

void Test2ObjPool()
{
    std::cout<<"\nTest2\n";
    ObjectPool<Order> obj_pool;
    {
        auto a = obj_pool.acquire();
        { auto b = obj_pool.acquire(); }
        obj_pool.acquire();
    }
}

int main()
{
    Test1ObjPool();
    Test2ObjPool();
    return 0;
}
