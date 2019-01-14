#include <math.h>
//#include "dynamicarray.h"
#include "crumb.h"

class mmsettings {
    private:
        string mmfolder;
        unsigned int mmnumtargets;
        unsigned int mmmaxtargets;
        unsigned int mmmaxpop;
        unsigned int mmnichepop;
        unsigned int mmmaxsize;
        short int mmread;
        unsigned int mmmutationrate;
        unsigned int mmcycleinterval;
        dynamicarray<unsigned int>* mmtargetarrayptr;
    public:
        mmsettings();
        ~mmsettings();
        void load();
        void levelup() {if (mmnumtargets < mmmaxtargets) {mmnumtargets++;}}
        string folder() {return mmfolder;}
        unsigned int numtargets() {return mmnumtargets;}
        unsigned int maxtargets() {return mmmaxtargets;}
        unsigned int maxpop() {return mmmaxpop;}
        unsigned int nichepop() {return mmnichepop;}
        unsigned int maxsize() {return mmmaxsize;}
        short int read() {return mmread;}
        unsigned int mutationrate() {return mmmutationrate;}
        unsigned int cycleinterval() {return mmcycleinterval;}
        dynamicarray<unsigned int>* targetarray() {if (mmtargetarrayptr != 0) {return mmtargetarrayptr;}}
};

mmsettings::mmsettings(){
    mmfolder = "arrays";
    mmnumtargets = 1;
    mmmaxtargets = 100;
    mmmaxpop = 100500;
    mmnichepop = 250;
    mmmaxsize = 1024;
    mmread = 5;
    mmmutationrate = 7; //Note: at least one mutation will be a flip.
    mmcycleinterval = 2;
    mmtargetarrayptr = 0;
}

mmsettings::~mmsettings(){
    if (mmtargetarrayptr != 0) {delete mmtargetarrayptr;}
}

void mmsettings::load(){
    int index;
    mmtargetarrayptr = new dynamicarray<unsigned int> (101, 101);

    for (index = 1; index <= mmmaxtargets; index++) {
        (*mmtargetarrayptr)[index] = index * index;
    }
}

mmsettings settings;

enum instname {skip, docmd, setnum, setreg,  //This is every possible state that the interpreter can be in.
               settingnum, 
               docmd_jump, docmd_addone, docmd_proc, docmd_endproc, 
               jumping, 
               setreg_global0, setreg_global1, setreg_local0, setreg_local1, 
               newinst};
enum registername {Rglobal0, Rglobal1, Rlocal0, Rlocal1}; //Represents the various registers.
enum argtype {argregister, arginteger}; //The two types of arguments.

class interpreter;
class argument;

class argument
{//This class stores either a registername type or an integer. Arguments are passed to commands by the interpreter.
    private:
        interpreter* myparent; //The interpreter that creates the argument. This is needed so that the argument class can return pointers to the interpreter's registers.
        argtype mytype; //Whether or not the argument is a register or an integer.
        registername myregister; //The register type is stored here.
        unsigned int myinteger; //The integer value is stored here.
    public:
        argument(registername userregister, interpreter* userparent); //Creates an argument with a register value.
        argument(unsigned int userunsignedint, interpreter* userparent); //Creates an argument with an unsigned integer value.
        unsigned int* getptr(); //Returns a pointer to the argument's value.
        argtype getarg(registername* registerptr); //Returns a pointer to the register type, if the argument is a register type.
        argtype getarg(unsigned int* unsignedintptr); //Returns a pointer to the unsigned integer, if the argument is an integer type.
        argtype getarg(registername* registerptr, unsigned int* unsignedintptr); //Returns two pointers, with one representing the argument's value and the other being set to null.
};

class interpreter {
//This class implements a new machine code for the execution of mathematical functions.
//This code can be read from a dynamicarraycrumb and passed to the interpreter.
//This allows mathematical formulae to be stored within a dynamicarraycrumb.
    private:
        unsigned int proc, maxproc; //Proc represents the current procedure. Maxproc represents the maximum number of procedures the interpreter will allow.
        unsigned int global0, global1; //The two global variables, accessible by every procedure.
        dynamicarray<unsigned int>* local0;
        dynamicarray<unsigned int>* local1; //Two variables which are local to every procedure.
        argument* argumentptr[3]; //Three arguments which effect the execution of commands.
        instname instruction; //The current instruction that the interpreter is executing.
        bool getnewinst; //After an instruction has been executed, it is retained in memory for accurate display. This flag indicates whether a new instruction is required.
        //Therefore, getnewinst is required to record whether or not the interpreter is ready to source a new instruction.
        short int read, readcount; //read represents the number of crumbs that will be read when the interpreter is loading a number. readcount represents the number that have been read.
        unsigned int number; //Stores the number that the interpreter is reading.
        dynamicarray<unsigned int>* search; //This array is used to search for numbers if the jump command comparison is true. Using an array allows multiple numbers to be read from overlapping crumbs.
        void dropargumentptrs(); //This moves arguments 1 & 2 to arguments 2 & 3, respectively. Clearing argument 1 for a new argument.
        void newargument(registername userregister); //Creates a new register argument.
        void newargument(unsigned int userunsignedint); //Creates a new integer argument.
    public:
        interpreter(unsigned int userglobal0, unsigned int usermaxproc); //The constructor requires an input value to initialise global0, and the maximum number of procedures that can be called.
        ~interpreter();
        void operator () (crumb&); //If a crumb object is past to the array, this sources the bxcrumb value of the object and calls the main interpreter function below.
        void operator () (bxcrumb code); //This is the main interpreter function. Each bxcrumb value is interpreted by this function through a series of nested switch statements.
        unsigned int getoutput() {return global1;} //The interpreter will return the value of global1 as its output.
        unsigned int getglobal0() {return global0;} //The following functions will return various data about the current state of the interpreter for display purposes.
        unsigned int getglobal1() {return global1;}
        unsigned int getproc() {return proc;}
        unsigned int getlocal0(unsigned int procindex) {return (*local0)[procindex];} //This returns any value in the local0 array based on the index value passed to the function.
        unsigned int getlocal1(unsigned int procindex) {return (*local1)[procindex];} //See above.
        instname getinst() {return instruction;}
        argtype getarg(int argindex, registername* registerptr, unsigned int* unsignedintptr) {return (*argumentptr[argindex]).getarg(registerptr, unsignedintptr);}
        //A polymorph of the getarg() function from the argument class. This returns the argtype and value of any of the three arguments, depending upon the value of argindex.
        unsigned int getarg0value() {return *(*argumentptr[0]).getptr();}
        unsigned int getarg1value() {return *(*argumentptr[1]).getptr();}
        unsigned int getarg2value() {return *(*argumentptr[2]).getptr();} //Returns the value within an argument.
        unsigned int getnumber() {return number;}
        short int getreadcount() {return readcount;}
    friend class argument; //Argument requires access to global0 & global1 variables and local0 & local1 arrays.
};

argument::argument (registername userregister, interpreter* userparent) {
    myparent = userparent;
    mytype = argregister;
    myregister = userregister;
}

argument::argument (unsigned int userunsignedint, interpreter* userparent) {
    myparent = userparent;
    mytype = arginteger;
    myinteger = userunsignedint;
}

unsigned int* argument::getptr() {
    if (mytype == argregister) {
        switch (myregister) { //Where a registername type is stored, we return a pointer to the register.
        case Rglobal0: return &(*myparent).global0; break;
        case Rglobal1: return &(*myparent).global1; break;
        case Rlocal0: return &(*(*myparent).local0)[(*myparent).proc]; break;
        case Rlocal1: return &(*(*myparent).local1)[(*myparent).proc]; break;
        }
    } else {
        return &myinteger;
    }
}

argtype argument::getarg(registername* registerptr) {
    if (mytype == argregister) {*registerptr = myregister;} else {registerptr = 0;}
    return mytype;
}

argtype argument::getarg(unsigned int* unsignedintptr) {
    if (mytype == arginteger) {*unsignedintptr = myinteger;} else {unsignedintptr = 0;}
    return mytype;
}

argtype argument::getarg(registername* registerptr, unsigned int* unsignedintptr) {
    getarg(registerptr);
    getarg(unsignedintptr);
    return mytype;
}

interpreter::interpreter(unsigned int input, unsigned int usermaxproc) {
    maxproc = usermaxproc-1; //Since the first procedure is numbered 0, maxproc must be set to 1 less than the total maximum number of procedures.

    global0 = input; //User input starts in global0.
    global1 = 0;
    local0 = new dynamicarray<unsigned int>(usermaxproc);
    local1 = new dynamicarray<unsigned int>(usermaxproc);
    proc = 0; //The first procedure is numbered 0.
    (*local0)[proc] = 0;
    (*local1)[proc] = 0; //All other registers begin at 0.

    argumentptr[0] = new argument(0, this);
    argumentptr[1] = new argument(0, this);
    argumentptr[2] = new argument(0, this); //All arguments start at the integer 0.
    instruction = newinst; //A new instruction is ready to be sourced by the interpreter.

    read = settings.read(); //Read is loaded from the user's current settings.
    readcount = 0; //Interpretation starts without a read attempt being in progress.

    search = new dynamicarray<unsigned int>(16, read); //We set the maximum size to 16 as this is the pracital limit of the value of read.
}

interpreter::~interpreter() { //Deleting all objects created in the above constructor code.
    delete argumentptr[2];
    delete argumentptr[1];
    delete argumentptr[0];
    delete search;
    delete local1;
    delete local0;
}

void interpreter::dropargumentptrs(){
    delete argumentptr[2];
    argumentptr[2] = argumentptr[1];
    argumentptr[1] = argumentptr[0];
}

void interpreter::newargument(registername userregister) {
    dropargumentptrs();
    argumentptr[0] = new argument(userregister, this);
}

void interpreter::newargument(unsigned int userunsignedint) {
    dropargumentptrs();
    argumentptr[0] = new argument(userunsignedint, this);
}

void interpreter::operator () (crumb& crumbobj) {return operator () (crumbobj.bx());} //Sources the bx value of the crumb and passes it to the main interpreter function below.
void interpreter::operator () (bxcrumb code) {
    short int count; //Used in both docmd_jump cases.
    
    if (getnewinst == true) {instruction = newinst; getnewinst = false;} //Test for whether to source a new instruction.
    switch (instruction) { //The previous instruction that was executed determines the manner in which the current bxcrumb is interpreted.
    case docmd_jump: //This case occurs where a jump condition was found to be true.
        instruction = jumping; //This flag indicates that jumping is in progress so that it can continue until finished.
    case jumping: //Here we search for a number that matches the value in argument 0.
        //This code works like the setnum code below. However, we use an array to track multiple values simultaneously, so that crumbs are not read in blocks.
        if (readcount > 0) {readcount--;} //Note, searching continues after readcount is zero until the number is found.
        for (count = 1; count < read; count++) {(*search)[count-1] = (*search)[count];} //Drops every array value.
        (*search)[read-1] = 0; //Set the top value to zero.
        for (count = 0; count < read; count++) { //Adds the crumb to every number being counted and assigns it a value based upon its position in the sequence. (e.g. bx11 can be 12 or 3)
            switch (code) {
            case bx01: (*search)[count] += (unsigned int)pow(4, count); break;
            case bx10: (*search)[count] += 2 * (unsigned int)pow(4, count); break;
            case bx11: (*search)[count] += 3 * (unsigned int)pow(4, count); break;
            }
        }
        if (readcount == 0 && (*search)[0] == *(*argumentptr[0]).getptr()) {getnewinst = true;} //The number in search[0] is complete and ready for comparison.
        //We only compare when readcount is 0, as this means the appropriate number of crumbs has been read into search[0].
        break;
    case docmd: //bx01
    //This occurs where the interpreter is executing a command.
        switch (code) {
        case bx00:
            instruction = docmd_jump; //Here, we test to see if the interpreter will jump to another part of the crumb array.
            if (*(*argumentptr[1]).getptr() == *(*argumentptr[2]).getptr()) { //If the value to these arguments match, the interpreter will jump.
                readcount = read; //We read a number of crumbs based upon the value of read.
                for (count = 0; count < read; count++) {(*search)[count] = 0;} //Search is initialised to 0.
            } else {getnewinst = true;} //If the arguments don't match, a new instruction is sourced.
        break;
        case bx01:
            instruction = docmd_addone; //Here, we add 1 to an argument.
            (*(*argumentptr[0]).getptr())++; //Add 1 to the value in the first argument.
        break;
        case bx10:
            instruction = docmd_proc; //Here, we call a new procedure.
            if (proc < maxproc) { //A new procedure is not called if the maximum number of procedures has been reached.
                proc++; //Increment the procedure number.
                (*local0).increase();
                (*local1).increase(); //Increase the local arrays by one.
                (*local0)[proc] = 0;
                (*local1)[proc] = 0; //Local variables are initialised to 0.
            }
        break;
        case bx11:
            instruction = docmd_endproc; //Here, we end a procedure.
            if (proc > 0) { //Proc cannot drop below 0.
                (*local0).decrease();
                (*local1).decrease(); //We decrease the size of these arrays.
                proc--; //The value of proc is decreased.
            }
        break;
        }
    break;
    case setnum: //bx10
        instruction = settingnum;
    case settingnum: //This occurs where the interpreter is reading an integer for a new argument.
        readcount--; //Readcount records how many more crumbs are to be read. We decrement readcount here so that the appropriate power is applied below.
        switch (code) { //Adds the appropriate value to number.
        case bx01: number += (unsigned int)pow(4, readcount); break;
        case bx10: number += 2 * (unsigned int)pow(4, readcount); break;
        case bx11: number += 3 * (unsigned int)pow(4, readcount); break;
        }
        if (readcount == 0) { //Number reading is finished when readcount is 0. We set the number and prepare to source a new instruction.
            newargument(number);
            getnewinst = true;
        }
    break;
    case setreg: //bx11
    //This occurs where the interpreter is sourcing a register for a new argument.
        switch (code) {//This switch statement sets the argument to a register.
        case bx00:
            instruction = setreg_global0;
            newargument(Rglobal0);
        break;
        case bx01:
            instruction = setreg_global1;
            newargument(Rglobal1);
        break;
        case bx10:
            instruction = setreg_local0;
            newargument(Rlocal0);
        break;
        case bx11:
            instruction = setreg_local1;
            newargument(Rlocal1);
        break;
        }
    break;
    default: //This occurs where the interpreter is sourcing a new instruction.
    switch (code) {
        case bx00: //Here, we source a new instruction.
            instruction = skip;
            getnewinst = true;
            break;
        break;
        case bx01: //Here, we are about to execute a command.
            instruction = docmd;
        break;
        case bx10: //Here, we prepare to read a number into a new argument.
            instruction = setnum;
            readcount = read; //We read a number of crumbs equal to the value of read.
            number = 0; //Number is set to 0 to prepare for adding values denoted by the crumbs.
        break;
        case bx11: //Here, we are about to set a new reigster.
            instruction = setreg;
        break;
        }
    break;
    }
}
