#include "../includes/CGI.hpp"
#include "../includes/Request.hpp"
#include "../includes/Location.hpp"
#include <stdlib.h>
#include <string.h>	//for strdup
#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <unistd.h>

CGI::CGI()
{

    
}

CGI::~CGI()
{
   // if (_m_env.size())
   // {
   //     free(_argv[0]);
   //     free(_argv[1]);
   //     for (int i = 0; i < _m_env.size(); i++)
   //         free(_env_char[i]);
   //     free(_env_char);
   // }

}

CGI::CGI(const CGI &other)
{
   
}

std::map<std::string, std::string> CGI::getEnvMap() const
{
    return _m_env;
}

bool CGI::setupPipes()
{
  // if (pipe(pipe_in) < 0)
  // {
  //     //_code = 500;
  //     return false;
  // }
    if (pipe(pipe_out) < 0)
    {
        //_code = 500;
      //  close(pipe_in[0]);
		//close(pipe_in[1]);
        return false;
    }
    return true;
}
std::string CGI::execute()
{

    	std::string res = "";
    _pid_CGI = fork();
	if (_pid_CGI == 0)
	{
        close(pipe_out[0]);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_out[1]);


       //FILE	*fIn = tmpfile();
	   //FILE	*fOut = tmpfile();
	   //pipe_in[0] = fileno(fIn);
	   //pipe_out[1] = fileno(fOut);

       //dup2(pipe_in[0], STDIN_FILENO);
		//dup2(pipe_out[1], STDOUT_FILENO);

		//dup2(pipe_in[0], STDIN_FILENO);
		//dup2(pipe_out[1], STDOUT_FILENO);
		//close(pipe_in[0]);
		//close(pipe_in[1]);
		//close(pipe_out[0]);
		//close(pipe_out[1]);
		bool status = execve(this->_argv[0], this->_argv, this->_env_char);
		exit(status);
	}
	else if (this->_pid_CGI > 0)
    {
        close(pipe_out[1]);

        char buffer[1024];
		int  readBytes;
		while ((readBytes = read(pipe_out[0], buffer, 1024)) > 0)
		{
			res.append(buffer, readBytes);
		}

        if (readBytes == -1)
		{
			std::cerr << "Couldn't read from CGI " << _argv[0] << ": " << strerror(errno) << std::endl;
			close(pipe_out[0]);
			return "";
		}
        close(pipe_out[0]);
		waitpid(_pid_CGI, NULL, WNOHANG);
        // continue process
    }
	else
	{
        std::cerr << "Fork failed" << std::endl;
        return NULL;
	//	error_code = 500;
	}
    //std::cerr << "-------------EXECUTE CGI---------------" << std::endl;
    //for (size_t i = 0; _env_char[i]; i++)
	//	std::free(_env_char[i]);
	//for (size_t i = 0; _argv[i]; i++)
	//	std::free(_argv[i]);
    //std::cerr << "-------------CGI RESPONSEEE---------------" << std::endl;
    //std::cerr << res << std::endl;

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


    // The following environment variables are not request-specific and are set for all requests:
	//this->_m_env["SERVER_SOFTWARE"] = "AMANIX";
   //this->_m_env["SERVER_NAME"] =  ar_request.getServerName();
   //this->_m_env["GATEWAY_INTERFACE"] = std::string("CGI/1.1");

   //// The following environment variables are specific to the request being fulfilled by the gateway program:
   //this->_m_env["SERVER_PROTOCOL"] = "HTTP/1.1";
   //this->_m_env["SERVER_PORT"] =  std::to_string(ar_request.getPort());
   //this->_m_env["REQUEST_METHOD"] = ar_request.getMethod();
   //this->_m_env["PATH_INFO"] = ""; //ar_request.getRequestFile();//
   //this->_m_env["PATH_TRANSLATED"] = ""; //ar_request.getRequestFile();//
   ////cgi-bin/*.py
   //this->_m_env["SCRIPT_NAME"] = ar_location.getPath() + "/" + cgi_executable;//
	//
   /// this->_m_env["DOCUMENT_ROOT"] = ar_location.getRoot();    
   //this->_m_env["REDIRECT_STATUS"] = "200";
   //
   //if (ar_request.getMethod() == "POST")
   //{
   //    this->_m_env["CONTENT_LENGTH"] = ar_request.getBody().length();
   //    std::map<std::string, std::string>	m_requestHeaders =
   //                                            ar_request.getHeaders();

   //    this->_m_env["CONTENT_TYPE"] = m_requestHeaders["content-type"];
   //}
   //else if (ar_request.getMethod() == "GET")
   //    this->_m_env["QUERY_STRING"] = "";

   ////Opcionales
   //this->_m_env["SCRIPT_FILENAME"] = cgi_executable;
   //this->_m_env["REQUEST_URI"] = ar_request.getRequestFile();//

   // std::cerr << "-----REQUESTT----" << std::endl;
    //std::cerr << ar_request << std::endl;

    if (ar_request.getMethod() == "GET")
        _m_env["QUERY_STRING"] = "";

    if (ar_request.getMethod() == "POST")
    {
        std::map<std::string, std::string>	m_requestHeaders =
                                           ar_request.getHeaders();
        _m_env["CONTENT_LENGTH"] =  m_requestHeaders["content-length"];
        _m_env["CONTENT_TYPE"] =  m_requestHeaders["content-type"];
       // _m_env["UPLOAD_DIRECTORY"] =  
    }

    std::string script_name = ar_request.getRequestFile() + "/" +ar_location.getIndex();
   if (ar_request.getRequestFile()[0] == '/')
        script_name = script_name.substr(1,script_name.length() );
    
    this->_m_env["REQUEST_URI"] = ar_location.getCgiPass();
   this->_m_env["SCRIPT_NAME"] = script_name;
   this->_m_env["REQUEST_METHOD"] = ar_request.getMethod();

     std::map<std::string, std::string>::iterator it;
     std::map<std::string, std::string>::iterator it_end = _m_env.end();
   // for (it = _m_env.begin(); it != it_end; it++)
     //   std::cout << (*it).first << " : " << (*it).second << std::endl;
    prepareArgForExecve();

}
void CGI::prepareArgForExecve()
{
    _env_char = (char**)malloc(sizeof(char *) * _m_env.size() + 1);
    if (!_env_char)
        return ;

    std::map<std::string, std::string>::const_iterator it;
    std::map<std::string, std::string>::const_iterator it_end = _m_env.end();
    int i = 0;
    for ( it = _m_env.begin(); it != it_end; it++ )
    {
        std::string env_varible = (*it).first + "=" + (*it).second;
 //       std::cout << "ENV_var: " << env_varible << std::endl;
        _env_char[i] = strdup(env_varible.c_str());

        i++;
    }
    _env_char[i] == NULL; 
    _argv = (char **)malloc(sizeof(char *) * 3);
    if (!_argv)
        return ;
	_argv[0] = strdup(_m_env["REQUEST_URI"].c_str());
    _argv[1] = strdup(_m_env["SCRIPT_NAME"].c_str());
	_argv[2] = NULL;


}
