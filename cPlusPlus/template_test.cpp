#include <atomic>
#include <stdio.h>
#include <thread>
#include <list>
#include <iostream>
class PI {
public:
    double operator *(float v) {
        return (double)val*v;
    }
    const float val = 3.1415927f;
};
template <typename T>
struct PI_T
{
    PI_T(T i_val): val(i_val){}
    public:
        T operator *(T v) {
        return (T)val*v;
    }
    const T val;
};

class Time
{
    public:
        int hour;
        int min;
        int sec;
};

class Box {
    public : 
        int volume( ); 
        static int height; //把height定义为静态的数据成员
    private : 
        int width; 
        int length;
};

int Box::height = 10;

template <typename T, typename T1>
class TEST
{
    public:
      void setData(T v);
      T getData();
      
      T1 data1;
    private:
      T data;
};

template <typename T, typename T1>
void TEST<T,T1>::setData(T v)
{
    data = v;
}

template <typename T, typename T1>
T TEST<T,T1>::getData()
{
    return data;
}

int main(void) {
    float radius = 5.23f;
    PI pi;
    auto circumference = 2*( pi*radius);
    std::cout << circumference << std::endl;
    PI_T <int> a(3);
    PI_T <int> d(2);
    int b = 5;
    auto c = 2*(a*b*d.val);
    std::cout << c << std::endl;
    Time time{1,2,3};
    std::cout << time.hour << time.min << time.sec << std::endl;
    std::cout << "Box->height " << Box::height << std::endl;
    TEST <int, float> template_test;
    template_test.setData(5);
    template_test.data1 = 3.1;
    std::cout << template_test.getData() << " " << template_test.data1 << std::endl;
    return 0;
}
