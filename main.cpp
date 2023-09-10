/** *************************************************
 *  SSU - CS 315 - Project 01                       *
 *  @author Brandon Dale                            *
 *                                                  *
 *  Main driver code for the project.               *
 *  Reads in a filename as a command-line argument  *
 *  and creates all of the relevant HTML files.     *
 *                                                  *
 * @file main.cpp                                   *
 * @date September 15th, 2022                       *
 ***************************************************/


#include "SocialNetwork.h"
#include <string>
#include <cassert>
#include <iostream>

using namespace std;


/** ****************************************************
 *  MAIN DRIVER CODE - Handles reading in command-line *
 *  arguments and then uses the Social Network class   *
 *  to create all of the relevant HTML files.          *
 ******************************************************/
int main(int argc, char* argv[]) {

    // Validate and get filename from arguments
    if (argc < 2) {
       cerr << "ERROR -- INVALID NUMBER OF ARGUMENTS PROVIDED -- TERMINATING\n";
       exit(1);
    }
    string input_filename = argv[1];

    // Create the social network
    SocialNetwork sn(input_filename);

    // Create all HTML Files for the network
    sn.createAllHTMLFiles();

    return 0;
}

