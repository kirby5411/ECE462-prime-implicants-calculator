#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <cmath>
#include <algorithm>

using namespace std;

// Define DEBUG_MODE to enable debug output
int DEBUG_MODE = 0;
int STEP_BY_STEP = 0;

// Define DEBUG_PRINT to print debug messages
#define DEBUG_PRINT(msg) \
    do { if(DEBUG_MODE) cout << msg; } while (false)

// Define SBS_PRINT to print step by step solution
#define SBS_PRINT(msg) \
    do { if(STEP_BY_STEP) cout << msg; } while (false)


vector<string> int_to_binary(vector<int> &int_vec, int num_literals){
    vector<string> ans;
    for(int i = 0 ; i < int_vec.size() ; i++){
        string temps = bitset<sizeof(int) *8>(int_vec[i]).to_string();
        temps = temps.substr(temps.length() - num_literals);
        DEBUG_PRINT("Number "<<i<<"'s value in decimal is: "<<int_vec[i]<<endl);
        DEBUG_PRINT("Number "<<i<<"'s value in binary string is: "<<temps<<endl);
        ans.push_back(temps);
    }
    return ans;
}

int main() {
    ifstream file("testcase.txt"); // Open the file
    string line;

    if (file.is_open()) {
        // Skip the first 6 lines
        for (int i = 0; i < 6; ++i) {
            getline(file, line);
        }

        getline(file, line); // Read a line from the file containing integers
        istringstream first_iss(line); // Create a string stream to parse the line
        first_iss >> DEBUG_MODE >> STEP_BY_STEP; // set debug mode and step by step
        cout<<"DEBUG_MODE is set to: " << DEBUG_MODE << endl;
        cout<<"STEP_BY_STEP is set to: " << STEP_BY_STEP << endl;

        getline(file, line);
        istringstream second_iss(line); // Create a string stream to parse the line
        int numliteral;

        second_iss >> numliteral;
        DEBUG_PRINT("literal count is: " << numliteral << endl);

        getline(file, line);
        istringstream third_iss(line); // Create a string stream to parse the line
        vector<int> terms; // minterms or maxterms (currently only support minterms)
        int temp;

        // Read integers from the line and store them in the vector
        while (third_iss >> temp) {
            terms.push_back(temp);
        }
        file.close(); // Close the file

        for (int i : terms) {
            DEBUG_PRINT(i << " ");
        }
        DEBUG_PRINT(endl);

        DEBUG_PRINT("length of terms recieved is: " << terms.size() << endl);

        //checking input integrity
        if(terms.size() == 0 || terms.size() > pow(2,numliteral)){
            cerr << "Incorrect input format" << endl;
            return 1;
        }
        for(int i = 0 ; i < terms.size() ; i++){
            if(i!=0){
                if(terms[i] <= terms[i-1]){
                    cerr << "Incorrect input format" << endl;
                    return 1;
                }
            }
        }

        // converting input to binary
        vector<string> ans = int_to_binary(terms, numliteral);
        vector<string> temp_vs = ans;

        bool reduction_finished = false;
        int reduction_count = 0;
        // reductions
        while(!reduction_finished){
            SBS_PRINT("After "<<reduction_count<<" reduction"<<endl);
            for(int i = 0 ; i < ans.size() ; i++){
                SBS_PRINT(ans[i]<<endl);
            }
            reduction_finished = true;
            ans.clear();
            vector<bool> added;
            for(int i = 0 ; i < temp_vs.size() ; i++){
                added.push_back(false);
            }
            for(int i = 0 ; i < temp_vs.size()-1 ; i++){
                for(int k = i+1 ; k < temp_vs.size() ; k++){
                    int cnt = 0;
                    int combine_pos = -1;
                    bool cannot_combine = false;
                    for(int j = 0 ; j < numliteral ; j++){
                        if(temp_vs[k][j] == '_'){
                            if(temp_vs[i][j] == '_'){
                                continue;
                            } else {
                                cannot_combine = true;
                                break;
                            }
                        }
                        if(temp_vs[i][j] == '_'){
                            if(temp_vs[k][j] == '_'){
                                continue;
                            } else {
                                cannot_combine = true;
                                break;
                            }
                        }
                        if(temp_vs[i][j] != temp_vs[k][j]){
                            cnt++;
                            combine_pos = j;
                            if(cnt >= 2){
                                cannot_combine = true;
                                break;
                            }
                        }
                    }
                    if(cannot_combine){
                        continue;
                    }
                    if(combine_pos == -1){
                        cerr << "Two identical elements present in vector. Halting program immediately..."<<endl;
                        return 1;
                    }
                    added[i] = true;
                    added[k] = true;
                    string temps = temp_vs[i];
                    temps[combine_pos] = '_';
                    bool is_in_ans = false;
                    for(int j = 0 ; j < ans.size() ; j++){
                        if(temps == ans[j]){
                            is_in_ans = true;
                            break;
                        }
                    }
                    if(!is_in_ans){
                        ans.push_back(temps);
                        reduction_finished = false;
                    }
                }
                if(!added[i]){
                    ans.push_back(temp_vs[i]);
                    added[i] = true;
                }
            }
            //push last element too if it is not included
            if(!added[added.size()-1]){
                ans.push_back(temp_vs[added.size()-1]);
            }
            temp_vs = ans;
            reduction_count++;
        }

        cout<<"Program finished after "<<reduction_count-1<<" reduction"<<endl;
        cout<<"Final prime implicants are: "<<endl;
        for(int i = 0 ; i < ans.size() ; i++){
            vector<int> represeted_int;
            vector<int> _pos;
            int fixed_value = 0;
            for(int j = 0 ; j < ans[i].size() ; j++){
                if(ans[i][j] == '1'){
                    fixed_value += pow(2,numliteral-1-j);
                } else if (ans[i][j] == '_'){
                    _pos.push_back(numliteral-1-j);
                }
            }
            for(int j = 0 ; j < pow(2,_pos.size()) ; j++){
                int tempi = fixed_value;
                for(int k = 0 ; k < _pos.size() ; k++){
                    if(j%(int)(pow(2,k+1)) >= (int)pow(2,k)){
                        tempi += pow(2,_pos[k]);
                    }
                }
                represeted_int.push_back(tempi);
            }
            sort(represeted_int.begin(), represeted_int.end());
            cout<<ans[i]<<" (";
            for(int j = 0 ; j < represeted_int.size() ; j++){
                cout<<" "<<represeted_int[j];
            }
            cout<<" )"<<endl;
        }



    } else {
        cerr << "Unable to open file!" << endl;
        return 1;
    }

    return 0;
}
