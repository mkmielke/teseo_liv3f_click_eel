	uint8_t numSatellites;

	ATMO_TeseoLIV3F_GetNumSatellites( &numSatellites );
	ATMO_CreateValueUnsignedInt( out, numSatellites );

	return ATMO_Status_Success;
