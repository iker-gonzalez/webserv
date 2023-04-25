#!/usr/bin/python3

# Import modules for CGI handling 
import cgi 
# Create instance of FieldStorage 
form = cgi.FieldStorage()

# Get data from fields
username = form.getvalue('username')
password = form.getvalue('password')
#username = "ABCS"
#password = "ZLA"
#print(form.keys())
if form:
	content = [
		"<html>",
		"<head>",
		"<title>Hello - Second CGI2 Program</title>",
		"</head>",
		"<body>",
		"<h2>Hello {username}, your password is {password}</h2>".format(username = username, password = password),
		"</body>",
		"</html>"
	]
else:
	content =  ["<html>",
		"<head>",
		"<title>Hello - Second CGI Program</title>",
		"</head>",
		"<body>",
		"<h2>Hello {username}, your password is {password} </h2>".format(username = username, password = password),
		"</body>",
		"</html>"
	]

contentLength = sum(len(s) for s in content)
print("HTTP/1.1 200 OK")
print("Content-Length: {clength}".format(clength = contentLength))
print("Content-type: text/html\r\n\r\n")

for s in content:
	print(s)