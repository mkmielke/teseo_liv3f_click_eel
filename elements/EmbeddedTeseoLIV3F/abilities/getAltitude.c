	float altitude;

	ATMO_TeseoLIV3F_GetAltitude( &altitude );
	ATMO_CreateValueFloat( out, altitude );

	return ATMO_Status_Success;
