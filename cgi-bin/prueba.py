#!/usr/bin/env python

import cgi
import os
import sys

# Set the content type to HTML
print("Content-type:text/html\r\n\r\n")

# Read form data from stdin
form_data = sys.stdin.read()

# Parse the form data
form = cgi.FieldStorage(
    fp=sys.stdin,
    headers={
        'Content-Type': os.environ['CONTENT_TYPE'],
        'Content-Length': str(len(form_data))
    },
    environ={
        'REQUEST_METHOD': os.environ['REQUEST_METHOD'],
        'QUERY_STRING': '',
        'CONTENT_TYPE': os.environ['CONTENT_TYPE'],
        'CONTENT_LENGTH': str(len(form_data))
    })

# Get data from fields
username = form.getvalue('username')
password = form.getvalue('password')

# Display the form data
print("<html>")
print("<body>")
print("<p>Username: {}</p>".format(username))
print("<p>Password: {}</p>".format(password))
print("</body>")
print("</html>")