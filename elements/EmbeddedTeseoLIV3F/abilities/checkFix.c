	bool posFix;

	ATMO_TeseoLIV3F_CheckFix( &posFix );
	ATMO_CreateValueBool( out, posFix );

	return ATMO_Status_Success;
