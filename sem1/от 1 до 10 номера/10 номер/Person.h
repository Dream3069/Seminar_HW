#pragma once
#ifndef PERSON_H
#define PERSON_H

#include <iostream>
#include <string>

class Person {
protected:
    std::string name;
public:
    explicit Person(const std::string& name);
    virtual ~Person() {}

    inline std::string getName() const { return name; }

    virtual void print() const;
};

#endif 
