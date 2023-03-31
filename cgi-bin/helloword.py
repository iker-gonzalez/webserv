#!/usr/bin/python3

# Import modules for CGI handling 
import cgi 

# Create instance of FieldStorage 
form = cgi.FieldStorage()

# Get data from fields
username = form.getvalue('username')
password = form.getvalue('password')

# Descomenta esto y mira que tengas python3 installado. A ver si recoge la información el cgi.FieldStorage()
# echo "username=foo&password=bar" | python3 helloword.py

# Print the form data to the console
if form:
    for key in form.keys():
        print("{}: {}".format(key, form.getvalue(key)))
else:
    print("No form data found")


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
