#pragma once
#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>
#include <iostream>

class Person {
protected:
    std::string name;
public:
    explicit Person(const std::string& name);
    virtual ~Person();

    virtual void print() const;
};

#endif
