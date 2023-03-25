#!/usr/bin/python

# Import modules for CGI handling 
import cgi 

# Create instance of FieldStorage 
form = cgi.FieldStorage()

# Get data from fields
username = form.getvalue('username')
password = form.getvalue('password')

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