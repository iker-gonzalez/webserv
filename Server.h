#ifndef SERVER_HPP
#define SERVER_HPP

#include "CommonInfo.hpp"
#include "Location.hpp"
#include <map>
#include <vector>
#include <ostream>
class Server : public CommonInfo
{
public:
	Server();
	~Server();
	//bool splitString( std::string& aline,  std::string& character, std::vector<std::string>& asplit_line);
	//bool checkSize(std::vector<std::string>& a_v_info, unsigned int minValue, unsigned int  maxValue);
	//bool fillInformation(std::vector<std::string>& asplit_line);

	//bool Parse(std::string aLine);
	void fillNecssaryInfo();
	bool fillSpecificInfo(std::vector<std::string>& a_v_strSplit);

	std::map<std::string, int> _m_necessary_info;
	std::vector<Location> getLocation() const;
	Location& getLocationByReference(int nbr_localitaion) ;
	void pushBackLocation(Location& a_location);
	bool locationBracketOpen = false;
private:
	std::vector<Location> _v_location;
};

std::ostream& operator<<(std::ostream& ors, const Server& server);

#endif // !SERVER_HPP

