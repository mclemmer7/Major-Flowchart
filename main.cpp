/*
 * TODO: Make sure there are no memory leaks.
 * TODO: Make createNode() a constructor within the class and not a function here?
 * TODO: Sort the nodes in the graph so searching for pre-reqs takes O(logn) instead of O(n).
 * TODO: Add a check to make sure the maxCredits is above 5 (or maybe we might have 12 minimum).
 * TODO: Try passing a set by reference into pickClasses, and editing that set as I go through.
 * TODO: Double check the interval partitioning algorithm and maybe rewrite pickClasses to use it.
 */

/*
 * This code works!
 * g++ -I./lemon-1.3.1 -I./lemon-1.3.1/build main.cpp coursenode.cpp coursenode.h
 *
 * ---- There are currently two main approaches to generating the flowchart ----
 * Approach 1:
 *      Generate a vector with all of the CourseNodes, sort the vector, then add each element to our graph
 *      Benefits: Since the ids will be sorted, finding pre-reqs will be in O(logn) time.
 *      Downsides: We need O(n) to add into vector, O(nlogn) to sort, and another O(n) to add to graph.
 *      Conclusion: Would probably be better for a large number of classes, or for a lot of pre-reqs
 * Approach 2:
 *      Benefits: Only O(n) to add into graph.
 *      Downsides: O(n) every time we want to add a pre-req.
 *      Conclusion: Would probably be better for smaller number of classes, or for fewer pre-reqs.
 * Final Conclusion:
 *      Approach 1 seems better, but is a harder to code. We will add it later and will go with approach 2
 *      for the time being.
 *
 *
 * ---- Traversing through the graph on our own ----
 *      I can't find a way to get a list of arcs given a node in lemon. Not sure why. What I can do is when we
 *      generate the arcs, I can store the id of the arc in the CourseNode class. We can access this with a getter.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <cstring>
#include <cstdlib>
#include "coursenode.h"
#include <lemon/smart_graph.h>

using namespace std;
using namespace lemon;

CourseNode* createNode(string input, SmartDigraph::NodeMap<CourseNode*>& data, SmartDigraph& graph); // This should probably be a constructor in the courseNode.cpp file.
set<int> pickClasses(set<int> s, int maxCredits, int year, SmartDigraph::NodeMap<CourseNode*>& data, SmartDigraph& graph); // Returns optimal set of classes that is under the max credits given a set of classes.
void assignPriority(int id, SmartDigraph::NodeMap<CourseNode*>& data, SmartDigraph& graph);
void createOutput(SmartDigraph::NodeMap<CourseNode*>& data, SmartDigraph& graph, set<int> availableClasses[3], int maxCredits, int startQuarter);

void lineRenderer(int start_x, int start_y, int end_x, int end_y, int rgb[3]);
void box(int x, int y, int width, int height, int rgb[3]);
void letter(int x, int y, char letter, int rgb[3]);
void word(int x, int y, string s, int rgb[3]);

const int width = 1100, height = 450;
int arr[width][height][3];

int main() {
    /*
    int rgb[3] = {255,0,0};
    Picture* pic = new Picture(512,255,rgb);
    int rgb2[3] = {255,255,255};
    //pic->placeBox(30,10,100,20,rgb2);
    pic->createPPM("pic.ppm");
     */

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            arr[i][j][0] = 230;
            arr[i][j][1] = 230;
            arr[i][j][2] = 230;
        }
    }

    set<int> availableClasses[3]; // Array of sets of node ids of courses that are available. Array index determines quarter.
    set<int> finalClasses[4][3];
    // graph and data are the heart of the graph
    SmartDigraph graph;
    SmartDigraph::NodeMap<CourseNode*> data(graph);

    string line;
    ifstream readFile;
    string inputFile;

    cout << "major1.txt contains courses for a Computer Science major and major2.txt contains courses"
            " for a Biochemistry major." << endl;
    while (!readFile.is_open()) {
        cout << "Please enter [major1.txt] or [major2.txt] to select your major." << endl;
        cin >> inputFile;
        if (inputFile == "major1.txt" || inputFile == "major2.txt") {
            // Use "../" + inputFile to run in CLion, and inputFile to run in ubuntu
            readFile.open(inputFile);
        }
    }
    // The file is open now, so get each line and create the CourseNodes
    while (!readFile.eof()) {
        getline(readFile, line);
        // Create the node with the given line that contains a course
        CourseNode* course = createNode(line, data, graph);
        // Add node to graph here
        SmartDigraph::Node n = graph.addNode();
        data[n] = course;
    }
    readFile.close();

    // Iterate through the graph to add the edges/arcs
    for (SmartDigraph::NodeIt n(graph); n != INVALID; ++n) {
         // Traverse through the requirements vector for the current node
         for (int i = 0; i < data[n]->GetRequirements().size(); i++) {
             string req = data[n]->GetRequirements()[i];
             for (SmartDigraph::NodeIt j(graph); j != INVALID; ++j) {
                 if (data[j]->GetCourseCode() == req) {
                     // Add the arc if the course code is equal to the current requirement
                     // The arc should be directed from data[j] to data[n]
                     SmartDigraph::Arc a = graph.addArc(j, n);
                     data[j]->AddArc(graph.id(a));
                 }
             }
         }
    }

    // Asking the user start quarter and maximum number of credits
    int maxCredits = 0;
    int startQuarter = 0;
    string input;

    cout << "Would you like to enter constraints for the maximum credits and starting quarter?" << endl;
    while (input != "y" && input != "n") {
        cout << "Please enter [y] for yes or [n] for no." << endl;
        cin >> input;
    }

    if (input == "y") {
        // Take the user input as a string, then try to convert it to an integer. If this doesn't work,
        // tell the user to input an integer. The input must be between 5 and 18 credits
        while (maxCredits < 12 || maxCredits > 18) {
            cout << "Please enter the maximum number of credits you would like to take per quarter (12 - 18)" << endl;
            cin >> input;
            try {
                maxCredits = stoi(input);
            } catch (exception &err) {
                cout << "Please enter an integer." << endl;
            }
        }
        // The input must be 1, 2, or 3 for the quarter selection. Anything else won't work, and the user
        // must try again.
        cout << "Please enter the quarter you are starting school" << endl;
        while (startQuarter < 1 || startQuarter > 3) {
            cout << "[1] for Autumn, [2] for Winter, and [3] for Spring" << endl;
            cin >> input;
            try {
                startQuarter = stoi(input);
            } catch (exception &err) {
                cout << "Please enter an integer." << endl;
            }
        }
        cout << "\nYou will take no more than " << maxCredits << " credits per quarter and are starting in quarter ";
        cout << startQuarter << "." << endl << endl;
    } else if (input == "n") {
        maxCredits = 18;
        startQuarter = 1;
    }

    // Generating a Set of classes the user actually has available.
    set<int> rootCourses;
    for (SmartDigraph::NodeIt n(graph); n != INVALID; ++n) {
        CourseNode* course = data[n];
        if (course->GetRequirements()[0] == "") { // If number of pre-requirements is zero
            rootCourses.insert(graph.id(n));
            vector<int> v = course->GetQuarters();
            for (int j = 0; j < v.size(); j++) {
                availableClasses[v[j] - 1].insert(graph.id(n));
            }
        }
    }

    // Driver function for assignPriority(), since assignPriority() is a DFS recursive function.
    for (set<int>::iterator itr = rootCourses.begin(); itr != rootCourses.end(); itr++) {
        assignPriority(*itr, data, graph);
    }

    // Now call createOutput, which will write the courses in the correct order into the output.txt file
    createOutput(data, graph, availableClasses, maxCredits, startQuarter);
    cout << "output.txt file has been created." << endl;

    ofstream img ("no_edges_picture.ppm");
    img << "P3" << endl;
    img << width << " " << height << endl;
    img << "255" << endl;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = arr[x][y][0];
            int g = arr[x][y][1];
            int b = arr[x][y][2];

            img << r << " " << g << " " << b << endl;
        }
    }
    img.close();

    cout << "Picture without edges has been generated." << endl;


    // Driver function for the lines between classes.
    int rgb[3];
    for(SmartDigraph::NodeIt n(graph); n != INVALID; ++n) {
        vector<int> svect = data[n]->GetCoords();
        rgb[0] = rand() % 306 - 50;
        rgb[1] = rand() % 306 - 50;
        rgb[2] = rand() % 306 - 50;
        for(SmartDigraph::OutArcIt a(graph, n); a != INVALID; ++a) {
            vector<int> evect = data[graph.target(a)]->GetCoords();
            lineRenderer(svect.at(0) + 40, svect.at(1) + 7, evect.at(0) - 1, evect.at(1) + 7, rgb);
        }
    }

    ofstream img2 ("edges_picture.ppm");
    img2 << "P3" << endl;
    img2 << width << " " << height << endl;
    img2 << "255" << endl;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int r = arr[x][y][0];
            int g = arr[x][y][1];
            int b = arr[x][y][2];

            img2 << r << " " << g << " " << b << endl;
        }
    }
    img2.close();
    cout << "Picture with edges has been generated." << endl;

    // Destructor
    for (SmartDigraph::NodeIt n(graph); n != INVALID; ++n) {
        delete data[n];
    }

    return 0;
};

/* This function is given a string input and pulls chunks of the string out with  getline delimiters.
 * It sends the courseCode, name, credits, requirements, and quarters offered into the CourseNode constructor
 * to create a new CourseNode object.
 * Returns a pointer to the CourseNode object that is created
 */
CourseNode* createNode(string input, SmartDigraph::NodeMap<CourseNode*>& data, SmartDigraph& graph) {
    stringstream stream(input);
    string courseCode;
    string name;
    // int credits;
    string creditString;
    string reqs;
    vector<string> requirements;
    string terms;
    vector<int> quarters;

    getline(stream, courseCode, ',');
    getline(stream, name, ',');
    name.erase(0,1);
    getline(stream, creditString, ',');
    creditString.erase(0,1);
    int credits = stoi(creditString);

    // Now get the requirements to take this course from the stream input
    getline(stream, reqs, ']');
    // Now delete the opening bracket '[' and then traverse through this string, delimited by commas
    reqs.erase(0,2);

    stringstream reqStream(reqs);
    string course;
    int count = 0;
    while (!reqStream.eof()) {
        getline(reqStream, course, ',');
        if (count != 0) {
            // Remove the space before each course, except for the first course, which has no space before it
            course.erase(0,1);
        }
        count++;
        requirements.push_back(course);
    }

    // Now get the quarters that offer this course from the stream input
    getline(stream, terms, ']');
    terms.erase(0,3);
    stringstream termStream(terms);
    string quarter;
    while (!termStream.eof()) {
        getline(termStream, quarter, ',');
        int temp = stoi(quarter);
        quarters.push_back(temp);
    }
    // Create the CourseNode and return it
    CourseNode* node = new CourseNode(courseCode, name, credits, requirements, quarters);
    return node;
}

// Greedy algorithm that picks the classes based on their order of priority.
set<int> pickClasses(set<int> s, int maxCredits, int year, SmartDigraph::NodeMap<CourseNode*>& data, SmartDigraph& graph) {
    int creditsLeft = maxCredits;
    set<int> retSet; // Return Set
    while(true) {
        int bestCourse = -1;

        // Iterate through every class in the set of available classes, test if they are under the current creditsLeft, and then set its ID as bestCourse if it has the highest priority so far
        set<int>::iterator itr;
        for (itr = s.begin(); itr != s.end(); itr++) {
            // This stops CSC 3000 and CSC 4941 from being taken before they should be taken
            if (data[graph.nodeFromId(*itr)]->GetCourseCode() != "CSC 3000" &&
            data[graph.nodeFromId(*itr)]->GetCourseCode() != "CSC 4941" &&
            data[graph.nodeFromId(*itr)]->GetCourseCode() != "CHM 4899") {
                if (bestCourse == -1 || data[graph.nodeFromId(*itr)]->GetPriority() >
                data[graph.nodeFromId(bestCourse)]->GetPriority()) {
                    if (data[graph.nodeFromId(*itr)]->GetCredits() <= creditsLeft) {
                        bestCourse = *itr; // Setting bestCourse as class ID with the highest priority out of possible classes searched.
                    }
                }
            } else if (year >= 3 && data[graph.nodeFromId(*itr)]->GetCourseCode() == "CSC 3000") {
                // This makes sure that CSC 3000 is only taken on the 3rd year or after
                if (bestCourse == -1 || data[graph.nodeFromId(*itr)]->GetPriority() >
                data[graph.nodeFromId(bestCourse)]->GetPriority()) {
                    if (data[graph.nodeFromId(*itr)]->GetCredits() <= creditsLeft) {
                        bestCourse = *itr;
                    }
                }
            } else if (year >= 4 && (data[graph.nodeFromId(*itr)]->GetCourseCode() == "CSC 4941" ||
            data[graph.nodeFromId(*itr)]->GetCourseCode() == "CHM 4899")) {
                // This makes sure that CSC 4941 or CHM 4899 is only taken on the 4th year or after
                if (bestCourse == -1 || data[graph.nodeFromId(*itr)]->GetPriority() >
                data[graph.nodeFromId(bestCourse)]->GetPriority()) {
                    if (data[graph.nodeFromId(*itr)]->GetCredits() <= creditsLeft) {
                        bestCourse = *itr;
                    }
                }
            }
        }
        if (bestCourse == -1) { // Only occurs when the iterator loops through all classes and it can't take any more classes.
            return retSet;
        }
        retSet.insert(bestCourse);
        s.erase(bestCourse);
        creditsLeft -= data[graph.nodeFromId(bestCourse)]->GetCredits();
    }
}

// Recursive DFS that counts the priority of each node as it traverses back up the graph.
void assignPriority(int id, SmartDigraph::NodeMap<CourseNode*>& data, SmartDigraph& graph) {
    SmartDigraph::Node node = graph.nodeFromId(id);

    // If the node's priority was already set using a previous call of assignPriority() on a different root node, don't traverse that path of classes again.
    // This works because -1 is the default priority of a class, it means it's priority hasn't been generated yet.
    if (data[node]->GetPriority() > -1) { // DYNAMIC PROGRAMMING!!!!
        return;
    }

    // Call assignPriority() on every child node.
    int max = -1;
    for (SmartDigraph::OutArcIt a(graph, node); a != INVALID; ++a) {
        assignPriority(graph.id(graph.target(a)), data, graph);
        
        // Picking the child node with the greatest priority, and basing the priority of the node entered off of that one.
        if (data[graph.target(a)]->GetPriority() > max) {
            max = data[graph.target(a)]->GetPriority();
        }
    }

    // for loop is skipped if there are no children, since max is set to -1, priority is set to 0. This is the base case.
    data[node]->SetPriority(max + 1);
}

void box(int x, int y, int width, int height, int rgb[3]) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            arr[x + i][y + j][0] = rgb[0];
            arr[x + i][y + j][1] = rgb[1];
            arr[x + i][y + j][2] = rgb[2];
        }
    }
}

void lineRenderer(int start_x, int start_y, int end_x, int end_y, int rgb[3]) {
    float rise = end_y - start_y;
    float run = end_x - start_x;
    float slope = rise / run;
    for (int x = start_x; x < end_x; x++) {
        int y = int(start_y + (slope * (x - start_x)));
        arr[x][y][0] = rgb[0];
        arr[x][y][1] = rgb[1];
        arr[x][y][2] = rgb[2];
    }
}

void word(int x, int y, string s, int rgb[3]){
    for (int i = 0; i < s.size(); i++) {
        letter(x + (i * 4),y,s.at(i),rgb);
    }
}

void letter(int x, int y, char letter, int rgb[3]) {
    int letArr[5][3];
    switch (int(letter)) {
        case 32: // Space
        {
            int a[5][3] = {0, 0, 0,
                           0, 0, 0,
                           0, 0, 0,
                           0, 0, 0,
                           0, 0, 0};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 48: // Number 0
        {
            int a[5][3] = {0, 1, 0,
                           1, 0, 1,
                           1, 1, 1,
                           1, 0, 1,
                           0, 1, 0};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 49: // Number 1
        {
            int a[5][3] = {0, 1, 0,
                           1, 1, 0,
                           0, 1, 0,
                           0, 1, 0,
                           1, 1, 1};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 50: // Number 2
        {
            int a[5][3] = {0, 1, 0,
                           1, 0, 1,
                           0, 0, 1,
                           1, 1, 0,
                           1, 1, 1};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 51: // Number 3
        {
            int a[5][3] = {1, 1, 0,
                           0, 0, 1,
                           1, 1, 0,
                           0, 0, 1,
                           1, 1, 0};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 52: // Number 4
        {
            int a[5][3] = {1, 0, 1,
                           1, 0, 1,
                           1, 1, 1,
                           0, 0, 1,
                           0, 0, 1};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 53: // Number 5
        {
            int a[5][3] = {1, 1, 1,
                           1, 0, 0,
                           1, 1, 0,
                           0, 0, 1,
                           1, 1, 1};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 54: // Number 6
        {
            int a[5][3] = {0, 1, 1,
                           1, 0, 0,
                           1, 1, 0,
                           1, 0, 1,
                           1, 1, 1};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 55: // Number 7
        {
            int a[5][3] = {1, 1, 1,
                           0, 0, 1,
                           0, 1, 0,
                           0, 1, 0,
                           0, 1, 0};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 56: // Number 8
        {
            int a[5][3] = {0, 1, 0,
                           1, 0, 1,
                           0, 1, 0,
                           1, 0, 1,
                           0, 1, 0};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 57: // Number 9
        {
            int a[5][3] = {0, 1, 1,
                           1, 0, 1,
                           0, 1, 1,
                           0, 0, 1,
                           1, 1, 0};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 65: // Letter A
        {
            int a[5][3] = {0, 1, 0,
                           1, 0, 1,
                           1, 1, 1,
                           1, 0, 1,
                           1, 0, 1};
            memcpy(letArr, a, sizeof(letArr));
            break;
        }
        case 66: // Letter B
        {
            int b[5][3] = {1, 1, 0,
                           1, 0, 1,
                           1, 1, 0,
                           1, 0, 1,
                           1, 1, 0};
            memcpy(letArr, b, sizeof(letArr));
            break;
        }
        case 67: // Letter C
        {
            int c[5][3] = {1, 1, 1,
                           1, 0, 0,
                           1, 0, 0,
                           1, 0, 0,
                           1, 1, 1};
            memcpy(letArr, c, sizeof(letArr));
            break;
        }
        case 68: // Letter D
        {
            int d[5][3] = {1, 1, 0,
                           1, 0, 1,
                           1, 0, 1,
                           1, 0, 1,
                           1, 1, 0};
            memcpy(letArr, d, sizeof(letArr));
            break;
        }
        case 69: // Letter E
        {
            int e[5][3] = {1, 1, 1,
                           1, 0, 0,
                           1, 1, 1,
                           1, 0, 0,
                           1, 1, 1};
            memcpy(letArr, e, sizeof(letArr));
            break;
        }
        case 70: // Letter F
        {
            int f[5][3] = {1, 1, 1,
                           1, 0, 0,
                           1, 1, 1,
                           1, 0, 0,
                           1, 0, 0};
            memcpy(letArr, f, sizeof(letArr));
            break;
        }
        case 71: // Letter G
        {
            int g[5][3] = {1, 1, 1,
                           1, 0, 0,
                           1, 0, 1,
                           1, 0, 1,
                           1, 1, 1};
            memcpy(letArr, g, sizeof(letArr));
            break;
        }
        case 72: // Letter H
        {
            int g[5][3] = {1, 0, 1,
                           1, 0, 1,
                           1, 1, 1,
                           1, 0, 1,
                           1, 0, 1};
            memcpy(letArr, g, sizeof(letArr));
            break;
        }
        case 73: // Letter I
        {
            int i[5][3] = {1, 1, 1,
                           0, 1, 0,
                           0, 1, 0,
                           0, 1, 0,
                           1, 1, 1};
            memcpy(letArr, i, sizeof(letArr));
            break;
        }
        case 76: // Letter L
        {
            int i[5][3] = {1, 0, 0,
                           1, 0, 0,
                           1, 0, 0,
                           1, 0, 0,
                           1, 1, 1};
            memcpy(letArr, i, sizeof(letArr));
            break;
        }
        case 77: // Letter M
        {
            int m[5][3] = {1, 0, 1,
                           1, 1, 1,
                           1, 1, 1,
                           1, 0, 1,
                           1, 0, 1};
            memcpy(letArr, m, sizeof(letArr));
            break;
        }
        case 78: // Letter N
        {
            int n[5][3] = {1, 0, 1,
                           1, 1, 1,
                           1, 1, 1,
                           1, 1, 1,
                           1, 0, 1};
            memcpy(letArr, n, sizeof(letArr));
            break;
        }
        case 79: // Letter O
        {
            int o[5][3] = {1, 1, 1,
                           1, 0, 1,
                           1, 0, 1,
                           1, 0, 1,
                           1, 1, 1};
            memcpy(letArr, o, sizeof(letArr));
            break;
        }
        case 80: // Letter P
        {
            int p[5][3] = {1, 1, 0,
                           1, 0, 1,
                           1, 1, 0,
                           1, 0, 0,
                           1, 0, 0};
            memcpy(letArr, p, sizeof(letArr));
            break;
        }
        case 81: // Letter Q
        {
            int p[5][3] = {1, 1, 1,
                           1, 0, 1,
                           1, 0, 1,
                           1, 1, 0,
                           0, 0, 1};
            memcpy(letArr, p, sizeof(letArr));
            break;
        }
        case 82: // Letter R
        {
            int r[5][3] = {1, 1, 0,
                           1, 0, 1,
                           1, 1, 0,
                           1, 0, 1,
                           1, 0, 1};
            memcpy(letArr, r, sizeof(letArr));
            break;
        }
        case 83: // Letter S
        {
            int s[5][3] = {1, 1, 1,
                           1, 0, 0,
                           1, 1, 1,
                           0, 0, 1,
                           1, 1, 1};
            memcpy(letArr, s, sizeof(letArr));
            break;
        }
        case 84: // Letter T
        {
            int t[5][3] = {1, 1, 1,
                           0, 1, 0,
                           0, 1, 0,
                           0, 1, 0,
                           0, 1, 0};
            memcpy(letArr, t, sizeof(letArr));
            break;
        }
        case 85: // Letter U
        {
            int u[5][3] = {1, 0, 1,
                           1, 0, 1,
                           1, 0, 1,
                           1, 0, 1,
                           1, 1, 1};
            memcpy(letArr, u, sizeof(letArr));
        }
            break;
        case 87: // Letter W
        {
            int w[5][3] = {1, 0, 1,
                           1, 0, 1,
                           1, 1, 1,
                           1, 1, 1,
                           1, 0, 1};
            memcpy(letArr, w, sizeof(letArr));
            break;
        }
        case 89: // Letter Y
        {
            int y[5][3] = {1, 0, 1,
                           1, 0, 1,
                           0, 1, 0,
                           0, 1, 0,
                           0, 1, 0};
            memcpy(letArr, y, sizeof(letArr));
            break;
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            if (letArr[j][i] == 1) {
                arr[x + i][y + j][0] = rgb[0];
                arr[x + i][y + j][1] = rgb[1];
                arr[x + i][y + j][2] = rgb[2];
            }
        }
    }
}
/* This function creates the output.txt file that contains all of the courses in the order that they
 * should be taken. This output.txt file will be taken into python and run to create the display of the flowchart.
 * This function takes in the graph along with the data NodeMap, the available classes set, the max credits per
 * quarter, and the quarter that the user plans to start taking the classes in.
 */
void createOutput(SmartDigraph::NodeMap<CourseNode*>& data, SmartDigraph& graph, set<int> availableClasses[3], int maxCredits, int startQuarter) {
    // File output to .txt file to be read and used in python to create a display of the classes
    // Use ../output.txt in CLion, and output.txt for ubuntu
    ofstream outputFile("output.txt");
    int currentYear = 1;
    int currentQuarter = startQuarter - 1;
    while (!availableClasses[0].empty() || !availableClasses[1].empty() || !availableClasses[2].empty()) {
        int titleColor[3] = {0, 0, 0};

        word(10, 10, "YEAR 1", titleColor);

        // Each time that the quarter is 3 (quarters are 0, 1, 2), another year has gone by.
        if (currentQuarter == 3) {
            currentYear++;
            word(((currentYear - 1) * 190) + 10, 10, "YEAR " + to_string(currentYear), titleColor);
        }
        currentQuarter = currentQuarter % 3;
        set<int> s = pickClasses(availableClasses[currentQuarter], maxCredits, currentYear, data, graph);

        outputFile << "Quarter " << currentQuarter + 1 << ":" << endl;

        int x = ((currentYear - 1) * 190) + (currentQuarter * 60) + ((startQuarter - 1) * -50) + 10;
        int y = 22;

        word(x, y - 6, "QUARTER " + to_string(currentQuarter + 1), titleColor);

        y -= 71;
        for (auto itr = s.begin(); itr !=s.end(); itr++) {
            outputFile << data[graph.nodeFromId(*itr)]->ToString() << endl;

            y += 71;
            vector<int> coords;
            coords.push_back(x);
            coords.push_back(y);
            data[graph.nodeFromId(*itr)]->SetCoords(coords);

            arr[x][y][0] = 255;
            arr[x][y][1] = 255;
            arr[x][y][2] = 255;

            int rgb[3] = {100, 100, 100};
            box(x, y, 40, 15, rgb);

            int rgb2[3] = {255,255,255};
            string code = data[graph.nodeFromId(*itr)]->GetCourseCode();
            char credits = char(data[graph.nodeFromId(*itr)]->GetCredits() + 48);
            word(x + 3, y + 2, code, rgb2);
            letter(x + 3, y + 8, credits, rgb2);

            availableClasses[0].erase(*itr);
            availableClasses[1].erase(*itr);
            availableClasses[2].erase(*itr);
            for (SmartDigraph::OutArcIt a(graph, graph.nodeFromId(*itr)); a != INVALID; ++a) {
                vector<int> v = data[graph.target(a)]->GetQuarters();
                for (int j = 0; j < v.size(); j++) {
                    availableClasses[v[j] - 1].insert(graph.id(graph.target(a)));
                }
            }
        }

        currentQuarter++;
    }
    outputFile.close();
}