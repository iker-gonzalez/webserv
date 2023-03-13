#ifndef CGI_HPP
#define CGI_HPP

#include <map>
#include <string>
class Request;
class Location;

class CGI
{
    public:
        CGI();
        ~CGI();
        CGI(const CGI& other);


        //Gettters
        std::map<std::string, std::string> getEnvMap()const;
        

        //Get information from Request
        //Prepare PIPE
        //Prepare Enviroment variable
        //Execute binary
        void    createCGIEnvironment(const Request &ar_request,
                                    const Location &ar_location);
        void    prepareArgForExecve();
        bool    setupPipes();
        std::string    execute(); 
   
    private:
        // Enviromnet variables 
        std::map<std::string, std::string> _m_env;
    public:
        // Pipes
        int pipe_in[2];
        int pipe_out[2];

        // CGI process pid after fork
        pid_t _pid_CGI;

        // argv[3] for the execve argument
        // argv[0] -> Executable
        // argv[1] -> Path
        // argv[2] -> NULL
        // env_char -> Enviroment
        char**  	_argv;
        char **     _env_char;


};

#endif