#pragma once
#ifndef STUDENT_H
#define STUDENT_H

#include "Person.h"
#include <string>

class RecordBook;

class Student : public Person {
private:
    RecordBook* recordBook;
public:
    explicit Student(const std::string& name);
    Student(const std::string& name, const std::string& recordNum);
    ~Student();

    void addGrade(int grade);
    double getAverage() const;
    void print() const override;

    inline bool hasRecordBook() const { return recordBook != nullptr; }
};

#endif 
