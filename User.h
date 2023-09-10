/** ********************************************************
 *  Implementation of the User class                       *
 *  @author Brandon Dale                                   *
 *                                                         *
 *  Contains the information for a single user within the  *
 *  Social Network.                                        *
 *                                                         *
 *  @file User.h                                           *
 *  @date September 15th, 2022                             *
 **********************************************************/


#ifndef CS315_PROJECT01_USER_H
#define CS315_PROJECT01_USER_H

#include <string>
#include <vector>
#include <sstream>
#include <fstream>


class User {
public:
    // ------------------------------------------------ Constructors ------------------------------------------------ //
    // Default Constructor, Sets all values to Empty or 0
    User();

    // Sets all values to the parameter values
    User(unsigned int id, std::string name, std::string location, std::string pic_url, std::vector<unsigned int> follows);

    // MAIN CONSTRUCTOR - Creates a user object given a pass-by-reference string stream of a JSON object array.
    explicit User(std::stringstream& ss);

    // -------------------------------------------- Operator Overloading -------------------------------------------- //
    bool operator ==(const User& other) const;
    bool operator !=(const User& other) const;
    bool operator < (const User& other) const;
    bool operator <=(const User& other) const;
    bool operator > (const User& other) const;
    bool operator >=(const User& other) const;
    friend std::ostream& operator<< (std::ostream& out, const User& user);

    // ----------------------------------------------- Public Methods ----------------------------------------------- //
    // Checks if user is valid -- To be valid, a user must have an ID greater than 0 and a non-empty name
    bool isValid() const;

    // Returns the ID of the user
    unsigned int getId() const;

    // Returns the name of the user
    std::string getName() const;

    // Returns the location of the user
    std::string getLocation() const;

    // Returns the picture url of the user
    std::string getPicUrl() const;

    // Returns a vector of unsigned ints which is a copy of the current user's follows vector
    std::vector<unsigned int> getFollows() const;

    // Returns the number of users that the current user follows (the size of the follows vector)
    unsigned int getFollowsSize() const;

    //Returns the ID number of the user that the current user follows at a certain ID INDEX
    unsigned int getFollowsIdAt(const int& i) const;

    // Generates the HTML user page file for the current user object.
    void generateUserHTMLProfilePage(const std::vector<std::string>& userNames = {},
                                    const std::vector<unsigned int>& followersIDs = {},
                                     const std::vector<unsigned int>& mutualIds = {});


private:
    // -------------------------------------------- Private Data Members -------------------------------------------- //
    unsigned int id;
    std::string name;
    std::string location;
    std::string pic_url;
    std::vector<unsigned int> follows;

    // -----------------------------------------  Private Helper Functions ------------------------------------------ //
    // Returns a string of an attributes title in a string stream inputLine.
    static std::string getAttributeTitle(std::stringstream& inputLine);

    // Extracts and returns the data string for a certain attribute, given a reference to the string stream containing
    // the json array object with a single user's information
    static std::string getAttributeData(std::stringstream& inputLine);

    // Sets the value of an attribute for the current user object, given the title of the attribute, and the string
    // data that it will be set to.
    void setAttribute(const std::string& title, std::string data);

    // Converts a string of unsigned integer strings into a vector or unsigned integers.
    static std::vector<unsigned int> stringArrayToUnsignedIntVector(std::string& stringArray);

    // Adds an unordered list of links to users specified by otherIDsList to the output file stream out.
    static void addHTMLUnorderedUserList(std::ofstream& out, const std::vector<std::string>& userNames,
                                  const std::vector<unsigned int>& otherIDsList,const std::string& listTitle);

    // Sets any private data members that have a specified default value to that default value if that data member is the
    // not-specified.
    void setDefaultAttributesWhenEmpty();
};


#endif //CS315_PROJECT01_USER_H

