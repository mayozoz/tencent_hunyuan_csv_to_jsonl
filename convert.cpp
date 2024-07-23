// program converts csv file input to jsonl file output in format id, value, query (optional)
// might need c++11 to compile: g++ -std=c++11 convert.cpp -o convert
// split file: split -l $(( $(wc -l < FILE_NAME.jsonl) / 6 )) FILE_NAME.jsonl OUT_FILE_INDEX_

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

class JConverter {
public:
    long long id;
    std::string value; 
    std::string query;
};

// Function to remove double quotes from a string
std::string removeDoubleQuotes(const std::string& str) {
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_csv_file> <output_jsonl_file>" << std::endl;
        return 1;
    }

    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cerr << "Error opening input file." << std::endl;
        return 1;
    }

    std::ofstream outfile(argv[2], std::ios::out | std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "Error opening output file." << std::endl;
        return 1;
    }

    std::string line;
    std::vector<std::string> headers;

    // read headers
    if (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string header;
        
        while (std::getline(iss, header, ',')) {
            headers.push_back(removeDoubleQuotes(header));
        }
    }

    long long id_num = 0; 
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        JConverter data;
        std::string cell;
        int index = 0;
        data.id = id_num;
        
        while (std::getline(iss, cell, ',')) {
            // default add to value
            data.value += headers[index] + ": " + removeDoubleQuotes(cell) + ", "; 
            if (headers[index] == "account_name" || (headers[index] == "sname" && cell.length() > 0)) {
                data.query = removeDoubleQuotes(cell);
            }
            index++;
        }
        data.value.erase(data.value.size()-2); // erase last 2 chars

        json json_line;
        json json_obj = {{"id", data.id}, {"value", data.value}, {"query", data.query}}; 

        outfile << json_obj.dump() << '\n';
        id_num++; 
    }

    

    infile.close();
    outfile.close();

    std::cout << "JSONL file created successfully." << std::endl;

    return 0;
}