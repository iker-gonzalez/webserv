#!/usr/bin/python3

import datetime
import cgi

# curl -X GET localhost:8080/cgi-bin/time.py
while (1):
    {
        
    }
print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n\r\n")
print("<html>")
print("<head>")
print(datetime.datetime.strftime(datetime.datetime.now(), "<h1>  %H:%M:%S </h1>"))