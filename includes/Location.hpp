#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "CommonInfo.hpp"
#include <ostream>

class Location : public CommonInfo
{
	public:

		Location();
		~Location();
	
		// Getters
		std::string getPath() const;
		std::string getCgiPass() const;
		std::string getReturn() const;
		std::string getAlias() const;
	
		// Setters
		void setPath(const std::string& a_path);
		void setCgiPass(const std::string& a_cgi_pass);
		void setReturn(const std::string& a_return);
	
		// Parse
		bool fillSpecificInfo(std::vector<std::string>& a_v_strSplit);
	
	private:
	
		std::string					_path;
		std::string					_return;
		std::string					_cgi_pass;
		std::string					_alias;
	
};
std::ostream& operator<<(std::ostream& ors, const Location& location);


#endif // !LOCATION_HPP
