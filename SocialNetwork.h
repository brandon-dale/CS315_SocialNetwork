/** *************************************************************
 *  Declaration of the SocialNetwork class                      *
 *  @author Brandon Dale                                        *
 *                                                              *
 *  Class represents a single social network consisting of      *
 *  multiple users. The main purpose is to read in a set of     *
 *  users, specified by a filename (in the constructor),        *
 *  and then create the matching series of HTML pages.          *
 *                                                              *
 *  @file SocialNetwork.h                                       *
 *  @date September 15th, 2022                                  *
 ***************************************************************/


#ifndef CS315_PROJECT01_SOCIALNETWORK_H
#define CS315_PROJECT01_SOCIALNETWORK_H

#include <vector>
#include "User.h"


class SocialNetwork {
public:
    // ------------------------------------------------ Constructors ------------------------------------------------ //
    // Default Constructor - Creates a social network object with 0 users
    SocialNetwork();

    // Creates a social network object given a "special" JSON file which
    // contains the information on the users of the network
    explicit SocialNetwork(const std::string& JSON_Filename);


    // ----------------------------------------------- Public Methods ----------------------------------------------- //
    // Creates all HTML files for the social network
    void createAllHTMLFiles();

private:
    // -------------------------------------------- Private Data Members -------------------------------------------- //
    unsigned int numUsers;
    std::vector<std::vector<bool>> followerRelationShips;
    std::vector<User> users;
    std::vector<std::string> userNames;

    // -----------------------------------------  Private Helper Functions ------------------------------------------ //
    // Adds the ids of the users that are followers of and mutuals with a certain user (given by currID) to the pass
    // by reference vectors
    void getFollowerAndMutualsFromId(std::vector<unsigned int>& followers, std::vector<unsigned int>& mutuals, const unsigned int& currID);

    // Check if the user with the id "followerID" is following the user with the id "followedID"
    bool isFollowing(const unsigned int& followerID, const unsigned int& otherUserID);

    // Creates an index.html file for a social network object.
    void createIndexHTMLFile();

    // Creates the user profile html file for each user in the Users array.
    void createAllUserHTMLPAGES();
};


#endif //CS315_PROJECT01_SOCIALNETWORK_H

