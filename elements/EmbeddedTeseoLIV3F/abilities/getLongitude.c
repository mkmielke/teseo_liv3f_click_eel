	float longitude;

	ATMO_TeseoLIV3F_GetLongitude( &longitude );
	ATMO_CreateValueFloat( out, longitude );

	return ATMO_Status_Success;
