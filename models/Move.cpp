#include "InfoIteration.cpp"
#include <string>
#include <vector>

using namespace std;

class Move {
    private:
        string name;
        vector<InfoIteration> infoIteration;
    public:
        Move() : name(""), infoIteration() {}

        Move(string name, vector<InfoIteration> infoIteration)
            : name(name), infoIteration(infoIteration) {}

        string getName() const {
            return name;
        }

        vector<InfoIteration> getInfoIteration() const {
            return infoIteration;
        }

        void setName(string name) {
            this->name = name;
        }

        void setInfoIteration(vector<InfoIteration> infoIteration) {
            this->infoIteration = infoIteration;
        }
};