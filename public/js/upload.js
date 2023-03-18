function handleFileSelect()
{
	var fileInput = document.getElementById("file");
	var fileName = fileInput.files[0].name;
	var fileFormat = fileName.split(".").pop();
	var form = document.getElementById("upload-form");
	form.action = "uploads/" + fileName + "." + fileFormat;
}
