#ifndef COMMONINFO_HPP
#define COMMONINFO_HPP

#include <string>
#include <map>
#include <vector>
class CommonInfo
{
	protected:
		std::string					_listen;
		std::string					_root;
		std::string					_index;
		std::string					_error_page;
		std::vector<std::string>	_v_methods;
		std::map<int, std::string>	_m_error_page;
		int							_client_size;
		bool						_autoindex;

		// Security attributes
		bool _closeBracket;

		//Exist unique attributes
		bool _is_listen;
		bool _is_root;
		bool _is_index;
		bool _is_v_methods;
		bool _is_m_error_page;
		bool _is_client_size;
		bool _is_autoindex;

	public:
		CommonInfo();
		virtual ~CommonInfo();
		CommonInfo(const CommonInfo& rhs);
		CommonInfo& operator=(const CommonInfo& rhs);

	public:
		// Getters
		std::string					getListen()const;
		std::string					getRoot()const;
		std::string					getIndex()const;
		std::vector<std::string>	getMethods() const;
		std::map<int, std::string>	getErrorPage() const;
		int							getClientSize() const;
		bool						getAutoindex()const;
		
		// Setters
		void setListen(const std::string& alisten);
		void setRoot(const std::string& aroot);
		void setIndex(const std::string& aindex);
		void setMethods(const std::vector<std::string>& a_v_methods) ;
		void setErrorPage(const std::map<int, std::string>& am_error_page) ;
		void setClientSize(int& a_client_size) ;
		void setAutoindex(bool a_auto_index);
		void setcloseBracket(bool a_auto_index);

		//Check
		bool checkRoot(const std::string& aroot);
		bool checkIndex(const std::string& aindex);
		bool checkMethods(const std::vector<std::string>& a_v_methods);
		bool checkErrorPage(const std::string& number, const std::string& aindex);
		bool checkClientSize(const std::string& a_client_size);
		bool checkAutoindex(std::string& a_auto_index);
		bool checkcloseBracket(bool a_auto_index);
		

		//IS
		bool isCloseBracket() const;
		bool isListen() const;
		bool isRoot() const;
		bool isIndex() const;
		bool isMethods() const;
		bool isErrorPage() const;
		bool isClientSize() const;


		//Parseo
		bool		Parse(std::string& aLine);
		bool		checkSize(std::vector<std::string>& a_v_info, unsigned int minValue, unsigned int  maxValue);
		bool		fillInformation(std::vector<std::string>& asplit_line);
		virtual bool fillSpecificInfo(std::vector<std::string>& a_v_strSplit) = 0;

};

#endif