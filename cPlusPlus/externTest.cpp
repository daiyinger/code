#include <iostream>

using namespace std;

class Medium
{
private:
    int data;
public:
    int datas;
    Medium(void)
    {

    }
    Medium(int par)
    {
	data = par;
	datas = par;
    }
    void setData(int par)
    {
	data = par;
	datas = par;
    }
    void display(void)
    {
	cout << "data " << data << endl;
    }
};
class MyMedium:public Medium
{
public:
    MyMedium(int para);
    //{
	//data = para;	
    //}
    void display(void)
    {
	cout << "data " << data << " num " << num << endl;
    }
private:
    int data;
    int num;
};

MyMedium::MyMedium(int para)
{
    num = para;
    data = para;
}
int main()
{
    Medium med(5);
    med.display();
    MyMedium myMed(6);
    myMed.display();
    myMed.setData(55);
    myMed.Medium::display();
    return 0;
}
