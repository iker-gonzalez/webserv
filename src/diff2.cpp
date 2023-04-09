	if(!(isMethodAllowed(request.getMethod(),_server.getMethods())))
	{
		_status_code = 403;
		return (1);
	}
	if (!(isClientSizeAllowed(_server.getClientSize())))
	{
		_status_code = 413;
		