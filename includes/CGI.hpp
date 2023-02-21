#ifndef CGI_HPP
#define CGI_HPP

#include <map>
#include <string>
class CGI
{
    public:
        CGI();
        ~CGI();
        CGI(const CGI& other);


        

        //Get information from Request
        //Prepare PIPE
        //Prepare Enviroment variable
        //Execute binary
        void createCGIEnvironment();
    
    private:
        // Enviromnet variables 
        std::map<std::string, std::string> _m_env;

        // Pipes
        int pipe_in[2];
        int pipe_out[2];

        // argv[3] for the execve argument
        // argv[0] -> Executable
        // argv[1] -> Path
        // argv[2] -> NULL
        // env_char -> Enviroment
        char*[3]	_argv;
        char **     _env_char;


}

#endif