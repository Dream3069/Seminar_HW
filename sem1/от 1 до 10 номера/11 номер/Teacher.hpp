#pragma once
#ifndef TEACHER_HPP
#define TEACHER_HPP

#include "Person.hpp"

class Teacher : public Person {
private:
    std::string subject;
public:
    Teacher(const std::string& name, const std::string& subj);

    void print() const override;
};

#endif
