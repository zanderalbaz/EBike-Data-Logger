#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

// TO DO:
// integrate with live data collection *** (Sensor_I2C_3.ino)
// 50 points (WINDOW_SIZE) >?? window size shuld be set alreadY? becasue live window?? ->idk dude...
    //I think this will work with data collection becasue will only collect 50 points; does not work rn
// read from all three sensors -> currrently have two 
    // make sure it has the same order as python pre-processed info
// add ML algorithm as a header
// do not worry about SD


using namespace std; 
//std namespace not supported on arduino... hmmmmmm.....
//need to run this in arduino IDE
int main(){
    ifstream file("inside.csv");
    if(!file.is_open()){
        cerr << "Error opening file" << endl;
        return -1;
    }
    
    vector<vector<string>> table;
    string line;
    
    while(getline(file, line)){
        stringstream ss(line);
        string cell;
        vector<string> row;

        while (getline(ss, cell, ',')){ 
            row.push_back(cell);
        }
        table.push_back(row); //creating the table

        }
        file.close();

        vector <double> dataForML;

        for(int i = 0; i < 18; i++){
            double sum = 0;
            double max = 0;
            double min = 0;
            int numCountData = 0;

            for(const auto& row : table){//each row in the table
                if(row[row.size()-1] == "2"){ //need to break up into 50 data points instead i think -> but this gets class
                    //idk how this works with live data
                    numCountData ++;
                    sum += stod(row[i]);
                    if(max<stod(row[i])){
                        max = stod(row[i]);
                    }
                    if(min>stod(row[i])){
                        min = stod(row[i]);
                    }
                }else{
                    continue;
                }
            }
            double avg = sum/numCountData;

            //STANDARD DEVIATION
            double toSum = 0;
            double summation = 0;
            for(const auto& row : table){//each row in the table
                if(row[row.size()-1] == "2"){
                    double mid = stod(row[i])-avg;
                    toSum = mid*mid;
                }
                summation += toSum;
            }

            double toSqrt = summation/numCountData;
            double stdev = sqrt(toSqrt); //appears to be working, but slightly different output than excel

            cout << endl << "Average: "<< avg <<endl;
            cout << "Maximum value: "<< max <<endl;
            cout << "Minimum value: "<< min <<endl;
            cout << "Standard deviation: " << stdev << endl << endl;
            dataForML.push_back(avg);
            dataForML.push_back(max);
            dataForML.push_back(min);
            dataForML.push_back(stdev);

            
        }
        dataForML.push_back(2); //must update for proper class
        for(const auto& data : dataForML){
            cout << data <<", "; //remember, this is the "footprint" for a single bike crossing the sensor platform (kinda)
        }
        cout<<endl;
    return 0; 
}

//output to MLA -> need to port to C


/*NOTES
while/if/for loops must be inside of a function -> duh dumbass
must have int "main" function
./a.exe, not ./a.out


*/