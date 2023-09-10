/** ****************************************************************
 *  Implementation of the SocialNetwork class                      *
 *  @author Brandon Dale                                           *
 *                                                                 *
 *  The primary use is to create a Social Network object given a   *
 *  JSON filename and to create all HTML Files specified           *
 *  in the project.                                                *
 *                                                                 *
 *  @file SocialNetwork.cpp                                        *
 *  @date September 9th, 2022                                      *
 ******************************************************************/


#include "SocialNetwork.h"
#include "User.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cassert>
#include <limits>
#include <algorithm>

using namespace std;


// -------------------------------------------------- CONSTRUCTORS -------------------------------------------------- //

SocialNetwork::SocialNetwork() {
    /*
     *  The default constructor of a SocialNetwork object. It initializes the number of users to 0.
     *  The vectors, followerRelationships, users, and userNames are initializes implicitly to be empty.
     *
     *  Parameters:
     *      Takes no parameters
     *
     *  Returns:
     *      No return value, creates a SocialNetwork object
     */
    numUsers = 0;
}

SocialNetwork::SocialNetwork(const string& JSON_Filename) {
    /*
     *  A constructor for a SocialNetwork object.
     *
     *  Initializes all private data members of a SocialNetwork object given the string filename of a JSON file containing
     *  an array of user information. This constructor parses through the JSON file extracting a strings which contain a
     *  single user's information as a JSON array. It then creates a string stream using that user information string and
     *  creates a User object with that string stream. Then, if needed, it sorts the user array by ID (smallest->biggest).
     *  Then it creates the 2D array of follower relationships and the 1D array of userNames.
     *
     *  Since this is not going to be widely used, the constructor currently makes the following assumptions:
     *      1) It is a valid JSON file
     *      2) Users will not have any attributes other than those listed above
     *      3) There will be at least one user in the given file
     *      4) There will not be any escaped characters within the user data strings
     *
     *  Parameters:
     *      const string& JSON_FILENAME:
     *          A string containing the filename of a JSON array containing user information. Assumes that the location of
     *          this file is in the correct place (For example: in the cmake-build-debug directory for CLion).
     *
     *  Returns:
     *      No return value as it creates a SocialNetwork object.
     */

    numUsers = 0;

    // ------------------ Create the array of users ------------------ //

    // Open the user input file, and assert that it opened successfully.
    ifstream inFS;
    inFS.open(JSON_Filename);
    assert(inFS.is_open());

    // Remove all characters before the start of the user array, marked by the first (and only) open bracket '[' and
    // remove all newline and tab characters before the start of a user object array (marked by '{')
    string userData;
    inFS.ignore(numeric_limits<streamsize>::max(), '[');

    // Create variables which track whether sorting of the Users array is needed. If the nextExpectedID does not match
    // the id of the next user created, then it flags that the user array will need to be sorted.
    bool userSortingNeeded = false;
    unsigned int nextExpectedID = 1;

    // While there is still a user data array in the filestream (i.e. still a user to create)
    while (inFS.peek() != ']') {

        // Remove anything up to (and including) the opening curly bracket
        inFS.ignore(numeric_limits<streamsize>::max(), '{');

        // Extract the userData and turn it into a string stream
        getline(inFS, userData, '}');
        stringstream userDataSS;
        userDataSS.str(userData);

        // Create a user object with that data, and insert it into the users array
        User newUser(userDataSS);
        assert(newUser.isValid());
        this->users.push_back(newUser);

        // Check if the Users array will need to be sorted
        if (newUser.getId() != nextExpectedID) userSortingNeeded = true;
        nextExpectedID++;

        // Clear the string stream, update the number of users and ignore all characters before the start of the next user array
        userDataSS.clear();
        numUsers++;
        inFS.ignore(); // remove either whitespace char or tab char
    }
    // Close the input filestream
    inFS.close();


    // ------------------ Sort the User Array ------------------ //
    if (userSortingNeeded) {
        sort(this->users.begin(), this->users.end());
    }


    // ------------------ Create the 2D follower relationship array and 1D array of names ------------------ //

    // Reserve the userNames vector so that resizing does not occur during its creation
    userNames.reserve(numUsers);

    // Creates a 2D array of booleans where a row represents a single user and a column represents which other users they follow
    for (int i = 0; i < numUsers; i++) {

        // Initialize a users row with all false and make a pointer to the current users
        vector<bool> row(numUsers, false);
        User* curUser = &users[i];

        // For each followed user, change each element with an index of (followedUserID - 1) in the relationship's matrix to true
        for (int j = 0; j < curUser->getFollowsSize(); j++) {
            unsigned int followerIndex = curUser->getFollowsIdAt(j) - 1;
            row[followerIndex] = true;
        }

        // Add the user follower row to the 2D array
        followerRelationShips.push_back(row);

        // Add the username to the usernames vector
        userNames.push_back(curUser->getName());
    }
}


// ------------------------------------------------ PUBLIC METHODS -------------------------------------------------- //

void SocialNetwork::createAllHTMLFiles() {
    /*
     *  Creates all the html files for the Social Network object (1 "index.html" file and N = numUsers "userN.html" files)
     *  Exports all files to the standard location for the compiler (ex: the cmake-build-debug directory in CLion).
     *
     *  Parameters:
     *      Takes no parameters.
     *
     *  Returns:
     *      Returns nothing.
     */

    // Check that there is at least one user
    if (this->numUsers < 1) {
        cerr << "CANNOT CREATE ALL SOCIAL NETWORK HTML FILES GIVEN ZERO USERS" << endl;
        exit(1);
    }


    // Create all the files
    this->createIndexHTMLFile();
    this->createAllUserHTMLPAGES();
}


// ------------------------------------------------ PRIVATE METHODS ------------------------------------------------- //

void SocialNetwork::getFollowerAndMutualsFromId(vector<unsigned int> &followers, vector<unsigned int> &mutuals,
                                                const unsigned int& currID) {
    /*
     *  Adds the ids of the users that are followers of and mutuals with a certain user (given by currID) to the pass
     *  by reference vectors
     *
     *  Parameters:
     *      vector<unsigned int>& followers:
     *          An empty vector to add the user id's of those that follow a specified user
     *
     *      vector<unsigned int>& mutuals:
     *          An empty vector to add the user id's of the users that are mutuals with a specified user
     *
     *      const unsigned int& currID:
     *          The id of the user who is being checked for mutual
     *
     *  Returns:
     *      Returns Nothing
     */

    for (unsigned int otherID = 1; otherID <= numUsers; otherID++) {

        if (currID != otherID && this->isFollowing(otherID, currID)) {
            followers.push_back(otherID);

            if (this->isFollowing(currID, otherID)) {
                mutuals.push_back(otherID);
            }
        }
    }

}

bool SocialNetwork::isFollowing(const unsigned int &followerID, const unsigned int &followedID) {
    /*
     *  Check if the user with the id "followerID" is following the user with the id "followedID"
     *
     *  Parameters:
     *      const unsigned int &followerID:
     *          Represents the id of the user which is being checked to see if they follow the user with followedID
     *
     *      const unsigned int &followedID:
     *          Represents the id of the user which is being checked to see if they are followed by the user with followerID
     *
     *  Returns:
     *      A boolean representing whether user "followerID" follows user "followedID"
     */

    // Make sure that both ID numbers are valid
    assert(followerID > 0 && followedID > 0);

    return this->followerRelationShips[followerID - 1][followedID - 1];
}

void SocialNetwork::createIndexHTMLFile() {
    /*
     *  Creates an index.html file for a social network object.
     *
     *  The created index.html file contains links to all user profile pages. This method assumes that all of these pages
     *  were created without error. However, an assertion is made that each user profile html file is made for each user.
     *  Thus, if no errors are raised, it can be assumed that all files were created successfully.
     *
     *  Parameters:
     *      Takes no parameters.
     *
     *  Returns:
     *      Returns nothing.
     */

    // Create the output file stream and make sure that it is open
    ofstream out;
    out.open("index.html");
    assert(out.is_open());

    // Add the universal html information to the file stream
    out << "<!DOCTYPE html>" << endl;
    out << "<html>" << endl;
    out << "<head>" << endl;
    out << "<title>My Social Network</title>" << endl;
    out << "</head>" << endl;
    out << "<body>" << endl;
    out << "<h1>My Social Network: User List</h1>" << endl;

    // Create an ordered list containing links to each user
    out << "<ol>" << endl;
    for (unsigned int currUserID = 1; currUserID <= numUsers; currUserID++) {
        out << R"(<li><a href="user)" << currUserID << R"(.html">)";
        out << userNames.at(currUserID - 1) << "</a></li>" << endl;
    }
    out << "</ol>" << endl;

    // Add the final closing tags for the html file
    out << "</body>" << endl;
    out << "</html>" << endl;

    // Close the file stream
    out.close();
}

void SocialNetwork::createAllUserHTMLPAGES() {
    /*
     *  Creates the user profile html file for each user in the Users array.
     *
     *  Parameters:
     *      Takes no parameters.
     *
     *  Returns:
     *      Returns nothing.
     */

    // Make sure that there is at least one user in the users array
    assert(!this->users.empty());

    for (unsigned int currID = 1; currID <= numUsers; currID++) {
        User& currUser = this->users.at(currID - 1);

        vector<unsigned int> followersIDs;
        vector<unsigned int> mutualsIDs;

        this->getFollowerAndMutualsFromId(followersIDs, mutualsIDs, currID);

        currUser.generateUserHTMLProfilePage(this->userNames, followersIDs, mutualsIDs);
    }
}
