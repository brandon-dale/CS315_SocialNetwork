/** **********************************************************************
 *  Implementation of the User class                                     *
 *  @author Brandon Dale                                                 *
 *                                                                       *
 *  The primary use is to create a User given a string stream reference  *
 *  containing a JSON File array with the information for a SINGLE user  *
 *  (Everything between, but not including, the curly brackets).         *
 *  The other main use is to create the individual HTML file for         *
 *  the profile page of the user object.                                 *
 *                                                                       *
 *  @file User.cpp                                                       *
 *  @date September 15th, 2022                                           *
 ************************************************************************/


#include "User.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <utility>
#include <limits>

using namespace std;

// -------------------------------------------------- CONSTRUCTORS -------------------------------------------------- //
User::User() {
    /*  Default Constructor
     *  DO NOT USE WITH AN ACTUAL USER - Sets all values to empty strings
     *  Note: The vector<string> follows data member is not set to anything here
     *
     *  Parameters:
     *      Takes no parameters.
     *
     *  Returns:
     *      Returns nothing.
     *
    */

    id = 0;
    name = "";
    location = "";
    pic_url = "";
}


User::User(unsigned int id, string name, string location, string pic_url, vector<unsigned int> follows) {
    /*
     *  A constructor for a User object, given all the user data values.
     *
     *  Makes the following assumptions:
     *      NO OTHER USER HAS THIS SAME ID.
     *          No error checking is done to ensure that two users do not have the same id
     *      The user's name is a non-empty string
     *
     *  Parameters:
     *      unsigned int id:
     *          Represents a user's id number. ID MUST BE GREATER THAN 0 TO BE VALID
     *
     *      string name:
     *          Represents a user's name. No error checking is done to ensure that it is not empty
     *
     *      string location:
     *          Represents a user's location. Can be assigned to be empty
     *
     *      string pic_url:
     *          Represents a user's picture url. Can be assigned to be empty
     *          If this parameter is empty, it assigns it to be the default picture
     *
     *      vector<unsigned int> follows:
     *          Represents a 1D array of user IDs, represents the ids of the users that the current user follows
     *
     *  Returns:
     *      Returns nothing.
     *
     */

    this->id = id;
    this->name = move(name);
    this->location = move(location);
    this->pic_url = move(pic_url);
    this->follows = move(follows);

    // Set any default attributes if they are empty
    this->setDefaultAttributesWhenEmpty();
}


User::User(stringstream &ss) {
    /*
     *  Creates a user object given a pass-by-reference string stream of a JSON object array.
     *
     *  It makes the following assumptions:
     *      1) The file "scraping" started and stopped at the correct points
     *             - Meaning it took everything after (not including) the opening { and everything before (not including)
     *               the closing }
     *      2) The data contains at least the following data members, and that they are valid
     *             a) id_str
     *             b) name
     *      3) There are no unexpected data types, if there are, an assertion will fail in the setAttribute method.
     *
     *  Parameters:
     *      string stream& ss:
     *          A string stream containing all the user's data in the following pattern ("attributeTitle" : "attributeData")
     *          Is passed by reference only to avoid copying. The string stream is not used outside this constructor
     *
     *  Returns:
     *      Returns nothing.
     *
     */

    // While there are still attributes in the user JSON array
    // NOTE: There are 3 cases for what the peek in the while statement below will output
    //           1) The first item (which is guaranteed): peek will return a newline character
    //           2) The "middle" items:                  peek will return a comma
    //           3) The "last" item:                     peek will return a tab character

    while (ss.good() && ss.peek() != '\t') {
        // Get the attributes title and data and set it
        string title = getAttributeTitle(ss);
        string data = getAttributeData(ss);
        setAttribute(title, data);

        // Remove any whitespace before the next character. So peek can determine if user input is complete
        ss.ignore();
    }
    ss.clear();

    // Set any default attributes (currently only sets a default picture url)
    this->setDefaultAttributesWhenEmpty();
}

// ---------------------------------------------- OPERATOR OVERLOADING ---------------------------------------------- //

bool User::operator==(const User &other) const {
    /*
     *  Determines whether two users are equal to each other.
     *  Two users are determined to be equal to each other if all of their data members are equal to each other.
     *
     *  Notes:
     *      1) The current implementation does not consider if both users are "default users" - meaning that they were
     *          constructed with the default constructor.
     *
     *  Parameters:
     *      const User& other:
     *          A second user object that is being compared to the current one.
     *
     *  Returns:
     *      bool:
     *          Returns true if all data members are equal to each other
     *          Otherwise, returns false.
     *
     */

    if (this->id == other.id &&
        this->name == other.name &&
        this->location == other.location &&
        this->pic_url == other.pic_url &&
        this->follows == other.follows)
    {
        return true;
    }
    return false;
}

bool User::operator!=(const User &other) const {
    /*
     *  Returns whether two user objects are NOT equal to each other.
     *  Returns the negation of an equal's comparison.
     *
     *  Parameters:
     *      const User& other:
     *          A second user object that is being compared to the current one.
     *
     *  Returns:
     *      bool:
     *          Returns true if any number of data members are not equal to each other
     *          Otherwise, returns false.
     *
     */

    return !(*this == other);
}

bool User::operator< (const User &other) const {
    /*
     *  Returns whether one user is "less than" another user.
     *  A user is considered "less than" another user if their ID is less than the other ID.
     *
     *  Parameters:
     *      const User& other:
     *          A second user object that is being compared to the current one.
     *
     *  Returns:
     *      bool:
     *          Returns true if the id of the current user (this) is less than the id of the other user (other).
     *          Otherwise, returns false.
     *
     */

    return this->id < other.id;
}

bool User::operator<=(const User &other) const {
    /*
     *  Returns whether one user is "less than or equal to" another user.
     *  A user is considered "less than or equal to" another user if their ID is less than or equal to the other ID.
     *
     *  Parameters:
     *      const User& other:
     *          A second user object that is being compared to the current one.
     *
     *  Returns:
     *      bool:
     *          Returns true if the id of the current user (this) is less than or equal to the id of the other user (other).
     *          Otherwise, returns false.
     *
     */

    return this->id <= other.id;
}

bool User::operator>(const User &other) const {
    /*
     *  Returns whether one user is "greater than" another user.
     *  A user is considered "greater than" another user if their ID is greater than the other ID.
     *
     *  For simplicity, method returns the negation of the less than or equal to operation.
     *
     *  Parameters:
     *      const User& other:
     *          A second user object that is being compared to the current one.
     *
     *  Returns:
     *      bool:
     *          Returns true if the id of the current user (this) is greater than the id of the other user (other).
     *          Otherwise, returns false.
     *
     */

    return !(*this <= other);
}

bool User::operator>=(const User &other) const {
    /*
     *  Returns whether one user is "greater than or equal to" another user.
     *  A user is considered "greater than" another user if their ID is greater than or equal to the other ID.
     *
     *  For simplicity, method returns the negation of the less than operation.
     *
     *  Parameters:
     *      const User& other:
     *          A second user object that is being compared to the current one.
     *
     *  Returns:
     *      bool:
     *          Returns true if the id of the current user (this) is greater than or equal to the id of the other user (other).
     *          Otherwise, returns false.
     *
     */

    return !(*this < other);
}

std::ostream &operator<<(ostream &out, const User &user) {
    /*
     *  Puts all the User's private data members into an output string stream, so that a user can be "printed" to the console.
     *
     *  Parameters:
     *      ostream& out:
     *          an output string stream to put all the user's data into, in a formatted order.
     *
     *      const User& user:
     *          A user object which will have their data inserted into the "out" ostream
     *
     *  Returns:
     *      ostream&:
     *          Returns the "filled in" output stream with the user's information.
     *
     */

    // Output the users id and name
    out << "id: " << user.id << endl;
    out << "name: " << user.name << endl;

    // If the user has a non-empty location or pic_url, output them
    if (!user.location.empty()) out << "location: " << user.location << endl;
    if (!user.pic_url.empty())  out << "pic url: " << user.pic_url << endl;

    // Output the follows array. If the array is empty, it will appear as a pair of empty brackets
    out << "Follows: [ ";
    for (unsigned int follow : user.follows) {
        out << follow << " ";
    }
    out << "]" << endl;

    // Return the output stream
    return out;
}


// ------------------------------------------------ PUBLIC METHODS -------------------------------------------------- //

bool User::isValid() const {
    /*
     *  Returns whether a user object is valid or not.
     *  To be valid, a user must have an ID greater than 0 and a non-empty name
     *
     *  Parameters:
     *      Takes no parameters.
     *
     *  Returns:
     *      bool:
     *          Returns true if the user's id is greater than 0, and they have a non-empty name
     *
    */
    return this->id > 0 && !this->name.empty();
}

unsigned int User::getId() const {
    /*
     *  Returns the value of the user's id
     *  ASSERTS that the user is valid before returning anything
     *
     *  Parameters:
     *      No parameters.
     *
     *  Returns:
     *      unsigned int:
     *          Returns the ID of the current user object.
     *
     */
    assert(this->isValid());
    return this->id;
}

string User::getName() const {
    /*
     *  Returns the value of the user's name
     *  ASSERTS that the user is valid before returning anything
     *
     *  Parameters:
     *      No parameters.
     *
     *  Returns:
     *      string:
     *          Returns the name of the current user object.
     *
     */

    assert(this->isValid());
    return this->name;
}

string User::getLocation() const {
    /*
     *  Returns the value of the user's location
     *  ASSERTS that the user is valid before returning anything
     *
     *  If the user does not have a specified location, or it is set to be an empty string, it will return an empty string
     *
     *  Parameters:
     *      No parameters.
     *
     *  Returns:
     *      string:
     *          Returns the location of the current user object.
     *
     */

    assert(this->isValid());
    return this->location;
}

string User::getPicUrl() const {
    /*
     *  Returns the value of the user's pic_url
     *  ASSERTS that the user is valid before returning anything
     *
     *  If the user does not have a specified pic_url, or it is set to be an empty string, it will return an empty string
     *  Though, all user's should have a default pic_url.
     *
     *  Parameters:
     *      No parameters.
     *
     *  Returns:
     *      string:
     *          Returns the pic_url of the current user object.
     *
     */

    assert(this->isValid());
    return this->pic_url;
}

vector<unsigned int> User::getFollows() const {
    /*
     *  Returns a vector of unsigned ints which is a copy of the current user's follows vector
     *  ASSERTS that the user is valid before returning anything
     *
     *  Parameters:
     *      No parameters.
     *
     *  Returns:
     *      vector<unsigned int>:
     *          A copy of the user's follows vector.
     *
     */

    assert(this->isValid());
    return this->follows;
}

unsigned int User::getFollowsSize() const {
    /*
     *  Returns the number of users that the current user follows (the size of the follows vector)
     *  ASSERTS that the user is valid before returning anything
     *
     *  Parameters:
     *      No parameters.
     *
     *  Returns:
     *      unsigned int:
     *          The size of the follows vector
     *
     */

    assert(this->isValid());
    return this->follows.size();
}

unsigned int User::getFollowsIdAt(const int &i) const {
    /*
     *  Returns the ID number of the user that the current user follows at a certain ID INDEX
     *
     *  ASSERTS that the user is valid before returning anything
     *  While no direct error checking is done on the index parameter "i", the vector library should check if it is valid
     *  and throw an error if it is not valid.
     *
     *  Parameters:
     *      const unt& i:
     *          The index value of a user in the follows vector
     *
     *  Returns:
     *      unsigned int:
     *          The id of a user in the current user's follows vector
     *
     */

    assert(this->isValid());
    return this->follows.at(i);
}

void User::generateUserHTMLProfilePage(const vector<string> &userNames, const vector<unsigned int> &followersIDs, const vector<unsigned int> &mutualIds) {
    /*
     *  Generates the HTML user page file for the current user object.
     *
     *  Parameters:
     *      const vector<string>& userNames:
     *          Holds all users names in the social network object that this user belongs to.
     *          Used so that each user doesn't have access to every other user's information, on the name.
     *      const vector<unsigned int>& followersIDs:
     *          Holds all the user IDs for the users that follow the current user.
     *      const vector<unsigned int>& mutualsIds:
     *          Holds all the user IDs for the users that are mutuals with the current user.
     *
     *  Returns:
     *      Returns nothing.
     *
     */

    // Make sure that it is a valid user object
    assert(this->isValid());

    // Open the filestream
    string filename = "user" + to_string(id) + ".html";
    ofstream out;
    out.open(filename);
    assert(out.is_open());

    // Create the HTML Page header
    out << "<!DOCTYPE html>" << endl << "<html>" << endl << "<head>" << endl;
    out << "<title>" << name << " Profile</title>" << endl << "</head>" << endl;

    // Start the HTML Body
    out << "<body>" << endl;
    out << R"(<h2><a href="index.html">Social Network</a></h2>)" << endl;   // A button that links back to the index page for ease of testing

    // Insert the users name and location
    out << "<h1>" << name;
    if (!this->location.empty()) out << " in " << location;
    out << "</h1>" << endl;

    // Insert the users profile picture (if it is specified)
    if (!this->pic_url.empty())     out << R"(<img alt="Profile pic" src=")" << pic_url << R"(" />)" << endl;


    // Create the unordered lists for follows, followers, and mutual users.
    // ---------------- FOLLOWS ---------------- //
    addHTMLUnorderedUserList(out, userNames, this->follows, "Follows");

    // ---------------- FOLLOWERS ---------------- //
    addHTMLUnorderedUserList(out, userNames, followersIDs, "Followers");

    // ---------------- MUTUALS ---------------- //
    addHTMLUnorderedUserList(out, userNames, mutualIds, "Mutuals");


    // Add the closing tags and close the output file stream.
    out << "</body>" << endl << "</html>";
    out.close();
}


// -------------------------------------------- PRIVATE HELPER METHODS ---------------------------------------------- //

string User::getAttributeTitle(stringstream &inputLine) {
    /*
     *  Returns a string of an attributes title in a string stream inputLine.
     *
     *  Note: The inputLine does not only contain a single line. It is a reference to the json array string stream
     *        that contains all the user's data. Is named inputLine to make things easier to read
     *
     *  Assumes the following:
     *      1) The data in the line takes the following form
     *             "title" : "data"
     *      2) The first substring between the first pair of double quotation marks contains the title of an attribute
     *
     *  Parameters:
     *      string stream& inputLine:
     *          Contains a json array containing a user's data.
     *
     *  Returns:
     *      string title:
     *          Contains a non-escaped string with the title of an attribute.
     *
     */

    string title;

    // Remove all characters up to (and including) the first occurring double quotation mark
    inputLine.ignore(numeric_limits<streamsize>::max(), '"');

    // Save the substring up to (BUT NOT INCLUDING) the second occurring double quotation mark
    getline(inputLine, title, '"');

    return title;

}

std::string User::getAttributeData(stringstream &inputLine) {
    /*
     *  Extracts and returns the data string for a certain attribute, given a reference to the string stream containing
     *  the json array object with a single user's information
     *
     *  Notes:
     *      1) The inputLine does not only contain a single line. It is a reference to the json array string stream
     *         that contains all the user's data. Is named inputLine to make things easier to read
     *      2) If the data that is extracted is the follows vector, it will return a string of all id strings with the
     *         double quotes still included. These will be processed out during the setAttribute method.
     *
     *  Assumes the following:
     *      1) The data in the line takes the following form (not including parenthesis)
     *             ("title" : "data")
     *         Though, at the time that this method is run, it should be in this form (not including parenthesis)
     *             ( : "data" )
     *      2) The second substring between the second pair of double quotation marks contains the data of an attribute
     *
     *  Parameters:
     *      string stream& inputLine:
     *          Contains a json array containing a user's data.
     *
     *  Returns:
     *      string data:
     *          Holds the string value of a user's attribute data
     *
     */

    // Remove all leading characters between the attribute title and the start of the data string (including brackets or double quotes)
    inputLine.ignore(numeric_limits<streamsize>::max(), ':');
    inputLine.ignore();

    // Remove the space after the colon and determine what the delimiter will be for the data string (will EITHER be " or ])
    assert(inputLine.peek() == '"' || inputLine.peek() == '[');
    char delimiter = static_cast<char>(inputLine.get());
    if (delimiter == '[') delimiter = ']';

    // Extract the data up until the delimiter
    string data;
    getline(inputLine, data, delimiter);

    return data;
}

void User::setAttribute(const std::string& title, std::string data) {
    /*
     *  Sets the value of an attribute for the current user object, given the title of the attribute, and the string
     *  data that it will be set to.
     *
     *  Even though not all attributes of a user are of type string, the necessary conversions will be automatically handled
     *
     *  Parameters:
     *      const string& title:
     *          the title of the attribute to be set
     *      string data:
     *          the data string to be set
     *
     *  Returns:
     *      Returns nothing
     *
     */

    // Make sure that there is a non-empty title
    assert(!title.empty());

    // This must come before the second assert statement as the follows data string can be empty
    if (title == "follows") {
        this->follows = stringArrayToUnsignedIntVector(data);
        return;
    }

    // Make sure that the data string is not empty
    assert(!data.empty());

    // Depending on the title, set the corresponding attribute
    if (title == "id_str") {
        this->id = stoi(data);
        return;
    }
    if (title == "name") {
        this->name = data;
        return;
    }
    if (title == "pic_url") {
        this->pic_url = data;
        return;
    }
    if (title == "location") {
        this->location = data;
        return;
    }

    // If title does not match any of the options above, the title is invalid
    cerr << "COULD NOT SET ATTRIBUTE - " << title << " - IS NOT A RECOGNIZED USER ATTRIBUTE TYPE" << endl;
    exit(1);
}

vector<unsigned int> User::stringArrayToUnsignedIntVector(string &arrayString) {
    /*
     *  Converts a string of unsigned integer strings into a vector or unsigned integers.
     *
     *  Makes the following assumptions:
     *      1) Each sub string of integers are >= 0 and are less than the maximum value of an unsigned integer.
     *         Though if a value is 0, it is an invalid user id.
     *      2) Each sub string only contains digits.
     *
     *  NOTES:
     *      1) The current implementation does not set the size of the vector before populating it, so a great deal of
     *         resizing is occurring. It may be faster to preset the size of the vector to increase performance.
     *      2) The current implementation uses the stoi method. This may cause a narrowing issue as all IDs (which are
     *         being converted here) are unsigned ints.
     *
     *  Parameters:
     *      string& arrayString:
     *          A string containing non-escaped integers in the following form (not including the parenthesis):
     *              ("1","2","3"...,"n")
     *
     *  Returns:
     *      vector<unsigned int> arr:
     *          A vector of user IDs
     *
     */

    vector<unsigned int> arr;

    // Create a string stream to process to array string
    stringstream arraySS;
    arraySS.str(arrayString);

    string val_str;
    while (arraySS.peek() == ',' || arraySS.peek() == '"') {

        arraySS.ignore(numeric_limits<streamsize>::max(), '"');
        getline(arraySS, val_str, '"'); // Extract the number from the SS
        arr.push_back(stoi(val_str)); // Push the integer into the array      NOTE: possible narrowing of conversion

    }

    return arr;
}

void User::addHTMLUnorderedUserList(ofstream& out, const vector<string> &userNames, const vector<unsigned int> &otherIDsList, const string &listTitle) {
    /*
     *  Adds an unordered list of links to users specified by otherIDsList to the output file stream out.
     *
     *  Makes the following assumptions:
     *      1) The listTitle is valid (is "Follows", "Followers", or "Mutuals")
     *         Since all uses of this method are hardcoded and this is a private method, this should not present any issues.
     *
     *  Parameters:
     *      ofstream& out:
     *          an output filestream for the User's HTML user page.
     *
     *      const vector<string>& userNames:
     *          A vector containing all the names of all users in the Social Network object that the current user belongs to.
     *
     *      const vector<unsigned int>& otherIDsList:
     *          A vector containing users IDs, these are printed as links to that users HTML page.
     *
     *      const string& listTitle:
     *          The title of the unordered list to be created.
     *
     *  Returns:
     *      Returns nothing.
     *
     */

    // Make sure that the output file stream is still open
    assert(out.is_open());

    // Insert the title of the unordered list
    out << "<h2>" << listTitle  << "</h2>" << endl;

    // If the list of IDs to use is NOT empty, create the unordered list
    if (!otherIDsList.empty()) {
        out << "<ul>" << endl;

        // For each user specified in otherIDsList, create a list element with a link to their profile page
        for (const unsigned int& otherID : otherIDsList) {
            const string& otherUserName = userNames.at(otherID - 1);
            out << R"(<li><a href="user)" << otherID << R"(.html">)";
            out << otherUserName << "</a></li>" << endl;
        }

        out << "</ul>" << endl;
    }


    // Otherwise, create a note that it is empty
    else {
        out << "<p>None</p>" << endl;
    }
}

void User::setDefaultAttributesWhenEmpty() {
    /*
     *  Sets any private data members that have a specified default value to that default value if that data member is the
     *  not-specified.
     *
     *  Currently, this only applies to a User's pic_url.
     *
     *  Parameters:
     *      No parameters.
     *
     *  Returns:
     *      Returns nothing.
     *
     */

    if (this->pic_url.empty()) {
        this->pic_url = R"(https://i.pinimg.com/236x/1c/8b/b2/1c8bb212c3fac9c3393b663c0ed9f6cb.jpg)";
    }
}


