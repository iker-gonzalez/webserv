#include "../includes/Utils.hpp"
#include <algorithm>
#include <vector>
#include <iostream>
bool isDigit(const std::string& a_attributes)
{
	for (unsigned int i = 0; i < a_attributes.size(); ++i)
	{
		if (a_attributes[i] < '0' || a_attributes[i] > '9')
				return false;
	}
	return true;
}

bool splitString(std::string& aline, std::string& character, std::vector<std::string>& asplit_line)
{
	std::string create_line;
	std::string::iterator it = aline.begin();

	while (it != aline.end() && (*it) == ' ')
		it++;
	for (it; it != aline.end(); ++it)
	{
		if (*it != ' ')
			create_line += *it;
		else
		{
			if (create_line.empty())
				continue;
			asplit_line.push_back(create_line);
			create_line.clear();
		}
	}
	asplit_line.push_back(create_line);
	return true;
}

bool CheckExtension(const std::string& filename, std::string a_extension)
{
	std::string extension(a_extension);
	std::size_t found = filename.rfind(extension);
	if (found == std::string::npos && found != (filename.size() - extension.size()))
		return false;

	return true;
}

bool onlySpaces(const std::string& aLine)
{
	for (unsigned int i = 0; i < aLine.size(); ++i)
	{
		if (aLine[i] !=  ' ')
			return false;
	}
	return true;

}

bool errorMessage(const std::string message)
{
	std::cout << message << std::endl;
	return false;
}
