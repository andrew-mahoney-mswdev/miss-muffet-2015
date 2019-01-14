#include <iostream>

using namespace std;

class byte;
class bit; //Classes are declared in advance because they reference each other.

class byte //Class gives complete control over the binary representation of one byte.
{
private:
    unsigned char value; //The class represents the binary data within this single byte value.
    bool getbit(unsigned char); //Returns the value of a single bit.
    void putbit(unsigned char, bool); //Puts the bool value into a single bit.
public:
    bit& operator [] (unsigned char); //Allows programmer access to a single bit with the [] operand, thereby simulating an array of 8 bits.
    byte& operator = (unsigned char); //Allows programmer to change the numerical value of the byte.
    operator short int() {return value;} //Allows programmer to output the numerical value of the byte as a short int.

friend class bit; //The byte class requires access to the bit class's constructor.
friend ostream& operator << (ostream&, byte&); //Allows programmer to output the binary representation of the byte with the << operand.
};

class bit
{ //This class was created because outputing multiple single bit values within the same ostream using the [] operand of the byte class did not work.
//The bit class does not track a value, rather it enables read and write access to a single bit within the byte class.
//The programmer is blind to the bit class, it is created exclusively by the byte class to manage single bits.
private:
    byte* parent; //The address of the byte object that created the bit object.
    unsigned char index; //The number of the bit (0-7) in the parent byte object, which this instance of bit represents.
    bit(byte*, unsigned char); //Constructor function which sets the parent and index variables.
    bool getbit() {parent->getbit(index);} //A polymorph of the same function in the byte class. It accesses the getbit() function of the byte class to return the value of the bit that this instance represents.
    void putbit(bool newstate) {parent->putbit(index, newstate);} //Another polymorph, which access the putbit() function of the parent byte.
public:
    byte& operator = (bool); //Allows the programmer to set the value of the bit. As the programmer cannot create a bit class, they can only manipulate it by using the [] operand of the byte class.
    bool operator == (bool); //Allows the programmer to check the state of a bit, again by using the [] operand of the byte class.

friend class byte; //The bit class requires access to the getbit() and putbit() functions of the byte class.
friend ostream& operator << (ostream&, bit&); //Allows the programmer to output a single bit if they use the [] operand of the byte class.
//IMPORTANT NOTE: The bit class is created by the byte class when the [] operand of the byte class is called.
//IMPORTANT NOTE: It is assumed that this operand will only be called where an operand of the bit class is also being used.
//IMPORTANT NOTE: Any operand of the bit class MUST delete the bit object after use, so that it doesn't remain in memory.
};

bit::bit(byte* myparent, unsigned char myindex) {
    parent = myparent;
    if (myindex < 8) {index = myindex;}
    else {cout << "bit::bit invalid index" << endl; system("PAUSE");}
}

bool byte::getbit(unsigned char index){
    switch (index) {
        case 0: return value / 128; break;
        case 1: return value / 64 % 2; break;
        case 2: return value / 32 % 2; break;
        case 3: return value / 16 % 2; break;
        case 4: return value / 8 % 2; break;
        case 5: return value / 4 % 2; break;
        case 6: return value / 2 % 2; break;
        case 7: return value % 2; break;
    }
}

void byte::putbit(unsigned char index, bool newstate){
    bool currentstate = getbit(index);
    if (currentstate < newstate){
        switch (index) {
            case 0: value += 128; break;
            case 1: value += 64; break;
            case 2: value += 32; break;
            case 3: value += 16; break;
            case 4: value += 8; break;
            case 5: value += 4; break;
            case 6: value += 2; break;
            case 7: value += 1; break;
        }
    } else if (currentstate > newstate){
        switch (index) {
            case 0: value -= 128; break;
            case 1: value -= 64; break;
            case 2: value -= 32; break;
            case 3: value -= 16; break;
            case 4: value -= 8; break;
            case 5: value -= 4; break;
            case 6: value -= 2; break;
            case 7: value -= 1; break;
        }
    }
}

bit& byte::operator [] (unsigned char index){
    bit* bitobj;
    bitobj = new bit(this, index);
    return *bitobj;
}

byte& byte::operator = (unsigned char newvalue){
    value = newvalue;
    return *this;
}

byte& bit::operator = (bool newstate){
    byte* myparent = parent;
    putbit(newstate);
    delete this;
    return *myparent;
}

bool bit::operator == (bool yourstate){
    bool mystate = getbit();
    delete this;
    if (mystate == yourstate) {return true;} else {return false;}
}

ostream& operator << (ostream& output, byte& byteobj){
    int index;
    for (index = 0; index < 8; index++){
        output << byteobj.getbit(index);
    }
    return output;
}

ostream& operator << (ostream& output, bit& bitobj){
    output << bitobj.getbit();
    delete &bitobj;
    return output;
}
