#pragma once
#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <vector>

class Student;

class Group {
private:
    std::string name;
    std::vector<Student*> students;
public:
    explicit Group(const std::string& groupName);

    void addStudent(Student* student);
    void removeStudent(Student* student);
    void removeStudent(int index);

    double getGroupAverage() const;
    void print() const;

    inline const std::string& getName() const { return name; }
    inline size_t getStudentCount() const { return students.size(); }
};

#endif 
