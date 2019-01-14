#include <iostream>

using namespace std;

template <typename T>
class dynamicarray{
    private:
        T** array;        
        unsigned int maxsize, size, highindex;
        void constructor(unsigned int usermax, unsigned int usersize);
        void setsize(unsigned int newsize);
    protected:
        unsigned int gethighindex() {return highindex;}
    public:
        dynamicarray(unsigned int usermax) {constructor(usermax, 1);}
        dynamicarray(unsigned int usermax, unsigned int usersize) {constructor(usermax, usersize);}
        ~dynamicarray();
        unsigned int increase();
        unsigned int decrease();
        unsigned int increase(unsigned int userincrease);
        unsigned int decrease(unsigned int userdecrease);
        unsigned int changesize(unsigned int usersize);
        unsigned int getsize() {return size;}
        unsigned int getmaxsize() {return maxsize;} 
        T& operator [] (unsigned int index);
};

template <typename T>
void dynamicarray<T>::constructor(unsigned int usermax, unsigned int usersize){
    maxsize = usermax;
    setsize(0);
    array = new T*[maxsize];
    increase(usersize);
}

template <typename T>
dynamicarray<T>::~dynamicarray(){
    changesize(1);
    delete array[highindex];
    setsize(0);
    delete array;
}

template <typename T>
void dynamicarray<T>::setsize(unsigned int newsize){
    size = newsize;
    highindex = size - 1;
}

template <typename T>
unsigned int dynamicarray<T>::increase(){
    if (size < maxsize) {
        setsize(size + 1);
        array[highindex] = new T;
    }
    return size;
}

template <typename T>
unsigned int dynamicarray<T>::decrease(){
    if (size > 1) {
        delete array[highindex];
        setsize(size - 1);
    }
    return size;
}

template <typename T>
unsigned int dynamicarray<T>::increase(unsigned int userincrease){
    unsigned int newsize = size + userincrease;
    if (newsize > maxsize) {newsize = maxsize;}
    while (size < newsize) {increase();}
    return size;
}

template <typename T>
unsigned int dynamicarray<T>::decrease(unsigned int userdecrease){
    unsigned int newsize = size - userdecrease;
    if (newsize < 1) {newsize = 1;}
    while (size > newsize) {decrease();}
    return size;
}

template <typename T>
unsigned int dynamicarray<T>::changesize(unsigned int usersize){
    if (usersize > size) {increase(usersize - size);}
    else if (usersize < size) {decrease(size - usersize);}
    return size;
}

template <typename T>
T& dynamicarray<T>::operator [](unsigned int index){
     if (index < size) {return *array[index];}
     else {cout << "dynamicarray operator [] invalid index" << endl; system("PAUSE");}
}
