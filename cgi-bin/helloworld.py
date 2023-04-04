#!/usr/bin/python3

# Import modules for CGI handling 
import os 
import sys 
import cgi 
import cgitb; cgitb.enable()

# Create instance of FieldStorage 
form = cgi.FieldStorage()

if form:
	for key in form.key():
		print("{}: {}".format(key, form.getvalue(key)))
else
	print("No from data found")

# Get data from fields
username = form.getvalue("username")
#username = "gfdgd"
password = form.getvalue("password")
#password = "Sloa"

#print(form.keys())

content = [
	"<html>",
	"<head>",
	"<title>Hello - Second CGI Program</title>",
	"</head>",
	"<body>",
	"<h2>Hello {username}, your password is {password}</h2>".format(username = username, password = password),
	"</body>",
	"</html>"
]

contentLength = sum(len(s) for s in content)

print("Content-Length: {clength}".format(clength = contentLength))
print("Content-type: text/html\r\n\r\n")

for s in content:
	print(s)
