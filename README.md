# Colab
Collaborative Research Project


##**Running tests**

###**Installation**
I used this as reference
[getting started with google-test on ubuntu](https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/)

    sudo apt-get install libgtest-dev
    
    # now build the lib files we needv
    # install cmake if not already
    sudo apt-get install cmake 
    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make

    # copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder for ease
    sudo cp *.a /usr/lib

###**Running**
Running the tests should be as easy as the following:

    cd /test
    qmake
    make
    ./test