#include "../includes/CGI.hpp"
#include "../includes/Request.hpp"
#include "../includes/Location.hpp"
#include <stdlib.h>
#include <string.h>	//for strdup
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> //??N Borrar
CGI::CGI() : _argv(NULL)
{

    
}

CGI::~CGI()
{

}

CGI::CGI(const CGI &other)
{
        pipe_out[0] = other.pipe_out[0];
        pipe_out[1] = other.pipe_out[1];
        pipe_in[1] = other.pipe_in[1];
        pipe_in[0] = other.pipe_in[0];
        _pid_CGI = other._pid_CGI;
}

std::map<std::string, std::string> CGI::getEnvMap() const
{
    return _m_env;
}

bool CGI::setupPipes(std::string &content)
{
     if (!content.empty())
    {
        if (pipe(pipe_in) < 0)
            return false;
    }
    if (pipe(pipe_out) < 0)
    {
        close(pipe_in[0]);
	    close(pipe_in[1]);
        return false;
    }
    return true;
}
std::string CGI::execute(std::string &content)
{
    std::string res = "";
    _pid_CGI = fork();
	if (_pid_CGI == 0)
	{
        close(pipe_out[0]);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[1]);

        if (!content.empty())
		{
			close(pipe_in[1]);
			dup2(pipe_in[0], STDIN_FILENO);
            close(pipe_in[0]); 
		}
        prepareArgForExecve();
        int status;
		status = execve(_argv[0], _argv, _env_char);
        exit(status);
	}
	else if (_pid_CGI > 0)
    {
        close(pipe_out[1]);
		if (!content.empty())
		{
			close(pipe_in[0]);
		}
    }
	else
	{
        std::cerr << "Fork failed" << std::endl;
        return NULL;
	}
    return res;
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
void CGI::createCGIEnvironment(const Request &ar_request, const Location& ar_location)
{
    const std::string cgi_executable = ar_location.getIndex();

    if (ar_request.getMethod() == "GET")
        _m_env["QUERY_STRING"];

    if (ar_request.getMethod() == "POST")
    {
        std::cout << "Executing POST CGI"<< std::endl;
	    std::string ConLenght = ar_request.getHeader("Content-Length:");
	    std::string ConType = ar_request.getHeader("Content-Type:");
    
	    ConLenght = ConLenght.substr(1, ConLenght.size() - 2);
	    ConType = ConType.substr(1, ConType.size() - 2);
        _m_env["CONTENT_LENGTH"] = ConLenght;
        _m_env["CONTENT_TYPE"] = ConType;
        _m_env["UPLOAD_DIRECTORY"] =  "public/content/uploads"; 
    }
    if (ar_request.getMethod() == "DELETE")
    {
        std::string ConLenght = ar_request.getHeader("Content-Length:");
	    std::string ConType = ar_request.getHeader("Content-Type:");
    
	    ConLenght = ConLenght.substr(1, ConLenght.size() - 2);
	    ConType = ConType.substr(1, ConType.size() - 2);
        _m_env["CONTENT_LENGTH"] = ConLenght;
        _m_env["CONTENT_TYPE"] = ConType;
        _m_env["UPLOAD_DIRECTORY"] =  "public/content/uploads"; 
    }
    std::string script_name = ar_request.getRequestFile();
    // If the request file is a .py dont apend the index
    int pos = script_name.find(".py");
    if (pos != ((int)script_name.length() - 3))
    {
        script_name += "/" + ar_location.getIndex();
    }
   if (ar_request.getRequestFile()[0] == '/')
        script_name = script_name.substr(1,script_name.length() -1);


    this->_m_env["REQUEST_URI"] = ar_location.getCgiPass();
    this->_m_env["SCRIPT_NAME"] = script_name;
    this->_m_env["REQUEST_METHOD"] = ar_request.getMethod();
}
void CGI::prepareArgForExecve()
{
    _env_char = (char**)malloc(sizeof(char *) * (_m_env.size() + 1));
    if (!_env_char)
     return ;
    std::map<std::string, std::string>::const_iterator it;
    std::map<std::string, std::string>::const_iterator it_end = _m_env.end();
    int i = 0;
    for ( it = _m_env.begin(); it != it_end; it++ )
    {
        std::string env_varible = (*it).first + "=" + (*it).second;
        _env_char[i] = strdup(env_varible.c_str());
        i++;
    }
    _env_char[i] = 0; 
    _argv = (char **)malloc(sizeof(char *) * 3);
    if (!_argv)
        return ;
    else
    {
	    _argv[0] = strdup(_m_env["REQUEST_URI"].c_str());
        _argv[1] = strdup(_m_env["SCRIPT_NAME"].c_str());
	    _argv[2] = NULL;
    }
    return;
}
CGI &CGI::operator=(const CGI &rhs){
        pipe_out[0] = rhs.pipe_out[0];
        pipe_out[1] = rhs.pipe_out[1];
        pipe_in[1] = rhs.pipe_in[1];
        pipe_in[0] = rhs.pipe_in[0];
        _pid_CGI = rhs._pid_CGI;
        return *this;
    }