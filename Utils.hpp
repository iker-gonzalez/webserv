#pragma once
#include <string>
#include <vector>
// Se mira si los atributos como client_size y error_page
// no tiene ningun caracter antes de convertirlos a nçumeros
bool isDigit(const std::string& a_attributes);
bool splitString(std::string& aline, std::string& character, std::vector<std::string>& asplit_line);
bool CheckExtension(const std::string& filename, std::string a_extension);
bool onlySpaces(const std::string&  aLine);
bool errorMessage(const std::string message);
