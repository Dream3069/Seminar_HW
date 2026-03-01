#pragma once
#ifndef RECORDBOOK_H
#define RECORDBOOK_H

#include <string>
#include <vector>

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

    inline const std::string& getNumber() const { return number; }
};

#endif 
