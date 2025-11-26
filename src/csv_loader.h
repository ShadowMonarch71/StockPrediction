#pragma once
#include <string>
#include <vector>

namespace sp {

struct Bar {
     std::string date; double open, high, low, close; double volume;
     };

class CSVLoader {
public:
    explicit CSVLoader(const std::string &path);
    std::vector<Bar> load();
private:
    std::string path_;
};

} // namespace sp
