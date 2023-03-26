#include <string>
#include <vector>

using namespace std;


#ifndef FLOWCHART_THE_FELLOWSHIP_COURSENODE_H
#define FLOWCHART_THE_FELLOWSHIP_COURSENODE_H


class CourseNode {
    string _courseCode;
    string _name;
    int _credits;
    vector<string> _requirements;
    vector<int> _quarters;
    vector<int> _arcs;
    int _priority = -1;
    vector<int> _coords;

public:
    CourseNode(string code, string name, int credits, vector<string> reqs, vector<int> quarters);
    string ToString()const;
    void AddArc(int id);
    void SetPriority(int i);
    void SetCoords(vector<int> v);
    string GetCourseCode()const;
    string GetName()const;
    int GetCredits()const;
    vector<string> GetRequirements()const;
    vector<int> GetQuarters()const;
    vector<int> GetArcs()const;
    int GetPriority()const;
    vector<int> GetCoords()const;
};

#endif //FLOWCHART_THE_FELLOWSHIP_COURSENODE_H