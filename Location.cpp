#include "Location.hpp"



Location::Location()
{
}


Location::~Location()
{
}


std::string Location::getPath() const
{
	return _path;
}

std::string Location::getCgiPass() const
{
	return _cgi_pass;
}

std::string Location::getReturn() const
{
	return _return;
}

void Location::setPath(const std::string& a_path)
{
	_path = a_path;
}

void Location::setCgiPass(const std::string& a_cgi_pass)
{
	_cgi_pass = a_cgi_pass;
}

void Location::setReturn(const std::string& a_return)
{
	_return = a_return;
}

bool Location::fillSpecificInfo(std::vector<std::string>& a_v_strSplit)
{
	if (!(a_v_strSplit[0].compare("location")) && checkSize(a_v_strSplit, 3, 3))
	{
		_path = a_v_strSplit[1];
		_closeBracket = false;
		return true;
	}
	else if (!(a_v_strSplit[0].compare("cgi_pass")) && checkSize(a_v_strSplit, 2, 2))
	{
		_cgi_pass = a_v_strSplit[1];
		return true;
	}
	else if (!(a_v_strSplit[0].compare("return")) && checkSize(a_v_strSplit, 2, 2))
	{
		_return = a_v_strSplit[1];
		return true;
	}
	return false;
}

std::ostream& operator<<(std::ostream& ors, const Location& location)
{
	ors << "listen" << location.getListen() << std::endl;
	ors << "server_name" << location.getServerName() << std::endl;
	ors << "root" << location.getRoot() << std::endl;
	ors << "index" << location.getIndex() << std::endl;
	std::vector<std::string> v_methods = location.getMethods();
	for (unsigned int i = 0; i < v_methods.size(); i++) {
		ors << "methods[" << i << "]: " << v_methods[i] << std::endl;
	}
	std::map<int, std::string> m_errors = location.getErrorPage();
	std::map<int, std::string>::iterator it;
	for (it = m_errors.begin(); it != m_errors.end(); it++) {
		ors << "error_page: "
			<< it->first    // string (key)
			<< ':'
			<< it->second   // string's value 
			<< std::endl;
	}
	ors << "client_size" << location.getClientSize() << std::endl;
	ors << "autoindex" << location.getAutoindex() << std::endl;
	return ors;
}