	float latitude;

	ATMO_TeseoLIV3F_GetLatitude( &latitude );
	ATMO_CreateValueFloat( out, latitude );

	return ATMO_Status_Success;
