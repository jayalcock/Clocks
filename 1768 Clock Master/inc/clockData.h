/** @file clockData.h
 * 
 * @brief Data file for clock info
 *
 * @par       
 */ 

#ifndef _CLOCKDATA_H_
#define _CLOCKDATA_H_


// Clock hour representation of angular position
static const uint16_t timeAngle[13] = {0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 0};
    
    
// Matrix for storing clock specific angle data for invidiviual digits
typedef uint16_t digitData[3][6];
//typedef uint16_t digitData[][6];

//typedef struct 
//{
//    digitData minute;
//    digitData hour;
                  
//} referenceData;

                            
digitData ZERO[2] =
{
    { // minute
        // left
         {timeAngle[3], timeAngle[12], timeAngle[6], 
            timeAngle[12], timeAngle[12], timeAngle[12]},
        // middle
        {timeAngle[3], timeAngle[6], timeAngle[6], 
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
            timeAngle[12],timeAngle[12], timeAngle[9]},
        //right
        {timeAngle[9], timeAngle[6], timeAngle[6], 
            timeAngle[6], timeAngle[6], timeAngle[9]}
    } 
                                
};
                            
digitData ONE[2] = 
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

digitData TWO[2] = 
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
        

digitData THREE[2] = 
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
                   
digitData FOUR[2] = 
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

digitData FIVE[2] = 
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

digitData SIX[2] = 
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
                      
digitData SEVEN[2] = 
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

digitData EIGHT[2] = 
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

                      
digitData NINE[2] = 
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
                      

                      
#endif /*_CLOCKDATA_H_*/