#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
//want to minimize # of libraries?

using namespace std; //std not supported on arduino...
int main(){
    ifstream file("0131_2.csv");
    if(!file.is_open()){
        cerr << "Error opening file" << endl;
        return -1;
    }
    //IRL: will grab 50 data points from predetermined location in memory -> should still exist as array of arrays, but will have to figure out how to open that"file"
    
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

        for(int i = 0; i < 9; i++){ // there are 10 rows -> 10th is class NEED TO EDIT WHEN ALL SENSOR S FXNAL
            double sum = 0;
            double max = 0;
            double min = 0;
            int numCountData = 0;

            for(const auto& row : table){//each row in the table
                if(row[row.size()-1] == "2"){ //need to break up into 50 data points instead i think -> but this gets class
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

            //cout << endl << "Average: "<< avg <<endl;
            //cout << "Maximum value: "<< max <<endl;
            //cout << "Minimum value: "<< min <<endl;
            //cout << "Standard deviation: " << stdev << endl << endl;
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