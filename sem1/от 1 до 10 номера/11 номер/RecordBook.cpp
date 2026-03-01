#include "RecordBook.hpp"
#include <iostream>

RecordBook::RecordBook() : number("") {}

RecordBook::RecordBook(const std::string& num) : number(num) {}

void RecordBook::addGrade(int grade) {
    grades.push_back(grade);
}

double RecordBook::getAverage() const {
    if (grades.empty()) return 0.0;
    double sum = 0;
    for (int g : grades) {
        sum += g;
    }
    return sum / grades.size();
}

void RecordBook::print() const {
    std::cout << "Zachetka " << number << ", grades: ";
    for (int g : grades) {
        std::cout << g << " ";
    }
    std::cout << "(Average " << getAverage() << ")";
}

bool RecordBook::save(std::ofstream& out) const {
    if (!out.is_open()) return false;

    size_t len = number.length();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(number.c_str(), len);

    size_t count = grades.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (int grade : grades) {
        out.write(reinterpret_cast<const char*>(&grade), sizeof(grade));
    }

    return out.good();
}

bool RecordBook::load(std::ifstream& in) {
    if (!in.is_open()) return false;

    size_t len;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (!in.good()) return false;

    number.resize(len);
    in.read(&number[0], len);

    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    if (!in.good()) return false;

    grades.clear();
    for (size_t i = 0; i < count; ++i) {
        int grade;
        in.read(reinterpret_cast<char*>(&grade), sizeof(grade));
        if (!in.good()) return false;
        grades.push_back(grade);
    }

    return true;
}
