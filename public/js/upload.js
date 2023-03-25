

function handleFileSelect()
{
	var fileInput = document.getElementById("file");
	var fileName = fileInput.files[0].name;
	var fileFormat = fileName.split(".").pop();
	var fileNameWithoutFormat = fileName.substring(0, fileName.lastIndexOf("."));
	var form = document.getElementById("upload-form");
	form.action = "uploads/" + fileNameWithoutFormat + "." + fileFormat;

	// create input element to hold file content
	var fileContentInput = document.createElement("input");
	fileContentInput.type = "hidden";
	fileContentInput.name = "file_content";
	fileContentInput.value = "";

	// append input element to form
	form.appendChild(fileContentInput);

	// read file content into input element value
	var reader = new FileReader();
	reader.onload = function(event){
		fileContentInput.value = event.target.result;
		console.log(event.target.result); // print file content to console
	};
	reader.readAsText(fileInput.files[0]);
}