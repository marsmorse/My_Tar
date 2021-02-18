# my_tar

Program similar to the tar system call that supports the use of the c, r, t, u, and x flags for creating, updating, and unpackaging archives.
### Introduction ###
Based on the input and results from the parseInput class the archive class my_tar will be able to create, extract, update, or append an archive. archive class uses
the header class to operate with archive headers. 
### Classes ###
#### Archive Class ####
Handles operations to create, extract, update, and append archives.
Input: path to a file or directory
Output: none
Memory: holds the file descriptor for the archive
#### ArchiveHeader Class ####
Class to handle getting archive header info, formatting it and unformatting it for creating and extracting archives.
Input: path to a file or directory
Output: none
Memory: posix_header strucute that contains inode info that takes up 500 bytes.
#### Xtractor Class ####
class to handle the creation of directories, files, and symbolic links
Input: archiveHeader object, archive file descriptor for writing content
Memory: none
### Notes ###
    When wrapping functions in a new class with a .h and .c file please add the new .h utility to the Makefile by
    1) Add a new variable with the filename of the header file
    ```
        FILENAME = filename
    ```
    2) Add code to compile the new class file into binary. like so:
    ```
        $(FILENAME).o: $(FILENAME).c $(FILENAME).h
            $(CC) $(CFLAGS) -c $(FILENAME).c
    ```
    3) Add the dependency to the main application target 
    ```
        $(TARGET): $(TARGET).o  $(FILENAME).o
            $(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o $(FILENAME).o

    ```
    to add tests just create a new run line in the test.rb script
### Testing ###
    To test my_tar cd into the test folder and enter 
    ```
        make testing
    ```
    to add tests just create a new run line in the test.rb script

    alternate testing is to manualy create tar files using my_tar and tar and check between both
### issues ###

Below is a list of the different issues with the code if you would like to work on 1 please put your name next to the issue in bold using "**" surround your name like by sandwiching your name between two astericks. When you finish simply delete the line.

#### general ####
* [Feature Request] Makefile [done]
* [Feature Request] Create a test suite for each type of expected command line input [might not be possible/ probably not worth it]
#### Archive Creation ####
* [Feature Request] Currently creating an archive where there are two members with the same name creates two archive entries that include content. the second entry isn't supposed to include content with duplicate file names
* [Feature Request] Update access times for files, links, and directories.
* [Feature Request] check for write access in home directory
#### Archive Extraction (-x flag) ####
* [Feature Request] Archive Extraction(-x flag)
* [Feature Request] need tests for Extraction
* [Feature Request] find out if archive is setting the correct file times
#### Archive Update (-u flag) ####
* [Feature Request] work with folders. currently if asked to update a folder it adds a new entry if newer but does not iterate the starting archive fd past the already updated items of the directory.
#### Archive Append (-a flag) ####
* [Feature Request] remove 1024 length zero buffer at the end of the archive before appending if it exists
#### input ####



