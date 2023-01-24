#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

bool isDigit(const std::string& a_attributes);
bool splitString(std::string& aline, std::string& character, std::vector<std::string>& asplit_line);
bool CheckExtension(const std::string& filename, std::string a_extension);
bool onlySpaces(const std::string&  aLine);
bool errorMessage(const std::string message);

#endif