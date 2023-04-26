#!/usr/bin/python3
import cgi, sys, os, os.path


form = cgi.FieldStorage()

filename = form.getvalue('delete')

#curl -X DELETE -d 'delete=borrar.txt' localhost:8080/cgi-bin/deleteCGI.py

if 'delete' in form:
	print("HTTP/1.1 404 ")
	print ("Content-type:text/html\r\n")
	print ("<html>")
	print ("<head>")
	print ("<title>DELETE CGI EXECUTE</title>")
	print ("</head>")
	print ("</html>")
else:
	print("HTTP/1.1 200 ")
	print ("Content-type:text/html\r\n")
	print ("<html>")
	print ("<head>")
	print ("<title>DELETE ERROR</title>")
	print ("</head>")
	print ("</html>")