#include <cstdlib>
#include <iostream>
#include <string>
//#include "dynamicarray.h"
//#include "filemngr.h"
//#include "crumb.h"
#include "mminterpreter.h"

using namespace std;

void randomise(){ //Initialises the random number generator.
    time_t now;
    time(&now);

    srand(now);
    rand();
}

int rand(int maxnum) {return rand() % maxnum;} //Generates a whole number between 0 and maxnum-1
bxcrumb bxrand() {return bx(rand() % 4);} //Generates a random bxcrumb value

string str(unsigned int number){ //Converts a positive number to a string.
    stringstream ssintstr;
    ssintstr << number;
    return ssintstr.str();
}

unsigned int num(string intstr){ //Converts a string to an integer.
    unsigned int number;
    stringstream ssintstr(intstr);
    ssintstr >> number;
    return number;
}

class mmcounter { //NOT YET DOCUMENTED, AS CLASS IS UNDER DEVELOPMENT.
private:
    unsigned int mmtime, mmnumber;
public:
    mmcounter();
    void settime(unsigned int thetime) {mmtime = thetime;}
    void ticktock();
    string time() {return str(mmtime);}
    string number();
};

mmcounter::mmcounter(){
    mmtime = 0;
    mmnumber = 0;
}

void mmcounter::ticktock(){
    mmtime++;
    mmnumber = 0;
}

string mmcounter::number(){
    string strnum = str(mmnumber);
    mmnumber++;
    return strnum;
}

mmcounter counter;

void bubblesort(dynamicarray<unsigned int>* unsignedintarray){ //REWRITE SO IT USES A REFERENCE?
    bool swap;
    unsigned int unsignedinttemp;
    unsigned int index;
    unsigned int highindex = (*unsignedintarray).getsize() - 1;
    
    do {
        swap = false;
        for (index = 0; index < highindex; index++) {
            if ((*unsignedintarray)[index] > (*unsignedintarray)[index+1]) {
                unsignedinttemp = (*unsignedintarray)[index];
                (*unsignedintarray)[index] = (*unsignedintarray)[index+1];
                (*unsignedintarray)[index+1] = unsignedinttemp;
                swap = true;
            }
        }
    } while (swap == true);
}

class muffetalgorithm {
//Represents a crumb arrayfile and records the accuracy with which that array calculates target values.
private:
    string filename;
    string filepath() {return settings.folder() + "\\" + filename + ".txt";} //Returns the full file name for access to the file.
    unsigned int errors, margin, size;
    int suborder;
    bool phenome;
    void getphenome();
    unsigned int execute(bxcrumb* bxcrumbarray, unsigned int input, unsigned int maxglobal1); //Executes the array, used while calculating the above values.
    void mutate(dynamicarraycrumb&); //Mutates the crumbarray represented by the class, and stores it in the dynamicarraycrumb object.
    unsigned int getgeneration(); //Returns the generation number of the algorithm. (The first generation is 0.)
public:
    muffetalgorithm(); //Creates a random algorithm.
    muffetalgorithm(muffetalgorithm& parentalgorithm); //Creates a new algorithm that is a mutation of crumbarray represented by the passed algorithm object.
    muffetalgorithm(string filename); //Doesn't create a new algorithm, but represents the crumbarray in the passed filename.
    ~muffetalgorithm() {remove(filepath());} //Deleting the class destroys the file that it represents.
    short int eggs; //Records how many times the algorithm will reproduce. Algorithms that fail to reproduce are deleted.
    void calculate(); //The function with which the user directs the class to calculate its accuracy values.
    void recalculate(); //Forces a calculation of fitness values, even if they've been previously calculated.
    unsigned int execute(unsigned int, unsigned int); //A polymorph of the primary execute function that can run independently of getphenome(). Allows the programmer to execute the array directly.
    unsigned int geterrors() {return errors;}
    string getfilename() {return filename;}

friend bool sortbyerrors(muffetalgorithm*, muffetalgorithm*);
friend bool sortbymargin(muffetalgorithm*, muffetalgorithm*);
friend void selecterrorniche(unsigned int, muffetalgorithm*[], unsigned int);
friend void selectmarginniche(unsigned int, muffetalgorithm*[], unsigned int);
friend void selectmarginniche(unsigned int, unsigned int, muffetalgorithm*[], unsigned int);
};

muffetalgorithm::muffetalgorithm(){ //Creates a random algorithm from scratch.
    unsigned int index;
    unsigned int maxsize = settings.maxsize();
    dynamicarraycrumb crumbarray(maxsize, maxsize); //The new crumbarray is created to be of maximum size.
    
    for (index = 0; index < maxsize; index++) {crumbarray[index] = bxrand();} //The value of every crumb is randomised.
    filename = "0" + space + counter.time() + space + counter.number(); //We source the time and number for creating a unique filename.
    crumbarray.save(filepath());

    phenome = false;
    eggs = 0;
}

muffetalgorithm::muffetalgorithm(muffetalgorithm& parent){ //Creates an algorithm that is a mutation of another.
    dynamicarraycrumb crumbarray(settings.maxsize());
    string generation = str(parent.getgeneration() + 1); //We calculate the generation number of the new algorithm.

    parent.mutate(crumbarray); //crumbarray becomes a mutation of the parent algorithm.
    filename = generation + space + counter.time() + space + counter.number();
    crumbarray.save(filepath());
    phenome = false;
    eggs = 0;
}

muffetalgorithm::muffetalgorithm(string userfilename){
    filename = userfilename; //The muffetalgorithm object represents the file passed by the programmer.
    phenome = false;
    eggs = 0;
}

unsigned int muffetalgorithm::getgeneration(){ //Uses a stringstream to get the first "word" of the filename, which is the generation number.
    string genstr;
    stringstream ssfilename(filename);
    ssfilename >> genstr;
    return num(genstr); //Converts the word to an integer.
}

void muffetalgorithm::calculate(){
    suborder = rand(); //This is reset with every calculation, so that tiebreaker algorithms will always sort differently.
    if (phenome == false) { //To avoid repeat processing, we only call the calculating function if the values haven't yet been calculated.
        getphenome();
        phenome = true;
    }
}

void muffetalgorithm::recalculate(){
    suborder = rand();
    getphenome();
    phenome = true;
}

void muffetalgorithm::getphenome(){
    unsigned int index, numtargets = settings.numtargets();
    unsigned int output, target, outputlimit; //We track the array output and compare this to the target value.
    unsigned int maxsize = settings.maxsize();   
    dynamicarray<unsigned int>& targetarray = *settings.targetarray();
    dynamicarraycrumb crumbarray(maxsize, filepath()); //We load the crumbarray.

    size = crumbarray.getsize(); //Size is set before the other phenome values, as its needed to initialise the following variable.
    bxcrumb bxcrumbarray[size];
    for (int index = 0; index < size; index++) {bxcrumbarray[index] = crumbarray[index].bx();} //The array is temporarily represented as a bxcrumb array (saves A LOT of processing time).

    errors = 0;
    margin = 0;

    for (index = 1; index <= numtargets; index++) { //Index begins at 1 so that zero is not passed to the interpreter.
    //We assess the crumbarray by passing the value of index and assessing how accurately it calculates the corresponding target.
        target = targetarray[index]; //We store the target value, this prevents us from calling the [] operand of dynamicarraycrumb multiple times.
        outputlimit = target * 2; //We do not allow the output to exceed twice the target value. This limit ensures a looping array has the same fitness as a non-functional array.
        output = execute(bxcrumbarray, index, outputlimit); 
        if (output < target) {margin += target - output; errors++;} //Where output and target do not match, we calculate the margin of error.
        else if (output > target) {margin += output - target; errors++;}
    }
}

unsigned int muffetalgorithm::execute(bxcrumb* bxcrumbarray, unsigned int input, unsigned int maxglobal1){
//Facilitates the interpretation of a crumb array, while preventing looping from occuring.
    unsigned int cycleinterval = settings.cycleinterval();
    interpreter interpret(input, settings.maxsize()); //The interpreter's maxproc value is set to the maximum size of the crumbarray.

    unsigned int cycle = cycleinterval;
    unsigned int index = 0;
    unsigned int lastglobal1 = interpret.getglobal1(); //This will be zero, but we set it from the interpreter for consistency.
    unsigned int global1;
   
    while (true) { //A continual loop, as we exit mid-loop.
        interpret(bxcrumbarray[index]);
        index++;

        if (index == size) {
            index = 0; //If cycles are not complete, we reset index
            cycle--; //and decrement cycle.

            if (cycle == 0) { //When cycles are complete, we check for progress and looping.
                global1 = interpret.getglobal1(); //If global1 has not changed, no progress is being made so we exit.
                if (lastglobal1 != global1 && global1 < maxglobal1) //If global1 has reached maximum, we are looping so we exit.
                    {lastglobal1 = global1;} //Save global1 for next comparison.
                else {return interpret.getoutput();} //This is the same as getglobal1(), but we call getoutput() for the sake of completion.
                cycle = cycleinterval; //If we will continue processing, we reset cycle.
            }
        }
    }
}

unsigned int muffetalgorithm::execute(unsigned int input, unsigned int outputlimit){
//A polymorph of the above execute() function that loads the crumbarray itself.  This allows for direct execution of the crumbarray, bypassing the controlled testing necessary within getphenome().
    unsigned int index, output;
    dynamicarraycrumb crumbarray(settings.maxsize(), filepath());
    unsigned int size = crumbarray.getsize();
    bxcrumb bxcrumbarray[size];

    for (int index = 0; index < size; index++) {bxcrumbarray[index] = crumbarray[index].bx();}
    output = execute(bxcrumbarray, input, outputlimit);
    return output;
}

void muffetalgorithm::mutate(dynamicarraycrumb& crumbarraychild){
//Here we randomly generate a set of copying errors and apply those errors.
//We copy the array represented by this muffetalogirthm object (the parent array) and apply those errors into the passed array (the child array).
    unsigned int maxsize = settings.maxsize();
    unsigned int mutationrate = settings.mutationrate();

    dynamicarraycrumb crumbarrayparent(maxsize, filepath()); //We load the crumbarray represented by this object, this will be the parent array.
    dynamicarray<unsigned int> errorpos(mutationrate); //An array used to record the position of copying errors (mutations).
    dynamicarray<bool> errortype(mutationrate); //An array used to record corresponding error types.

    unsigned int parentindex, childindex, errorindex, index2; //A lot of indexes are used in this function. Several different index variables make things easier for the programmer to track.
    unsigned int parentsize = crumbarrayparent.getsize();
    unsigned int numerrors;
    
    if (mutationrate > parentsize) {mutationrate = parentsize;} //We limit an algorithm's mutations to its size to prevent passing the value 0 in the call to rand() in the loop below.
    numerrors = rand(mutationrate); //We generate the number of copying errors, the remaining mutations will be flip errors (see below).

    //Here we determine the position and type of copying errors. A copying error is either a double copy or a failed copy.
    errorpos.changesize(numerrors);
    for (errorindex = 0; errorindex < numerrors; errorindex++) {
    //Here we simulate a shuffler algorithm, so that each number can only be selected once. (Applying multiple copying errors to the same number causes too many permutations.)
        errorpos[errorindex] = rand(parentsize - errorindex); //With each new number selected, there is one less number to choose from.
        for (index2 = 0; errorpos[errorindex] >= errorpos[index2] && index2 < errorindex; index2++) {errorpos[errorindex]++;}
        //We check the ascending sequence of previously selected numbers and increment the current number for each number with a lower value. This replicates the effect of a shuffler.
        bubblesort(&errorpos); //This ensures the array numbers are in order for the above checking process in the next loop.
        //Note that each of these bubblesort() calls will only loop through the array once, as at most one number is out of order. So this will take no longer than bubble sorting afterward would.
    }
    
    //We now set the type of errors for each position. The value true represents a duplicate copy, false represents a failed copy.
    errortype.changesize(numerrors);
    for (errorindex = 0; errorindex < numerrors; errorindex++) {errortype[errorindex] = rand(2);}

    crumbarraychild.changesize(maxsize); //Setting the size to maximum here prevents the need to manage incremental increases and decreases when applying copying errors.
    for (parentindex = 0, childindex = 0, errorindex = 0; parentindex < parentsize && childindex < maxsize; parentindex++, childindex++){
    //This for loop tracks three values: (1) The index being copied from the parent array, (2) the index being written in the child array, and (3) the index of the error array recording the next error.
    //This loop stops executing when we have either finished copying the parent array OR the child array has reached its maximum size.
        crumbarraychild[childindex] = crumbarrayparent[parentindex].bx(); //The copy instruction
        if (errorindex < numerrors && errorpos[errorindex] == parentindex) {
        //If we have not reached the end of the error array, we check whether we have reached the position of the next error.
            if (errortype[errorindex] == true) {parentindex--;} //For a duplicate copy we reduce the value of parentindex, so that the current parentindex will be copied again.
            else {childindex--;} //For a failed copy we reduce the value of childindex, so that the current value is rewritten.
            errorindex++; //We increment the index of the error array, so that the program will begin checking for the next position.
        }
    }
    crumbarraychild.changesize(childindex); //Finally, we set the size of the child array to correspond to the number of crumbs copied.
    
    numerrors = mutationrate - numerrors; //We calculate the remaining errors, these are flip errors. A flip error is a bit that is copied incorrectly. (It is simpler to apply these after copying.)
    for (errorindex = 0; errorindex < numerrors; errorindex++) {
        crumbarraychild[rand(childindex)].flip(rand(2));
        //These are generated in real time. With the position being randomly selected from the child array, and the bit to flip (0 or 1) being randomly selected.
    }
}

class outputfile {
private:
    filemngr* fileptr;
    bool fileopen;
    string header;
    unsigned int errors, margin, smallsize, bigsize;
    string filename;
    unsigned int subpop;
    string filepath() {return settings.folder() + "\\output.txt";}
    void openfile();
    void writeoutput();
    void newstrategy(unsigned int usererrors, unsigned int usermargin, unsigned int usersize, string userfilename);
    void count(unsigned int usererrors, unsigned int usermargin, unsigned int usersize, string userfilename);
public:
    outputfile(string header);
    ~outputfile();
    void note(unsigned int errors, unsigned int margin, unsigned int size, string filename);
};

outputfile::outputfile(string userheader){
    fileopen = false;
    header = userheader;
    subpop = 0;
}

outputfile::~outputfile(){
    if (fileopen == false && subpop > 0) {openfile();}
    if (fileopen == true) {
        writeoutput();
        (*fileptr).writeline();
        delete fileptr;
    }
}

void outputfile::openfile(){
    if (findfile(filepath()) == true) {fileptr = new filemngr(filepath(), app);}
    else {fileptr = new filemngr(filepath(), out);}
    (*fileptr).writeline(header + ":");
    fileopen = true;
}

void outputfile::writeoutput(){
    string line;

    if (fileopen == false) {openfile();}
    line = "errors: " + str(errors) + ", margin: " + str(margin) + ", size: " + str(smallsize);
    if (bigsize > smallsize) {line += "-" + str(bigsize);}
    line += ", pop: " + str(subpop);
    line += " (" + filename + ")";
    (*fileptr).writeline(line);
}

void outputfile::newstrategy(unsigned int usererrors, unsigned int usermargin, unsigned int usersize, string userfilename){
    errors = usererrors;
    margin = usermargin;
    smallsize = usersize;
    bigsize = usersize;
    filename = userfilename;
    subpop = 1;
}

void outputfile::count(unsigned int usererrors, unsigned int usermargin, unsigned int usersize, string userfilename){
    if (usererrors != errors || usermargin != margin) {
        writeoutput();
        newstrategy(usererrors, usermargin, usersize, userfilename);
    } else {
        bigsize = usersize;
        subpop++;
    }
}

void outputfile::note(unsigned int usererrors, unsigned int usermargin, unsigned int usersize, string userfilename){  
    if (subpop > 0) {count(usererrors, usermargin, usersize, userfilename);}
    else {newstrategy(usererrors, usermargin, usersize, userfilename);}
}

bool sortbyerrors(muffetalgorithm* algorithmptr1, muffetalgorithm* algorithmptr2){
muffetalgorithm& algorithm1 = *algorithmptr1;
muffetalgorithm& algorithm2 = *algorithmptr2;

    bool swap = false;

    if (algorithm1.errors != algorithm2.errors) {
        if (algorithm1.errors > algorithm2.errors) {swap = true;}
    } else if (algorithm1.margin != algorithm2.margin) {
        if (algorithm1.margin > algorithm2.margin) {swap = true;}
    } else if (algorithm1.size != algorithm2.size) {
        if (algorithm1.size > algorithm2.size) {swap = true;}
    } else if (algorithm1.suborder > algorithm2.suborder) {swap = true;}

    return swap;
}

bool sortbymargin(muffetalgorithm* algorithmptr1, muffetalgorithm* algorithmptr2){
muffetalgorithm& algorithm1 = *algorithmptr1;
muffetalgorithm& algorithm2 = *algorithmptr2;

    bool swap = false;
    
    if (algorithm1.margin != algorithm2.margin) {
        if (algorithm1.margin > algorithm2.margin) {swap = true;} 
    } else if (algorithm1.errors != algorithm2.errors) {
        if (algorithm1.errors > algorithm2.errors) {swap = true;}
    } else if (algorithm1.size != algorithm2.size) {
        if (algorithm1.size > algorithm2.size) {swap = true;}
    } else if (algorithm1.suborder > algorithm2.suborder) {swap = true;}
    
    return swap;
}

void bubblesortbyerrors(muffetalgorithm**, unsigned int, unsigned int);
void bubblesortbyerrors(muffetalgorithm* algorithm[], unsigned int arraysize) {bubblesortbyerrors(algorithm, 0, arraysize);}
void bubblesortbyerrors(muffetalgorithm* algorithm[], unsigned int indexstart, unsigned int indexend) {
    bool swap;
    muffetalgorithm* algorithmptr;
    unsigned int index, highindex;
    
    if (indexend == 0) {return;} //Prevents index from counting forever.
    highindex = indexend - 1;
   
    do {
        swap = false;
        for (index = indexstart; index < highindex; index++) {
            if (sortbyerrors(algorithm[index], algorithm[index+1]) == true) {
                algorithmptr = algorithm[index];
                algorithm[index] = algorithm[index+1];
                algorithm[index+1] = algorithmptr;
                swap = true;
            }
        }
    } while (swap == true);
}

void bubblesortbymargin(muffetalgorithm* algorithm[], unsigned int arraysize) {
    bool swap;
    muffetalgorithm* algorithmptr;
    unsigned int index;
    unsigned int highindex = arraysize - 1;
    
    do {
        swap = false;
        for (index = 0; index < highindex; index++) {
            if (sortbymargin(algorithm[index], algorithm[index+1]) == true) {
                algorithmptr = algorithm[index];
                algorithm[index] = algorithm[index+1];
                algorithm[index+1] = algorithmptr;
                swap = true;
            }
        }
    } while (swap == true);
}

void selecterrorniche(unsigned int niche, muffetalgorithm* algorithm[], unsigned int population) {
    unsigned int startofniche, endofniche, index;
    unsigned int seed = 0;
    unsigned int nichepop = settings.nichepop();
    outputfile* record = new outputfile("Errors " + str(niche));

    for (startofniche = 0; startofniche < population && (*algorithm[startofniche]).errors != niche; startofniche++);
    for (endofniche = startofniche; endofniche < population && (*algorithm[endofniche]).errors == niche; endofniche++);
    
    if (startofniche + nichepop < endofniche 
    && (*algorithm[startofniche]).margin == (*algorithm[startofniche + nichepop]).margin 
    && (*algorithm[startofniche]).errors == (*algorithm[startofniche + nichepop]).errors)
    {
        if (startofniche > 0) {endofniche = 0;}
        else {endofniche = 1;}
    }

    for (index = startofniche; index < endofniche && seed < nichepop; index++, seed++){
        (*algorithm[index]).eggs++;
        (*record).note((*algorithm[index]).errors, (*algorithm[index]).margin, (*algorithm[index]).size, (*algorithm[index]).getfilename());
    }
    delete record;
}

void selectmarginniche(unsigned int, unsigned int, muffetalgorithm*, unsigned int);
void selectmarginniche(unsigned int minmargin, muffetalgorithm* algorithm[], unsigned int population) {selectmarginniche(minmargin, (*algorithm[population-1]).margin, algorithm, population);}
void selectmarginniche(unsigned int minmargin, unsigned int maxmargin, muffetalgorithm* algorithm[], unsigned int population) {
    outputfile* record;
    unsigned int startofniche, endofniche, index;
    unsigned int seed = 0;
    unsigned int nichepop = settings.nichepop();

    if (maxmargin == minmargin) {record = new outputfile("Margin " + str(minmargin));}
    else if (maxmargin == (*algorithm[population-1]).margin) {record = new outputfile("Margin " + str(minmargin) + "+");}
    else {record = new outputfile("Margin " + str(minmargin) + "-" + str(maxmargin));}
    
    for (startofniche = 0; startofniche < population && (*algorithm[startofniche]).margin < minmargin; startofniche++);
    for (endofniche = startofniche; endofniche < population && (*algorithm[endofniche]).margin <= maxmargin; endofniche++);
    bubblesortbyerrors(algorithm, startofniche, endofniche);

    if (startofniche + nichepop < endofniche 
    && (*algorithm[startofniche]).margin == (*algorithm[startofniche + nichepop]).margin 
    && (*algorithm[startofniche]).errors == (*algorithm[startofniche + nichepop]).errors)
    {
        if (startofniche > 0) {endofniche = 0;}
        else {endofniche = 1;}
    }

    for (index = startofniche; index < endofniche && seed < nichepop; index++, seed++){
        (*algorithm[index]).eggs++;
        (*record).note((*algorithm[index]).errors, (*algorithm[index]).margin, (*algorithm[index]).size, (*algorithm[index]).getfilename());                
    }
    delete record;
}

unsigned int tidyuparray(muffetalgorithm* algorithm[], unsigned int algorithmarraysize) {
    bool swap;
    muffetalgorithm* algorithmptr;
    unsigned int index;
    unsigned int highindex = algorithmarraysize - 1;
    
    do {
        swap = false;
        for (index = 0; index < highindex; index++) {
            if (algorithm[index] == 0 && algorithm[index+1] != 0) {
                algorithmptr = algorithm[index];
                algorithm[index] = algorithm[index+1];
                algorithm[index+1] = algorithmptr;
                swap = true;
            }
        }
    } while (swap == true);

    for (index = 0; algorithm[index] != 0; index++);
    return index;
}

void writehtmlfile(unsigned int population);

int main(int argc, char *argv[])
{
    unsigned int index, progressbar;
    bool recalculate;
    unsigned int numtargets;
    unsigned int minmargin, maxmargin;
    unsigned int parentpop, parentindex, childindex;
    unsigned int population;
    unsigned int maxpop = settings.maxpop();
    filemngr* fileptr;
    string arrayfile;
    string indexfile = settings.folder() + "\\index.txt";
    muffetalgorithm* algorithm[maxpop];
    
    randomise();
    settings.load();
    for (index = 0; index < maxpop; index++) {algorithm[index] = 0;}

    parentpop = 0;
    if (findfile(indexfile) == false) {
        cout << "Creating..." << endl;
        population = maxpop;
        for (index = 0; index < population; index++) {
            algorithm[index] = new muffetalgorithm;
        }
    } else {
        cout << "Loading..." << endl;
        fileptr = new filemngr(indexfile, in);
        counter.settime((*fileptr).readunsignedint());
        (*fileptr).readline(); //Necessary so file reading will move to the next line.
        population = 0;
        for (index = 0; arrayfile = (*fileptr).readline(), (*fileptr).geteof() == false; index++) {
            algorithm[index] = new muffetalgorithm(arrayfile);
            population++;
        }
        delete fileptr;
        goto CALCULATING; //Indexing is skipped after loading.
    }

    INDEXING:
    cout << "Indexing..." << endl;
    fileptr = new filemngr(indexfile, out);
    (*fileptr).writeline(counter.time());
    for (index = 0; index < population; index++) {(*fileptr).writeline((*algorithm[index]).getfilename());}
    delete fileptr;
    
    CALCULATING:
    cout << "Calculating..." << endl;
    recalculate = false;
    progressbar = (population - parentpop) / 14;
    for (index = 0; index < population; index++) {
        if (index > parentpop && index % progressbar == 0) {cout << ".";}
        (*algorithm[index]).calculate();
        if ((*algorithm[index]).geterrors() == 0 && settings.numtargets() < settings.maxtargets()) {
            recalculate = true;
            index = population;
        }
    }
    cout << endl;

while (recalculate == true) {
    cout << "Recalculating..." << endl;
    recalculate = false;
    settings.levelup();
    progressbar = population / 16;
    for (index = 0; index < population; index++) {
        if (index % progressbar == 0 && index != 0) {cout << ".";}
        (*algorithm[index]).recalculate();
        if ((*algorithm[index]).geterrors() == 0 && settings.numtargets() < settings.maxtargets()) {
            recalculate = true;
            index = population;
        }
    }
    cout << endl;
}

    cout << "Selecting..." << endl; 
    numtargets = settings.numtargets();
    bubblesortbyerrors(algorithm, population);
    for (index = 0; index <= numtargets; index++) {
        selecterrorniche(index, algorithm, population);
    }
    
    bubblesortbymargin(algorithm, population);
    for (minmargin = 0, maxmargin = 0, index = 0; index < numtargets; index++, minmargin = maxmargin + 1, maxmargin += (*settings.targetarray())[index]) {
        selectmarginniche(minmargin, maxmargin, algorithm, population);
    }
    selectmarginniche(minmargin, algorithm, population);

    cout << "Deleting..." << endl;   
    for (index = 0; index < population; index++) {
        if ((*algorithm[index]).eggs == 0) {
            delete algorithm[index];
            algorithm[index] = 0;
        }
    }
    population = tidyuparray(algorithm, population);

    cout << "Outputing..." << endl << endl;
    writehtmlfile(population);
    cout << "Clock: " << counter.time() << endl;

    counter.ticktock();
    cout << "Mutating..." << endl;
    parentpop = population;
    for (childindex = population, parentindex = 0; parentindex < parentpop; parentindex++) {
        while ((*algorithm[parentindex]).eggs > 0) {
            algorithm[childindex] = new muffetalgorithm((*algorithm[parentindex]));
            population++;
            childindex++;
            (*algorithm[parentindex]).eggs--;
        }
    }
    goto INDEXING;

    system("PAUSE");
    return EXIT_SUCCESS;
}

void writehtmlfile(unsigned int population){
    string datafilepath = settings.folder() + "\\output.txt";
    filemngr htmlfile("Miss Muffet.htm", out);
    filemngr* datafile = new filemngr(datafilepath, in);

    htmlfile.writeline("<HTML>");
    htmlfile.writeline("<HEAD>");
    htmlfile.writeline("<TITLE>Miss Muffet</TITLE>");
    htmlfile.writeline("<BODY>");
    htmlfile.writeline("<FONT FACE=\"Arial\">");
    htmlfile.writeline("Level: " + str(settings.numtargets()) + "<BR>");
    htmlfile.writeline("Clock: " + counter.time() + "<BR>");
    htmlfile.writeline("Population: " + str(population) + "<BR>");
    htmlfile.writeline("<BR>");
    while ((*datafile).geteof() == false) {
        htmlfile.writeline((*datafile).readline() + "<BR>");
    }
    htmlfile.writeline("</FONT>");
    htmlfile.writeline("</BODY>");
    htmlfile.writeline("</HTML>");
    
    delete datafile;
    remove(datafilepath);
}
