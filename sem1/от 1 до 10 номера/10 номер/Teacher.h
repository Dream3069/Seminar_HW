#pragma once
#ifndef TEACHER_H
#define TEACHER_H

#include "Person.h"
#include <string>

class Teacher : public Person {
private:
    std::string subject;
public:
    Teacher(const std::string& name, const std::string& subj);
    void print() const override;

    inline const std::string& getSubject() const { return subject; }
};

#endif 
