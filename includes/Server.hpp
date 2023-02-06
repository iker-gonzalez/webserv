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

	//Parseo
	bool fillSpecificInfo(std::vector<std::string>& a_v_strSplit);

	//Getters
	std::vector<Location> getLocations() const;
	Location& getLocationByReference(int nbr_localitaion) ;
	
	void pushBackLocation(Location& a_location);

	//Tengo que poner esto en privado
	bool locationBracketOpen;
private:
	std::vector<Location> _v_location;
};

std::ostream& operator<<(std::ostream& ors, const Server& server);

#endif // !SERVER_HPP

