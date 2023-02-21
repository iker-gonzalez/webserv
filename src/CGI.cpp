#include "../includes/CGI.hpp"
#include "../includes/Request.hpp"


CGI::CGI()
{

    
}

CGI::~CGI()
{

}

CGI::CGI(const CGI &other)
{
   
}

/*
    Estas variables son esenciales para que el script CGI pueda funcionar correctamente y procesar la solicitud del navegador:

    REQUEST_METHOD: Esta variable contiene el método HTTP utilizado por el navegador para solicitar la página web. Por ejemplo, GET o POST.
    QUERY_STRING: Si la solicitud incluye datos enviados a través de un formulario HTML utilizando el método GET, esta variable contiene los datos.
    CONTENT_TYPE: Si la solicitud incluye datos enviados a través de un formulario HTML utilizando el método POST, esta variable contiene el tipo MIME de los datos enviados
    CONTENT_LENGTH: Si la solicitud incluye datos enviados a través de un formulario HTML utilizando el método POST, esta variable contiene la longitud de los datos enviados.
    SCRIPT_NAME: Esta variable contiene el nombre del script CGI que se está ejecutando.
    SERVER_NAME: Esta variable contiene el nombre del servidor web que está ejecutando el script CGI.
    SERVER_PORT: Esta variable contiene el número de puerto que el servidor web está utilizando para la conexión.
*/
 void CGI::createCGIEnvironment(const Request &ar_request)
{
    std::string cgi_executable;



    this->_m_env["REQUEST_METHOD"] = ar_request.getMethod();
    this->_m_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_m_env["SERVER_NAME"] =  ar_request.getServerName();
    this->_m_env["SERVER_PORT"] = ar_request.getPort();
    this->_m_env["PATH_INFO"] = ar_request.getRequestFile();//
    this->_m_env["PATH_TRANSLATED"] = ar_request.getRequestFile();//
	this->_m_env["SCRIPT_NAME"] = cgi_executable;//
    
    if (ar_request.getMethod() == "POST")
    {
        this->_m_env["CONTENT_LENGTH"] = "";
        this->_m_env["CONTENT_TYPE"] = "";
    }
    else if (ar_request.getMethod() == "GET")
        this->_m_env["QUERY_STRING"] = "";

    //Opcionales
    this->_m_env["GATEWAY_INTERFACE"] = std::string("CGI/1.1");
    this->_m_env["SCRIPT_FILENAME"] = this->_cgi_path;
    this->_m_env["REQUEST_URI"] = this->_cgi_path;//
    this->_m_env["REDIRECT_STATUS"] = "200";
	this->_m_env["SERVER_SOFTWARE"] = "AMANIX";

}