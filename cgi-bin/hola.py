#!/usr/bin/env python
import cgi
import cgitb
import cgitb
import sys
import datetime
# curl -X DELETE -d 'username=borrar' localhost:8080/cgi-bin/hola.py
# Create the form object
form = cgi.FieldStorage()

# curl -X DELETE -d '{"delete": "descarga"}' localhost:8080/cgi-bin/delete.py
print("HTTP/1.1 200 OK")
uploadDir = "./uploads/"
filename = form.getvalue('username')
# Check if the 'delete' parameter is present in the form
# filename2 = uploadDir + filename
filename = "public/content/uploads/borrar.txt"

if filename:
    print("Content-type: text/html\n\n")
    print("<html><head><title>Delete File</title></head>")
    print("<body><h1>Delete File</h1>")
    #print("<p>Please enter the name {username} of the file to be deleted:</p>").format(username = filename)
    print("<form method='post'>")
    print(filename)
    print("<input type='text' name='delete' id='delete' required>")
    print("<input type='submit' value='Delete'>")
    print("</form></body></html>")
    try:
        os.remove(filename)
    except Exception as e:
        print("Content-type: text/html\n\n")
        # If an error occurs during the delete operation, display an error message
else:
    # If the 'delete' parameter is not present in the form, display the delete form
    print("Content-type: text/html\n\n")
    print("<html><head><title>Delete File</title></head>")
    print("<body><h1>Delete File</h1>")
    print("<p>Please enter the name of the file to be deleted:</p>")
    print("<form method='post'>")
    print("<input type='text' name='delete' id='delete' required>")
    print("<input type='submit' value='Delete'>")
    print("</form></body></html>")
