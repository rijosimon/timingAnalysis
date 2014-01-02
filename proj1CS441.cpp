//
//  main.cpp
//  proj1CS441
//
//  Created by Rijo Simon on 9/22/13.
//  Copyright (c) 2013 Rijo Simon. All rights reserved.
//

#include <iostream>
#include <time.h>

#define SIZE 497

int main()
{
    int prime = 0;
    static double a[SIZE][SIZE], b[SIZE][SIZE], c[SIZE][SIZE];
    clock_t totalDuration = 0;
    for ( long int i=0 ; i!=SIZE ; i++ ) {
        for ( long int k=0 ; k!=SIZE ; k++ ) {
            int sum = 0.0;
            clock_t start = clock();
            for ( long int j=0 ; j!=SIZE ; j++ )
                sum += a[i][j] * b[j][k];
            clock_t end = clock();
            if(prime==0) prime+=1;   //prime the pump
            else
                totalDuration+= (end-start);
            c[i][k] = sum;
        }
    }
    long double numOfMultiplications = SIZE;
    numOfMultiplications*=(SIZE-1);
    numOfMultiplications*=SIZE;
    std::cout<<"For Size: "<<SIZE<<"\n\n";
    std::cout<<"The total time took doing the multiplication loops: "<<(double)(totalDuration)/CLOCKS_PER_SEC<<" seconds\n\n";
    double timeInNanoAveMult =(((totalDuration)/CLOCKS_PER_SEC)*1e+9/numOfMultiplications);
    std::cout<<"The average time for each multiplication: "<<timeInNanoAveMult<<" nanoseconds\n\n";
    return 0;
}

