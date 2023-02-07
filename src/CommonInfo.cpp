#include "../includes/CommonInfo.hpp"
#include "../includes/Utils.hpp"

#include <cstdlib> //for std::atoi 
#include <iostream> //quitar
CommonInfo::CommonInfo(void) : _client_size(0), _autoindex(false)
{
	_is_listen = false;
	_is_server_name = false;
	_is_root = false;
	_is_index = false;
	_is_v_methods = false;
	_is_m_error_page = false;
	_is_client_size = false;
	_is_autoindex = false;

	//Empieza cerrado
	_closeBracket = true;
}


CommonInfo::~CommonInfo()
{
}

bool CommonInfo::checkSize(std::vector<std::string>& a_v_info, unsigned int minValue, unsigned int  maxValue)
{
	if (a_v_info.size() < minValue || a_v_info.size() > maxValue)
		return false;

	return true;

}

bool CommonInfo::fillInformation(std::vector<std::string>& a_v_strSplit)
{
	if (!(a_v_strSplit[0].compare("server_name")) && checkSize(a_v_strSplit, 2, 2) &&
		checkServerName(a_v_strSplit[1]))
		_server_name = a_v_strSplit[1];
	else if (!(a_v_strSplit[0].compare("root")) && checkSize(a_v_strSplit, 2, 2) && checkRoot(a_v_strSplit[1]))
		_root = a_v_strSplit[1];
	else if (!(a_v_strSplit[0].compare("index")) && checkSize(a_v_strSplit, 2, 2) && checkIndex(a_v_strSplit[1]))
		_index = a_v_strSplit[1];
	else if (!(a_v_strSplit[0].compare("methods")) && checkSize(a_v_strSplit, 2, 4))
	{
		a_v_strSplit.erase(a_v_strSplit.begin());
		if (!checkMethods(a_v_strSplit))
			return errorMessage("ERROR: Parse in 'method' attribute");
		_v_methods = a_v_strSplit;
	}
	else if (!(a_v_strSplit[0].compare("error_page")) && checkSize(a_v_strSplit, 2, 3) 
		&& checkErrorPage(a_v_strSplit[1], a_v_strSplit[2]))
	{
		_m_error_page.insert(std::pair<int, std::string>(std::atoi(a_v_strSplit[1].c_str()), a_v_strSplit[2]));
	}
	else if (!(a_v_strSplit[0].compare("client_size")) && checkSize(a_v_strSplit, 2, 2) && checkClientSize(a_v_strSplit[1]))
		_client_size = std::atoi(a_v_strSplit[1].c_str());
	else if (!(a_v_strSplit[0].compare("autoindex")) && checkSize(a_v_strSplit, 2, 2) && checkAutoindex(a_v_strSplit[1]))
		_autoindex = true;
	else if (!(a_v_strSplit[0].compare("}")) && !isCloseBracket())
		_closeBracket = true;
	else
		return false;
	return true;

}

bool CommonInfo::Parse(std::string& aLine)
{
	std::string line_spaces;
	if (aLine.empty() || onlySpaces(aLine))
		return true;

	std::vector<std::string> v_strSplit;
	std::string s = " ";
	if (!splitString(aLine, s, v_strSplit))
		return false;

	if (!fillInformation(v_strSplit) && !fillSpecificInfo(v_strSplit))
		return false;
	return true;


}
std::string CommonInfo::getListen() const
{
	return _listen;
}

std::string CommonInfo::getServerName() const
{
	return _server_name;
}

std::string CommonInfo::getRoot() const
{
	return _root;
}

std::string CommonInfo::getIndex() const
{
	return _index;
}

std::vector<std::string> CommonInfo::getMethods() const
{
	return _v_methods;
}

std::map<int, std::string> CommonInfo::getErrorPage() const
{
	return _m_error_page;
}


int CommonInfo::getClientSize() const
{
	return _client_size;
}

bool CommonInfo::getAutoindex() const
{
	return _autoindex;
}

void CommonInfo::setListen(const std::string& alisten) 
{
	_listen = alisten;
}

void CommonInfo::setServerName(const std::string& aserver_name)
{
	_server_name = _server_name;
}

void CommonInfo::setRoot(const std::string& aroot)
{
	_root = aroot;
}

void CommonInfo::setIndex(const std::string& aindex)
{
	_index = aindex;
}

void CommonInfo::setMethods(const std::vector<std::string>& a_v_methods)
{
	_v_methods = a_v_methods;
}

void CommonInfo::setErrorPage(const std::map<int, std::string>& a_m_error_page)
{
	_m_error_page = a_m_error_page;
}


void CommonInfo::setClientSize(int& a_client_size)
{
	_client_size = a_client_size;
}

void CommonInfo::setAutoindex(bool a_auto_index)
{
	_autoindex = a_auto_index;
}

void CommonInfo::setcloseBracket(bool a_close)
{
	_closeBracket = a_close;
}


bool CommonInfo::checkServerName(const std::string& aserver_name)
{
	if (_is_server_name == true)
		return false;
	_is_server_name = true;
	return true;
}
bool CommonInfo::checkRoot(const std::string& aroot)
{
	if (_is_root == true)
		return false;
	//Mirar que exista el directorio
	_is_root = true;
	return true;
}
bool CommonInfo::checkIndex(const std::string& aindex)
{
	if (_is_index == true)
		return false;
	if (!CheckExtension(aindex, ".html"))
		return false;
	_is_index = true;
	return true;
}
bool CommonInfo::checkMethods(const std::vector<std::string>& a_v_methods)
{

	if (_is_v_methods == true)
		return false;
		std::cout << _is_v_methods << std::endl;
	for (unsigned int i = 0; i < a_v_methods.size(); i++)
	{
		std::cout << "m:" << a_v_methods[i] << std::endl;
		if (a_v_methods[i] != "GET" &&
			a_v_methods[i] != "DELETE" &&
			a_v_methods[i] != "POST")
			return false;
	}
	_is_v_methods = true;
	return true;
}
bool CommonInfo::checkErrorPage(const std::string& number, const std::string& aindex)
{
	if (!isDigit(number))
		return false;
	if (!CheckExtension(aindex, ".html"))
		return false;
	_is_m_error_page = true;
	return true;
}
bool CommonInfo::checkClientSize(const std::string& a_client_size)
{
	if (_is_client_size == true)
		return false;
	if (!isDigit(a_client_size))
		return false;
	_is_client_size = true;
	return true;
}
bool CommonInfo::checkAutoindex(std::string& a_auto_index)
{
	if (_is_autoindex == true)
		return false;
	if (a_auto_index.compare("on") && a_auto_index.compare("off"))
		return false;
	_is_autoindex = true;
	return true;
}
bool CommonInfo::checkcloseBracket(bool a_auto_index)
{
	if (_is_root == true)
		return false;
	_is_root = true;
	return true;

}

bool CommonInfo::isCloseBracket()
{
	return _closeBracket;
}
