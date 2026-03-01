#pragma once
#ifndef GROUP_HPP
#define GROUP_HPP

#include <string>
#include <vector>
#include <fstream>
#include "Student.hpp"

class Group {
private:
    std::string name;
    std::vector<Student*> students;
public:
    explicit Group(const std::string& groupName);
    ~Group();

    void addStudent(Student* student);
    void removeStudent(const std::string& studentName);
    void printAll() const;
    double getAverageGrade() const;

    void sortByName();
    void sortByAverage();
    std::vector<Student*> filterByAverage(double minAverage) const;

    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
};

#endif
