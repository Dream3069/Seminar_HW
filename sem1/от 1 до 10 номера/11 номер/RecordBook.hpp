#pragma once
#ifndef RECORDBOOK_HPP
#define RECORDBOOK_HPP

#include <string>
#include <vector>
#include <fstream>

class RecordBook {
private:
    std::string number;
    std::vector<int> grades;
public:
    RecordBook();
    explicit RecordBook(const std::string& num);

    void addGrade(int grade);
    double getAverage() const;
    void print() const;

    bool save(std::ofstream& out) const;
    bool load(std::ifstream& in);
};

#endif
