// reads stock data from csv file into Bar structs

#include "csv_loader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace sp;
using std::getline;

CSVLoader::CSVLoader(const std::string &path) : path_(path) {}

std::vector<Bar> CSVLoader::load() {
    std::vector<Bar> rows;

    std::ifstream in(path_);
    if (!in) throw std::runtime_error("failed to open the file: " + path_);

    std::string line;

    // skip header line
    if (!getline(in, line)) return rows;

    if (line.find("Date") == std::string::npos) throw std::runtime_error("CSV header must contain 'Date'");

    // parse each row: Date,Open,High,Low,Close,Volume
    while (getline(in, line)) {
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string tok;
        Bar b{};

        getline(ss, b.date, ',');

        getline(ss, tok, ','); b.open = std::stod(tok);
        getline(ss, tok, ','); b.high = std::stod(tok);
        getline(ss, tok, ','); b.low = std::stod(tok);
        getline(ss, tok, ','); b.close = std::stod(tok);
        getline(ss, tok, ','); b.volume = std::stod(tok);

        rows.push_back(b);
    }

    return rows;
}
