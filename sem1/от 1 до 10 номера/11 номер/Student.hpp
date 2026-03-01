#pragma once
#ifndef STUDENT_HPP
#define STUDENT_HPP

#include "Person.hpp"
#include "RecordBook.hpp"

class Student : public Person {
private:
    RecordBook recordBook;
public:
    explicit Student(const std::string& name);
    Student(const std::string& name, const std::string& recordNum);
    const std::string& getName() const;

    void addGrade(int grade);
    double getAverage() const;
    void print() const override;

    bool save(std::ofstream& out) const;
    bool load(std::ifstream& in);
};

#endif
