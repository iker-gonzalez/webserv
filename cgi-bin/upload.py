#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Get filename here
fileitem = form['filename']

# Test if the file was uploaded
#if fileitem.filename:
#   open(os.getcwd() + '/cgi-bin/tmp/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
#   message = 'The file "' + os.path.basename(fileitem.filename) + '" was uploaded to ' + os.getcwd() + '/cgi-bin/tmp'
#else:
#   message = 'Uploading Failed'
print("HTTP/1.1 200 OK")
print ("Content-type:text/html\r\n")
print("<html>")
print("<head>")
print("<h1> " + fileitem.filename + " </h1>")
