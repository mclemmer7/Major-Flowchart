#include "coursenode.h"

#include <string>
#include <sstream>

using std::string;
using std::stringstream;

// Constructor for CourseNode that sets the courseCode, name, credits, requirements, and quarters to the given values.
CourseNode::CourseNode(string code, string name, int credits, vector<string> reqs, vector<int> quarters) :
_courseCode(code), _name(name), _credits(credits), _requirements(reqs), _quarters(quarters) {
}

// Returns a string representation of the courseNode.
// Example: CSC 3430, Algorithms Analysis and Design, 4, [MAT 2200, CSC 2431], [2]
string CourseNode::ToString() const {
    stringstream retVal;
    retVal << _courseCode << ", " << _name << ", " << _credits << ", [";
    for (int i = 0; i < _requirements.size(); i++) {
        if (i == _requirements.size() - 1) {
            retVal << _requirements[i];
        } else {
            retVal << _requirements[i] << ", ";
        }
    }
    retVal << "], [";
    for (int i = 0; i < _quarters.size(); i++) {
        if (i == _quarters.size() - 1) {
            retVal << _quarters[i];
        } else {
            retVal << _quarters[i] << ",";
        }
    }
    retVal << "]";
    return retVal.str();
}

void CourseNode::AddArc(int id) {
    _arcs.push_back(id);
}

void CourseNode::SetPriority(int i) {
    _priority = i;
}

void CourseNode::SetCoords(vector<int> v) {
    _coords = v;
}

// Returns the course code
string CourseNode::GetCourseCode() const {
    return _courseCode;
}

// Returns the course name
string CourseNode::GetName() const {
    return _name;
}

// Returns the number of credits
int CourseNode::GetCredits() const {
    return _credits;
}

// Returns the required classes for this course
vector<string> CourseNode::GetRequirements() const {
    return _requirements;
}

// Returns the quarters that this course is offered
vector<int> CourseNode::GetQuarters() const {
    return _quarters;
}

vector<int> CourseNode::GetArcs() const {
    return _arcs;
}

int CourseNode::GetPriority() const {
    return _priority;
}

vector<int> CourseNode::GetCoords() const {
    return _coords;
}
