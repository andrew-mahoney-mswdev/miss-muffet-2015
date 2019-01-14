#include <iostream>
#include <sstream>
#include <string>
#include "byte.h"
#include "dynamicarray.h"
#include "filemngr.h"

using namespace std;

enum bxcrumb {bx00, bx01, bx10, bx11}; //Allows the programmer to represent the various states of 2 bits by proceeding 2 binary digits with the characters bx.

bxcrumb bx(unsigned char crumbvalue){ //Allows conversion from an int to a bxcrumb.
    switch(crumbvalue){
        case 0: return bx00;
        case 1: return bx01;
        case 2: return bx10;
        case 3: return bx11;
        default: cout << "bx invalid crumbvalue" << endl; system("PAUSE");
    }
}

string hex(byte byteobj){ //Converts a byte's value to a hexadecimal string. Used by dynamicarraybytefilemngr to output bytes to a file.
    short int bytevalue;
    ostringstream hexadecimal;
    string hexstring;
    
    bytevalue = (short int)byteobj;
    hexadecimal << hex << bytevalue;
    if (bytevalue > 15) {hexstring = hexadecimal.str();}
    else {hexstring = "0" + hexadecimal.str();}

    return hexstring;
}

unsigned char hexbyte(string hexstring){ //Converts a 2 character hex string into a one byte integer value.
    unsigned char value;
    switch (hexstring[0]){
        case '0': value = 0; break;
        case '1': value = 16; break;
        case '2': value = 32; break;
        case '3': value = 48; break;
        case '4': value = 64; break;
        case '5': value = 80; break;
        case '6': value = 96; break;
        case '7': value = 112; break;
        case '8': value = 128; break;
        case '9': value = 144; break;
        case 'a': value = 160; break;
        case 'b': value = 176; break;
        case 'c': value = 192; break;
        case 'd': value = 208; break;
        case 'e': value = 224; break;
        case 'f': value = 240; break;
    }
    switch (hexstring[1]){
        case '0': value += 0; break;
        case '1': value += 1; break;
        case '2': value += 2; break;
        case '3': value += 3; break;
        case '4': value += 4; break;
        case '5': value += 5; break;
        case '6': value += 6; break;
        case '7': value += 7; break;
        case '8': value += 8; break;
        case '9': value += 9; break;
        case 'a': value += 10; break;
        case 'b': value += 11; break;
        case 'c': value += 12; break;
        case 'd': value += 13; break;
        case 'e': value += 14; break;
        case 'f': value += 15; break;
    }
    return value;
}

class dynamicarraybytefilemngr : public dynamicarray<byte>{ //Allows file management of dynamicarray<byte>.
    public:
        dynamicarraybytefilemngr(unsigned int usermax) : dynamicarray<byte>(usermax) {};
        dynamicarraybytefilemngr(unsigned int usermax, unsigned int usersize) : dynamicarray<byte>(usermax, usersize) {};
        dynamicarraybytefilemngr(unsigned int usermax, string filename) : dynamicarray<byte>(usermax) {load(filename);};        
        void save(filemngr*); //Saves the byte array.
        void save(string); //Creates a filemngr object for the save function above.
        unsigned int load(filemngr*); //Loads the byte array.
        unsigned int load(string); //Creates a filemngr object for the load function above.         
};

void dynamicarraybytefilemngr::save(string filename){
    filemngr* bytefile = new filemngr(filename, out);
    save(bytefile);
    delete bytefile;
}

void dynamicarraybytefilemngr::save(filemngr* bytefile){
    unsigned int index, size;

    if ((*bytefile).getstate() != out) {(*bytefile).changestate(out);}
    size = getsize();

    for (index = 0; index < size; index++){
        if ((index+1) % 16 != 0) {(*bytefile).writeword(hex((*this)[index]));}
        else {(*bytefile).writeline(hex((*this)[index]));} //16 bytes are written per line.
    }
}

unsigned int dynamicarraybytefilemngr::load(string filename){
    unsigned int index;
    filemngr* bytefile = new filemngr(filename, in);
    index = load(bytefile);
    delete bytefile;
    return index;
}

unsigned int dynamicarraybytefilemngr::load(filemngr* bytefile){
    unsigned int index, highindex, maxsize;
    string hexstring;
    
    if ((*bytefile).getstate() != in) {(*bytefile).changestate(in);}
    highindex = gethighindex();
    maxsize = getmaxsize();

    hexstring = (*bytefile).readstring(); //The first byte is read.
    for (index = 0; (*bytefile).geteof() == false && index < maxsize; index++){ //Reading stops if end of file or maxsize is reached.
        if (index > highindex) {increase();} //During the loop, index = highindex. Array size is increased accordingly (if it was smaller than required).
        (*this)[index] = hexbyte(hexstring);
        hexstring = (*bytefile).readstring(); //If end of file is reached, bytefile will detect it at this point.
    } //If end of file is reached, index is incremented once more (see start of loop).
    changesize(index); //Index now equals size. Array size is changed accordingly (if it was larger than required).

    return index;
}

class dynamicarraycrumb;
class crumb; //Classes are declared in advance because they reference each other.

class dynamicarraycrumb
{//This class is a polymorph of the dynamicarray<T> class, allowing the programmer to manipulate a dynamic array of crumbs (2 bit units).
//This class does not really create a dynamic array of crumbs. Rather, it manages a dynamic array of bytes, giving the impression of a dynamic array of crumbs.
private:
    dynamicarraybytefilemngr* parentarray; //The dynamic array of bytes.
    unsigned int maxsize, size; //The maximum number of crumbs, and current number of crumbs, represented by the array.
    void constructor(unsigned int usermax, unsigned int usersize); //The initialisation function, called by the constructor functions.
    void adjustsize(unsigned int newsize); //Sets the value of size and the size of the parent array.
    unsigned int bytesfor(unsigned int numcrumbs); //Calculates the number of bytes required to represent a given number of crumbs. Used by the previous two functions.
public: //Most of the public functions are direct polymorphs of functions in dynamicarray<T>.
    dynamicarraycrumb(unsigned int usermax) {constructor(usermax, 1);}
    dynamicarraycrumb(unsigned int usermax, unsigned int usersize) {constructor(usermax, usersize);}
    dynamicarraycrumb(unsigned int usermax, string filename); //Constructs the object and loads the byte values from a file.
    ~dynamicarraycrumb() {delete parentarray;}
    void save(string);
    unsigned int load(string);
    unsigned int increase();
    unsigned int decrease();
    unsigned int increase(unsigned int userincrease);
    unsigned int decrease(unsigned int userdecrease);
    unsigned int changesize(unsigned int usersize);
    unsigned int getsize() {return size;}
    unsigned int getmaxsize() {return maxsize;}
    crumb& operator [] (unsigned int index); //Allows the programmer to access a 2-bit data unit within the simulated array.
};

dynamicarraycrumb::dynamicarraycrumb(unsigned int usermax, string filename){
    constructor(usermax, 1);
    load(filename);
}

void dynamicarraycrumb::constructor(unsigned int usermax, unsigned int usersize){
    maxsize = usermax;
    parentarray = new dynamicarraybytefilemngr(bytesfor(maxsize)); //Ensures that the maximum size of the parent byte array will be able to represent the required maximum number of crumbs.
    adjustsize(usersize);
}

class crumb{
//This class does not track a value, rather it allows access to the value of a crumb within a byte object.
//The programmer is blind to the crumb class, it is created exclusively by the dynamicarraycrumb class to manage individual crumbs.
private:
    dynamicarraycrumb* parentobject; //The address of the dynamicarracycrumb object that created the crumb object. This is required so that the = operator can return it.
    byte* parentbyte; //The address of the byte in which the crumb exists.
    unsigned char index; //The index of the crumb within that byte. This value is intended to be 0, 2, 4, or 6.
    crumb (dynamicarraycrumb* parentobject, byte* parentbyte, unsigned char index); //Passing a 0 or 1 to this function will return the first of second bit of the crumb, respectively.
    bool crumb::getbit(bool position) {return (*parentbyte)[index + position] == 1;} //Gets one of the crumb's bits from the byte.
    //This code is a little mickey mouse.  This call to the == operand of byte will return a 1 if the value of the bit in question is 1, and a 0 if the value is 0.
    //So even though it doesn't return the value of the bit itself, the returned value matches the bit's value.
    //Defining the function this way was necessary to avoid making the crumb class a friend of the byte class, so that it could have direct access to byte::getbit().
    void crumb::putbit(bool position, bool newstate) {(*parentbyte)[index + position] = newstate;} //Allows a binary value to be inserted into either the first of second bit of the crumb.
public:
    void flip(bool); //Switches the value of one of the crumb's bits.
    bxcrumb bx(); //Returns a bxcrumb type to reflect the crumb's value.
    dynamicarraycrumb& operator = (bxcrumb); //Sets the value of the crumb to that reflected by a bxcrumb type.
    bool operator == (bxcrumb); //Compares the crumb's value to a bxcrumb type.
    
friend class dynamicarraycrumb; //Dynamicarraycrumb requires access to the crumb class's constructor.
friend ostream& operator << (ostream&, crumb&); //Outputs the textual binary representation of the crumb.
//IMPORTANT NOTE: The crumb class is created by the dynamicarraycrumb class when the [] operand of the dynamicarraycrumb class is called.
//IMPORTANT NOTE: It is assumed that this operand will only be called where a function or operand of the crumb class is also being used.
//IMPORTANT NOTE: Any operand of the crumb class MUST delete the crumb object after use, so that it doesn't remain in memory.
};

crumb::crumb(dynamicarraycrumb* myparentobject, byte* myparentbyte, unsigned char myindex){
    parentobject = myparentobject;
    parentbyte = myparentbyte;
    index = myindex;
}

void dynamicarraycrumb::adjustsize(unsigned int newsize){
    if (newsize > maxsize) {newsize = maxsize;}
    else { if (newsize == 0) {newsize = 1;} } //Verifies the value of size.
    size = newsize;
    (*parentarray).changesize(bytesfor(size)); //Changes the size of the parent array to the number of bytes required to represent the current crumb size.
}

unsigned int dynamicarraycrumb::bytesfor(unsigned int numcrumbs){
    unsigned int numbytes = numcrumbs / 4; //The number of bytes is equal to one quarter the number of crumbs.
    if (numcrumbs % 4 > 0) {numbytes++;} //An extra byte is needed to represent any remaining crumbs.
    return numbytes;
}

void dynamicarraycrumb::save(string filename){
    filemngr* bytefile = new filemngr(filename, out);
    
    (*bytefile).writeline(size); //Write size of crumb array.
    (*parentarray).save(bytefile); //Write bytearray to the file.
       
    delete bytefile;
}

unsigned int dynamicarraycrumb::load(string filename){
    unsigned int filesize;
    filemngr* bytefile = new filemngr(filename, in);

    adjustsize((*bytefile).readint()); //Read and adjust size.
    (*parentarray).load(bytefile); //Load bytearray.

    delete bytefile;
    return size;
}

//Note, the following polymorphed functions differ from those in dynamicarray<T>, becuase it is not necessary to individually create and destoy every crumb.
//Instead creation and deletion of the bytes representing the crumbs is automatically mananged by dynamicarray<byte>.
unsigned int dynamicarraycrumb::increase(){
    adjustsize(size + 1);
    return size;
}

unsigned int dynamicarraycrumb::decrease(){
    adjustsize(size - 1);
    return size;
}

unsigned int dynamicarraycrumb::increase(unsigned int userincrease){
    adjustsize(size + userincrease);
    return size;
}

unsigned int dynamicarraycrumb::decrease(unsigned int userdecrease){
    if (userdecrease > size) {userdecrease = size;} //This verifies that the equation below will not result in a negative number, which cannot be represented by an unsigned int.
    adjustsize(size - userdecrease);
    return size;
}

unsigned int dynamicarraycrumb::changesize(unsigned int usersize){
    adjustsize(usersize);
    return size;
}

crumb& dynamicarraycrumb::operator [] (unsigned int index){
    crumb* crumbobj;
    if (index < size) {
        crumbobj = new crumb(this, &(*parentarray)[index / 4], index % 4 * 2); //Creates a crumb object which represents and manages the 2-bit segment within the relevant byte.
        return *crumbobj;
    }
    else {cout << "dynamicarraycrumb operator [] invalid index" << endl; system("PAUSE");}
}

void crumb::flip(bool position){
    if (getbit(position) == 0) {putbit(position, 1);}
    else {putbit(position, 0);}
    delete this;
}

bxcrumb crumb::bx(){
    bxcrumb crumbstate;
    switch (getbit(0)){
        case 0:
            switch (getbit(1)){
                case 0: crumbstate = bx00; break;
                case 1: crumbstate = bx01; break;
            }
            break;
        case 1:
            switch (getbit(1)){
                case 0: crumbstate = bx10; break;
                case 1: crumbstate = bx11; break;
            }
            break;
    }
    delete this;
    return crumbstate;
}

dynamicarraycrumb& crumb::operator = (bxcrumb crumbstate){
    dynamicarraycrumb* myparentobject = parentobject;
    switch (crumbstate) {
        case bx00:
            putbit(0, 0);
            putbit(1, 0);
            break;
        case bx01:
            putbit(0, 0);
            putbit(1, 1);
            break;
        case bx10:
            putbit(0, 1);
            putbit(1, 0);
            break;
        case bx11:
            putbit(0, 1);
            putbit(1, 1);
            break;
    }
    delete this;    
    return *myparentobject;
}

bool crumb::operator == (bxcrumb yourcrumbstate){
    bxcrumb mycrumbstate = bx(); //The crumb object is deleted by this function.
    if (mycrumbstate == yourcrumbstate) {return true;} else {return false;}
}

ostream& operator << (ostream& output, crumb& crumbobj){
    output << crumbobj.getbit(0) << crumbobj.getbit(1);
    delete &crumbobj;
    return output;
}
