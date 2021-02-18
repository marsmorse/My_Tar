#!/bin/bash
$program_name = "my_tar"
#prints if test succeed or failed. if failed prints the difference between outp and expected output
#input
# $1 = test number
# $2 = test letter
passFail() {
        cmp -l $1 $2 > "test_results$3$4"
        status=$?
        if [[ $status = 0 ]]; then
            printf "\033[1;32m"; #green
            printf "\nTest %c%c\n" "$3" "$4"
            printf "PASSED\n";
            printf "\033[0m"; 
            
        else
            printf "\033[0;31m";
            printf "\nTest %c%c\n" "$3" "$4"
            printf "FAILED\n";
            printf "\033[0m"; 
            diff -c $1 $2
        fi
        return 1;
}
updatetest()
{
    echo "*****************************"
    echo 'TESTING update'
    echo "*****************************"
    touch tfile
    touch tfile2
    cat testfile1 > tfile
    cat "some text" > tfile
    tar -cf u.tar tfile tfile2
    tar -cf myu.tar tfile tfile2
    cat testfile3 > tfile
    cat "some different text" > tfile2
    tar -uf u.tar tfile tfile2
    ./my_tar -uf myu.tar tfile tfile2
    passFail u.tar myu.tar
    rm tfile
    rm tfile2
    
}
createtest() {
    printf "\n";
    echo "*****************************"
    echo 'TESTING $program_name'
    echo "*****************************"
    echo "testing archive creation on regular files"
    echo "------------------------------------------"
    test '1' 'a' '-cf' 'testfile1'
    test '2' 'c' "-cf" "testfile1 testfile2"
    echo "testing archive creation with symbolic links"
    echo "------------------------------------------"
    test '2' 'a' "-cf" "symblink"
    echo "testing archive creation with directories"
    echo "------------------------------------------"
    tar -xf dir.tar
    test '3' 'a' '-cf' 'j'
    echo "testing archive creation with a combination of the above"
    echo "------------------------------------------"
    test '3' 'a' '-cf' 'testfile1 symblink testfile2'
    test '3' 'b' "-cf" 'j testfile1'
    test '3' 'c' "-cf" 'j symblink testfile2 testfile2'
    #test '3' 'd' "-cf" "testfile1 testfile2 j symblink"
}
#input
# $1 = test number
# $2 = test letter
# $3 = test input
test() {
        printf "\nTest %c%c\n" "$1" "$2"
        printf "args: '%s' '%s'\n" "$3" "$4";
        if [[ $3 == "" ]]; then
            printf "empty flag input %s\n" "$3";
        else
            tar $3 tar_result.tar $4
            ./my_tar $3 my_tar_result.tar $4
            passFail tar_result.tar my_tar_result.tar
        fi
        return 1;
}

#createtest
updatetest
#rmdir j
