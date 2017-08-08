//
//  main.cpp
//  FIFO
//
//  Created by Michael Josh Dangcil on 4/15/17.
//  Copyright © 2017 San Jose State University. All rights reserved.
//

#include <iostream>
#include <queue>
using namespace std;

const int maxSize = 26;
const int limit = 5;
int main() {
    //Alpabet array
    char alpha[maxSize] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                             'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    queue <char> myFIFO;
    
    //Initialize queue
    //Retain only the last five(5) elements of the array.
    cout << "FIFO will retain only the last five elements of the alphabet." << endl;
    
    for(int i = 0; i < maxSize; i++) {
        if(myFIFO.size() != limit) {
            myFIFO.push(alpha[i]);
        } else {
            myFIFO.pop();
            myFIFO.push(alpha[i]);
        }
    }
    
    for(int i = 0; i < limit; i++) {
        cout << myFIFO.front();
        myFIFO.pop();
        cout << endl;
    }
    
    return 0;
}
