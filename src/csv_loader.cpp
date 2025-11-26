/*
 * csv_loader.cpp
 *
 * Simple CSV loader implementation for OHLCV (Date,Open,High,Low,Close,Volume) files.
 *
 * Responsibilities:
 *  - Open the file given in the CSVLoader constructor
 *  - Validate the header contains the "Date" column (basic sanity check)
 *  - Read each data line and parse the six expected columns in order
 *  - Convert numeric fields to double using std::stod and populate a Bar struct
 *  - Return a vector<Bar> containing all parsed rows
 *
 * Assumptions and behavior:
 *  - CSV must have a header line. The loader only checks that the header contains the
 *    substring "Date"; it does not validate the full header names or column order more
 *    strictly. If you have a different header layout, update the validation here.
 *  - Each data line is expected to be: Date,Open,High,Low,Close,Volume
 *    with numeric fields parseable by std::stod (use dot decimal). Fields are split on ','.
 *  - Empty lines are skipped.
 *  - On failure to open the file, the loader throws std::runtime_error.
 *  - On malformed numeric data, std::stod will throw an exception which propagates to the caller.
 *
 * Complexity:
 *  - Time: O(N) where N is number of data lines
 *  - Memory: O(N) to store parsed rows
 *
 * Usage example:
 *  CSVLoader loader("data/sample.csv");
 *  auto bars = loader.load();
 *
 * If you need more robust parsing (quoted fields, different separators, thousands separators,
 * or heterogeneous headers) replace this simple parser with a dedicated CSV library.
 */

#include "csv_loader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace sp;
using std::getline;

// Construct with the path to the CSV file to be loaded.
CSVLoader::CSVLoader(const std::string &path) : path_(path) {}

// Load the CSV and return a vector of Bars.
// Throws std::runtime_error if the file cannot be opened or if header validation fails.
std::vector<Bar> CSVLoader::load() {
    std::vector<Bar> rows;

    // Open input file stream
    std::ifstream in(path_);
    if (!in) throw std::runtime_error("failed to open the file: " + path_);

    std::string line;

    // Read header line. We expect a header and perform a minimal validation.
    if (!getline(in, line)) return rows; // empty file -> return empty vector

    // Minimal header validation: ensure the header contains the word "Date".
    // This guards against accidentally feeding a wrong file, but is intentionally lightweight.
    if (line.find("Date") == std::string::npos) throw std::runtime_error("CSV header must contain 'Date'");

    // Read data lines
    while (getline(in, line)) {
        if (line.empty()) continue; // skip blank lines

        // Parse comma-separated tokens using a stringstream. This simple approach
        // does not handle quoted commas or escaped characters. If you need that,
        // use a proper CSV parsing library.
        std::istringstream ss(line);
        std::string tok;
        Bar b{}; // zero-initialized Bar

        // Expected order: Date,Open,High,Low,Close,Volume
        getline(ss, b.date, ',');

        // Parse numeric fields; std::stod will throw if the token is not a valid number.
        getline(ss, tok, ','); b.open = std::stod(tok);
        getline(ss, tok, ','); b.high = std::stod(tok);
        getline(ss, tok, ','); b.low = std::stod(tok);
        getline(ss, tok, ','); b.close = std::stod(tok);
        getline(ss, tok, ','); b.volume = std::stod(tok);

        rows.push_back(b);
    }

    return rows;
}
