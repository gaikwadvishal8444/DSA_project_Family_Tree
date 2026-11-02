/*
* ============================================================================
* FAMILY TREE MANAGEMENT SYSTEM
* Course : Data Structures and Algorithms (R25-EC-PCC-202)
* Project : CCE-III Course Project, Unit V - Trees and Graphs
* Language : C++ (no STL containers are used)
*
* Data structure : General (n-ary) tree.
* Each family member is a node. Children of a node are stored as a
* linked list using 'firstChild' and 'nextSibling' pointers, and every
* node keeps a 'parent' pointer so that ancestors can be found quickly.
* ============================================================================
*/
#include <iostream>
#include <cstring>
#include <cstdlib>
using namespace std;
const int MAX_NAME_LEN = 50; // maximum length of a member's name
const int MAX_MEMBERS = 500; // maximum number of family members
const int MAX_DEPTH = 100; // maximum number of generations in a path
// ----------------------------------------------------------------------------
// Class Person : one node of the family tree
// ----------------------------------------------------------------------------
class Person {
public:
char name[MAX_NAME_LEN];
char gender; // 'M' or 'F'
int birthYear;
Person* parent; // link to parent (NULL for the root ancestor)
Person* firstChild; // link to the first child
Person* nextSibling; // link to the next child of the same parent
// Constructor: creates a member with no relatives linked yet
Person(const char* n, char g, int year) {
strncpy(name, n, MAX_NAME_LEN - 1);
name[MAX_NAME_LEN - 1] = '\0';
gender = g;
birthYear = year;
parent = NULL;
firstChild = NULL;
nextSibling = NULL;
}
// Operator overloading: prints a member's details using <<
friend ostream& operator<<(ostream& os, const Person& p) {
os << p.name << " (" << p.gender << ", born " << p.birthYear << ")";
return os;
}
};
// ----------------------------------------------------------------------------
// Class FamilyTree : manages the whole tree and all operations on it
// ----------------------------------------------------------------------------
class FamilyTree {
private:
Person* root; // earliest known ancestor
int count; // total number of members in the tree
// DFS (recursive) search for a member by name inside the subtree of 'node'
Person* searchRec(Person* node, const char* name) const {
if (node == NULL) return NULL;
if (strcmp(node->name, name) == 0) return node;
for (Person* c = node->firstChild; c != NULL; c = c->nextSibling) {
Person* found = searchRec(c, name);
if (found != NULL) return found;
}
return NULL;
}
// Preorder traversal that prints the subtree with indentation

void displayRec(const Person* node, int depth) const {
if (node == NULL) return;
for (int i = 0; i < depth; i++) cout << "    ";
if (depth > 0) cout << "|-- ";
cout << *node << "\n";
for (Person* c = node->firstChild; c != NULL; c = c->nextSibling)
displayRec(c, depth + 1);
}
// Counts the members in the subtree rooted at 'node'
int countRec(const Person* node) const {
if (node == NULL) return 0;
int total = 1;
for (Person* c = node->firstChild; c != NULL; c = c->nextSibling)
total += countRec(c);
return total;
}
// Height of the subtree (number of generations)
int heightRec(const Person* node) const {
if (node == NULL) return 0;
int best = 0;
for (Person* c = node->firstChild; c != NULL; c = c->nextSibling) {
int h = heightRec(c);
if (h > best) best = h;
}
return best + 1;
}
// Frees every node of a subtree (post-order: children first, then node)
void freeSubtree(Person* node) {
if (node == NULL) return;
Person* c = node->firstChild;
while (c != NULL) {
Person* next = c->nextSibling;
freeSubtree(c);
c = next;
}
delete node;
}
// Detaches 'node' from its parent's child list (does not delete it)
void unlinkFromParent(Person* node) {
Person* p = node->parent;
if (p == NULL) return;
if (p->firstChild == node) {
p->firstChild = node->nextSibling;
} else {
Person* prev = p->firstChild;
while (prev != NULL && prev->nextSibling != node)
prev = prev->nextSibling;
if (prev != NULL) prev->nextSibling = node->nextSibling;
}
node->nextSibling = NULL;
node->parent = NULL;
}
// Lowest Common Ancestor (LCA): first node of 'b's upward path that also
// lies on 'a's upward path (a node counts as its own ancestor)
Person* lowestCommonAncestor(Person* a, Person* b) const {
Person* pathA[MAX_DEPTH];
int lenA = 0;
for (Person* p = a; p != NULL && lenA < MAX_DEPTH; p = p->parent)
pathA[lenA++] = p;
for (Person* q = b; q != NULL; q = q->parent)
for (int i = 0; i < lenA; i++)
if (pathA[i] == q) return q;
return NULL;
}
// Number of parent-steps needed to climb from 'node' up to 'anc'
int stepsUp(const Person* node, const Person* anc) const {
int steps = 0;
for (const Person* p = node; p != NULL; p = p->parent) {
if (p == anc) return steps;
steps++;
}
return -1; // not an ancestor

}
// Converts 1 -> "first", 2 -> "second", ... for cousin names
const char* ordinalName(int n) const {
static const char* names[] = {"zeroth", "first", "second", "third",
"fourth", "fifth", "sixth", "seventh",
"eighth", "ninth"};
if (n >= 0 && n <= 9) return names[n];
return "higher-degree";
}
public:
// Constructor: an empty tree
FamilyTree() : root(NULL), count(0) {}
// Destructor: frees all members
~FamilyTree() { freeSubtree(root); }
bool isEmpty() const { return root == NULL; }
int size() const { return count; }
// Creates the root (earliest known ancestor). Only allowed on empty tree.
bool setRoot(const char* name, char gender, int year) {
if (root != NULL) return false;
root = new Person(name, gender, year);
count = 1;
return true;
}
// Returns the member with the given name, or NULL if not found
Person* findMember(const char* name) const {
return searchRec(root, name);
}
// Adds a child under the named parent (names are assumed unique)
bool addChild(const char* parentName, const char* childName,
char gender, int year) {
if (count >= MAX_MEMBERS) return false;
if (findMember(childName) != NULL) return false; // duplicate name
Person* p = findMember(parentName);
if (p == NULL) return false; // parent missing
Person* child = new Person(childName, gender, year);
child->parent = p;
// Append at the end of the child list so children stay in birth order
if (p->firstChild == NULL) {
p->firstChild = child;
} else {
Person* last = p->firstChild;
while (last->nextSibling != NULL) last = last->nextSibling;
last->nextSibling = child;
}
count++;
return true;
}
// Prints the whole tree (preorder DFS traversal)
void displayTree() const {
if (root == NULL) {
cout << "The family tree is empty.\n";
return;
}
cout << "\n--- Family Tree ---\n";
displayRec(root, 0);
}
// Prints details of one member and the name of the parent
void searchMember(const char* name) const {
Person* p = findMember(name);
if (p == NULL) {
cout << "Member \"" << name << "\" not found.\n";
return;
}
cout << "Found: " << *p << "\n";
cout << "Parent: " << (p->parent ? p->parent->name : "(none - root ancestor)")
<< "\n";
}

// Walks up the parent pointers and lists all ancestors
void showAncestors(const char* name) const {
Person* p = findMember(name);
if (p == NULL) {
cout << "Member \"" << name << "\" not found.\n";
return;
}
cout << "\nAncestors of " << p->name << ":\n";
if (p->parent == NULL) {
cout << " (none - this is the root ancestor)\n";
return;
}
int level = 1;
for (Person* a = p->parent; a != NULL; a = a->parent) {
cout << " " << level << " generation(s) up : " << a->name << "\n";
level++;
}
}
// Lists the children and siblings of a member
void showChildrenAndSiblings(const char* name) const {
Person* p = findMember(name);
if (p == NULL) {
cout << "Member \"" << name << "\" not found.\n";
return;
}
cout << "\nChildren of " << p->name << ":\n";
if (p->firstChild == NULL) cout << " (none)\n";
for (Person* c = p->firstChild; c != NULL; c = c->nextSibling)
cout << " " << c->name << "\n";
cout << "Siblings of " << p->name << ":\n";
bool found = false;
if (p->parent != NULL) {
for (Person* s = p->parent->firstChild; s != NULL; s = s->nextSibling) {
if (s != p) {
cout << " " << s->name << "\n";
found = true;
}
}
}
if (!found) cout << " (none)\n";
}
// Level-order (BFS) traversal: prints the family generation by generation
void showGenerations() const {
if (root == NULL) {
cout << "The family tree is empty.\n";
return;
}
Person* queue[MAX_MEMBERS]; // array-based queue (no STL)
int head = 0;
int tail = 0;
queue[tail++] = root;
int generation = 1;
while (head < tail) {
int levelSize = tail - head; // members in the current generation
cout << "Generation " << generation << ":";
for (int i = 0; i < levelSize; i++) {
Person* cur = queue[head++];
cout << " " << cur->name;
for (Person* c = cur->firstChild; c != NULL; c = c->nextSibling)
queue[tail++] = c; // enqueue children for next generation
}
cout << "\n";
generation++;
}
}
// Explains how two members are related using their common ancestor
void showRelationship(const char* nameA, const char* nameB) const {
Person* a = findMember(nameA);
Person* b = findMember(nameB);
if (a == NULL || b == NULL) {
cout << "One or both members were not found.\n";
return;
}

if (a == b) {
cout << a->name << " and " << b->name << " are the same person.\n";
return;
}
Person* lca = lowestCommonAncestor(a, b);
if (lca == NULL) {
cout << "No common ancestor found.\n";
return;
}
int da = stepsUp(a, lca); // generations from a up to the common ancestor
int db = stepsUp(b, lca); // generations from b up to the common ancestor
cout << "\nRelationship between " << a->name << " and " << b->name << ":\n";
if (da == 0) {
cout << " " << a->name << " is the ancestor of " << b->name
<< " (" << db << " generation(s) below).\n";
} else if (db == 0) {
cout << " " << b->name << " is the ancestor of " << a->name
<< " (" << da << " generation(s) below).\n";
} else if (da == 1 && db == 1) {
cout << " " << a->name << " and " << b->name << " are siblings.\n";
} else {
int minD = (da < db) ? da : db;
int diff = (da > db) ? da - db : db - da;
if (minD == 1) {
// One person is a sibling of the other's parent: uncle/aunt type
Person* uncle = (da < db) ? a : b;
Person* niece = (da < db) ? b : a;
cout << " " << uncle->name << " is the ";
for (int i = 0; i < diff - 1; i++) cout << "great-";
cout << (uncle->gender == 'F' ? "aunt" : "uncle")
<< " of " << niece->name << ".\n";
} else {
// Cousins: degree = minimum distance - 1, removed = difference
cout << " " << a->name << " and " << b->name << " are "
<< ordinalName(minD - 1) << " cousins";
if (diff > 0) cout << ", " << diff << " time(s) removed";
cout << ".\n";
}
}
cout << " Common ancestor: " << lca->name << "\n";
}
// Removes a member together with all of their descendants
bool removeMember(const char* name) {
Person* p = findMember(name);
if (p == NULL) return false;
if (p == root) { // removing the root deletes the whole tree
freeSubtree(root);
root = NULL;
count = 0;
return true;
}
int removed = countRec(p);
unlinkFromParent(p);
freeSubtree(p);
count -= removed;
return true;
}
// Shows simple statistics about the tree
void showStatistics() const {
cout << "\nTotal family members : " << count << "\n";
cout << "Generations (height) : " << heightRec(root) << "\n";
}
};
// ----------------------------------------------------------------------------
// Input helper functions
// ----------------------------------------------------------------------------
// Reads a non-empty name (names may contain spaces, so getline is used)
void readName(char* buffer, int size, const char* prompt) {
while (true) {

cout << prompt;
cin.getline(buffer, size);
if (buffer[0] != '\0') return;
cout << " Name cannot be empty. Try again.\n";
}
}
// Reads 'M' or 'F'
char readGender() {
char line[10];
while (true) {
cout << " Gender (M/F): ";
cin.getline(line, 10);
if (line[0] == 'M' || line[0] == 'm') return 'M';
if (line[0] == 'F' || line[0] == 'f') return 'F';
cout << " Please enter M or F.\n";
}
}
// Reads a birth year between 1800 and 2026
int readYear() {
char line[20];
while (true) {
cout << " Birth year: ";
cin.getline(line, 20);
int year = atoi(line);
if (year >= 1800 && year <= 2026) return year;
cout << " Enter a year between 1800 and 2026.\n";
}
}
// Converts a menu entry to a number; returns -1 if it is not a whole number
int parseChoice(const char* s) {
if (s[0] == '\0') return -1;
for (int i = 0; s[i] != '\0'; i++)
if (s[i] < '0' || s[i] > '9') return -1;
return atoi(s);
}
// ----------------------------------------------------------------------------
// main : interactive menu
// ----------------------------------------------------------------------------
int main() {
FamilyTree tree;
char choice[10];
char nameA[MAX_NAME_LEN];
char nameB[MAX_NAME_LEN];
char nameC[MAX_NAME_LEN];
bool running = true;
cout << "=============================================\n";
cout << " FAMILY TREE MANAGEMENT SYSTEM\n";
cout << "=============================================\n";
while (running) {
cout << "\n1. Create root ancestor\n"
<< "2. Add child\n"
<< "3. Display family tree\n"
<< "4. Search member\n"
<< "5. Show ancestors\n"
<< "6. Show children and siblings\n"
<< "7. Show generations (BFS)\n"
<< "8. Find relationship between two members\n"
<< "9. Remove member (with descendants)\n"
<< "10. Show statistics\n"
<< "0. Exit\n"
<< "Enter choice: ";
cin.getline(choice, 10);
switch (parseChoice(choice)) {
case 1: {
readName(nameA, MAX_NAME_LEN, "Enter name of root ancestor: ");
char g = readGender();
int y = readYear();
if (tree.setRoot(nameA, g, y))
cout << "Root ancestor added.\n";
else
cout << "The tree already has a root ancestor.\n";

break;
}
case 2: {
if (tree.isEmpty()) {
cout << "Create the root ancestor first (option 1).\n";
break;
}
readName(nameA, MAX_NAME_LEN, "Enter parent's name: ");
if (tree.findMember(nameA) == NULL) {
cout << "Parent \"" << nameA << "\" not found.\n";
break;
}
readName(nameB, MAX_NAME_LEN, "Enter child's name: ");
if (tree.findMember(nameB) != NULL) {
cout << "A member named \"" << nameB << "\" already exists.\n";
break;
}
char g = readGender();
int y = readYear();
if (tree.addChild(nameA, nameB, g, y))
cout << nameB << " added as child of " << nameA << ".\n";
else
cout << "Could not add the member (tree may be full).\n";
break;
}
case 3:
tree.displayTree();
break;
case 4:
readName(nameA, MAX_NAME_LEN, "Enter name to search: ");
tree.searchMember(nameA);
break;
case 5:
readName(nameA, MAX_NAME_LEN, "Enter member's name: ");
tree.showAncestors(nameA);
break;
case 6:
readName(nameA, MAX_NAME_LEN, "Enter member's name: ");
tree.showChildrenAndSiblings(nameA);
break;
case 7:
tree.showGenerations();
break;
case 8:
readName(nameA, MAX_NAME_LEN, "Enter first member's name: ");
readName(nameB, MAX_NAME_LEN, "Enter second member's name: ");
tree.showRelationship(nameA, nameB);
break;
case 9:
readName(nameC, MAX_NAME_LEN, "Enter name of member to remove: ");
if (tree.removeMember(nameC))
cout << nameC << " and their descendants were removed.\n";
else
cout << "Member \"" << nameC << "\" not found.\n";
break;
case 10:
tree.showStatistics();
break;
case 0:
running = false;
cout << "Exiting. Goodbye!\n";
break;
default:
cout << "Invalid choice. Please try again.\n";
}
}
return 0;
}
