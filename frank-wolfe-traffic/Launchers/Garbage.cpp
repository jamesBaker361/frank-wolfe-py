#include <iostream>

class Garbage{
    public:
        Garbage(int garbageInt) : garbageInt(garbageInt) {
            std::cout << "Garbage created with address "<< this<< std::endl;
        }
        ~Garbage() {
            std::cout << "deleted garbage "<< garbageInt << "address = "<< this<< std::endl;
        }

        Garbage(const Garbage & old){
            std::cout << "Garbage copied with new address "<< this  <<std::endl;
            garbageInt = old.garbageInt;
        }

    private:
        int garbageInt;
};