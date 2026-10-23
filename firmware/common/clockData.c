#include <stdint.h>
#include "clockData.h"

static const uint8_t slave_numbering[30][4] = 
{{0, 15, 30, 45}, {1, 16, 31, 46}, {2, 17, 32, 47}, {3, 18, 33, 48}, {4, 19, 34, 49}, 
{5, 20, 35, 50}, {6, 21, 36, 51}, {7, 22, 37, 52}, {8, 23, 38, 53}, {9, 24, 39, 54}, 
{10, 25, 40, 55}, {11, 26, 41, 56}, {12, 27, 42, 57}, {13, 28, 43, 58}, {14, 29, 44, 59}, 
{60, 75, 90, 105}, {61, 76, 91, 106}, {62, 77, 92, 107}, {63, 78, 93, 108}, {64, 79, 94, 109}, 
{65, 80, 95, 110}, {66, 81, 96, 111}, {67, 82, 97, 112}, {68, 83, 98, 113}, {69, 84, 99, 114}, 
{70, 85, 100, 115}, {71, 86, 101, 116}, {72, 87, 102, 117}, {73, 88, 103, 118}, {74, 89, 104, 119}};

// Number zero representation                           
static const digitData ZERO[2] =
{
    { // minute
        // left
         {timeAngle[3], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[12], 
            timeAngle[6], timeAngle[12], timeAngle[3]},
        // right
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]}
                                    
    },
    { // hour
        // left side of digit
        {timeAngle[6], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[3]},
        // middle
        {timeAngle[9], timeAngle[6], timeAngle[12], 
            timeAngle[6],timeAngle[12], timeAngle[9]},
        //right
        {timeAngle[9], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[9]}
    } 
                                
};
          
// Number one representation          
static const digitData ONE[2] = 
{
    { // minute
        // left
        {45, 45, 45, 45, 45, 45},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[3]},
        // right
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]}
                                    
    },
    { // hour
        // left side of digit
        {45, 45, 45, 45, 45, 45},
        // middle
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12],timeAngle[12], timeAngle[12]},
        //right
        {timeAngle[9], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[9]}
    }   

                                
};

// Number two representation
static const digitData TWO[2] = 
{
    { // minute
        // left
        {timeAngle[6], timeAngle[12], timeAngle[3], 
            timeAngle[12], timeAngle[12], timeAngle[12]},
        // middle
        {timeAngle[9], timeAngle[9], timeAngle[12], 
            timeAngle[3], timeAngle[12], timeAngle[9]},
        // right
        {timeAngle[9], timeAngle[6], timeAngle[6], 
            timeAngle[9], timeAngle[6], timeAngle[9]}
                                    
    },
    { // hour
        // left side of digit
        {timeAngle[3], timeAngle[3], timeAngle[6], 
            timeAngle[6],timeAngle[6], timeAngle[3]},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[9], 
            timeAngle[6],timeAngle[3], timeAngle[3]},
        //right
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[9], timeAngle[12]}
    }

                                
};
        
// Number three representation
static const digitData THREE[2] = 
{
    { // minute
        // left
        {timeAngle[6], timeAngle[12], timeAngle[3], 
            timeAngle[12], timeAngle[12], timeAngle[12]},
        // middle
        {timeAngle[9], timeAngle[9], timeAngle[12], 
            timeAngle[9], timeAngle[12], timeAngle[9]},
        // right
        {timeAngle[9], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[9]}
                                    
    },
    { // hour
        // left side of digit
        {timeAngle[3], timeAngle[3], timeAngle[6], 
            timeAngle[3],timeAngle[6], timeAngle[3]},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[9], 
            timeAngle[6],timeAngle[9], timeAngle[3]},
        //right
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]}
    }
                          
};     
  
// Number four representation  
static const digitData FOUR[2] = 
{
    { // minute
        // left
        {timeAngle[3], timeAngle[12], timeAngle[12], 
            timeAngle[12], 45, 45},
        // middle
        {timeAngle[9], timeAngle[12], timeAngle[12], 
            timeAngle[9], timeAngle[12], timeAngle[12]},
        // right
        {timeAngle[6], timeAngle[12], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[12]}
                                    
    },
    { // hour
        // left side of digit
        {timeAngle[6], timeAngle[6], timeAngle[6], 
            timeAngle[3], 45, 45},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[9], 
            timeAngle[6],timeAngle[9], timeAngle[3]},
        //right
        {timeAngle[9], timeAngle[6], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[9]}
    }
                          
};               

// Number five representation
static const digitData FIVE[2] = 
{
    { // minute
        // left
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[3], timeAngle[12]},
        // middle
        {timeAngle[9], timeAngle[3], timeAngle[12], 
            timeAngle[9], timeAngle[12], timeAngle[9]},
        // right
        {timeAngle[9], timeAngle[12], timeAngle[9], 
            timeAngle[12], timeAngle[12], timeAngle[12]}
                                    
    },
    { // hour
        // left side of digit
        {timeAngle[3], timeAngle[6], timeAngle[6], 
            timeAngle[3], timeAngle[6], timeAngle[3]},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[3], 
            timeAngle[6],timeAngle[9], timeAngle[3]},
        //right
        {timeAngle[6], timeAngle[9], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[9]}
    }
                          
};              

// Number six representation
static const digitData SIX[2] = 
{
    { // minute
        // left
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]},
        // middle
        {timeAngle[9], timeAngle[3], timeAngle[12], 
            timeAngle[6], timeAngle[12], timeAngle[9]},
        // right
        {timeAngle[9], timeAngle[12], timeAngle[9], 
            timeAngle[12], timeAngle[12], timeAngle[12]}
                                    
    },
    { // hour
        // left side of digit
        {timeAngle[3], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[3]},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[3], 
            timeAngle[6],timeAngle[12], timeAngle[3]},
        //right
        {timeAngle[6], timeAngle[9], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[9]}
    }
                          
};
               
// Number seven representation       
static const digitData SEVEN[2] = 
{
    { // minute
        // left
        {timeAngle[3], timeAngle[12], 45, 
            45, 45, 45},
        // middle
        {timeAngle[9], timeAngle[9], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]},
        // right
        {timeAngle[9], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[9]}
                                    
    },
    { // hour
        // left side of digit
        {timeAngle[6], timeAngle[3], 45, 
            45, 45, 45},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[6], 
            timeAngle[6],timeAngle[6], timeAngle[3]},
        //right
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]}
    }
                          
};

// Number eight representation
static const digitData EIGHT[2] = 
{
    { // minute
        // left
        {timeAngle[6], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[3]},
        // middle
        {timeAngle[9], timeAngle[6], timeAngle[12], 
            timeAngle[6], timeAngle[12], timeAngle[9]},
        // right
        {timeAngle[9], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[9]}
                                    
    },
    { // hour
        // left side of digit
        {timeAngle[3], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[12], 
            timeAngle[6],timeAngle[12], timeAngle[3]},
        //right
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]}
    }
                          
};

// Number nine representation                     
static const digitData NINE[2] = 
{
    { // minute
        // left
        {timeAngle[6], timeAngle[6], timeAngle[6], 
            timeAngle[12], timeAngle[3], timeAngle[12]},
        // middle
        {timeAngle[9], timeAngle[6], timeAngle[12], 
            timeAngle[9], timeAngle[12], timeAngle[9]},
        // right
        {timeAngle[9], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[9]}
                                    
    },
    { // hour
        // left side of digit
        {timeAngle[3], timeAngle[12], timeAngle[12], 
            timeAngle[3], timeAngle[6], timeAngle[3]},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[12], 
            timeAngle[6],timeAngle[9], timeAngle[3]},
        //right
        {timeAngle[6], timeAngle[12], timeAngle[12], 
            timeAngle[12], timeAngle[12], timeAngle[12]}
    }
                          
};
                        
const uint8_t get_clock_numbers(const uint8_t boardNo, const uint8_t clockNo)
{
    return slave_numbering[boardNo][clockNo];
}

digitData * get_digit_data(const uint8_t digit)
{   
    switch(digit)
    {
        case 0:
            return &ZERO;
        case 1:
            return &ONE;
        case 2:
            return &TWO;
        case 3:
            return &THREE;
        case 4:
            return &FOUR;
        case 5:
            return &FIVE;
        case 6:
            return &SIX;
        case 7:
            return &SEVEN;
        case 8:
            return &EIGHT;
        case 9:
            return &NINE;
    }
            
}