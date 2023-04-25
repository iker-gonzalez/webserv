#!/usr/bin/env python
import cgi
import cgitb
import cgitb
import sys
import datetime


cgitb.enable()

# Set the path to the directory where the file to be deleted is located

for line in sys.stdin:
    print(line)
# Create the form object
form = cgi.FieldStorage()

# curl -X DELETE -d '{"delete": "descarga"}' localhost:8080/cgi-bin/delete.py
print("HTTP/1.1 200 OK")
filename = form.getvalue('username')
# Check if the 'delete' parameter is present in the form
if "username"  in form:
    # Get the name of the file to be deleted from the form
    #filename = form["delete"].value
   	# Retrive env from UPLOAD_DIRECTORY
	uploadDir = os.environ['UPLOAD_DIRECTORY']
	if (uploadDir == ""):
		uploadDir = './uploads'
	# Add / at the end if none is present
	if (uploadDir[-1] != '/'):
		uploadDir += '/'
    # Combine the file path and filename to get the full path to the file
    file_to_delete = os.path.join(uploadDir, filename)
   
    try:
        # Attempt to delete the file
        os.remove(file_to_delete)
        print("Content-type: text/html\n\n")
        print("<html><head><title>File Deleted</title></head>")
        print("<body><h1>File Deleted Successfully</h1></body></html>")
    except Exception as e:
        # If an error occurs during the delete operation, display an error message
        print("Content-type: text/html\n\n")
        print("<html><head><title>Error</title></head>")
        print("<body><h1>Error Deleting File</h1>")
        print("<p>An error occurred while attempting to delete the file:</p>")
        print("<pre>{}</pre>".format(e))
        print("</body></html>")
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
