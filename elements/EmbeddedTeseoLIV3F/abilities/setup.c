	ATMO_TeseoLIV3F_Config_t config;
	config.i2cDriverInstance = ATMO_PROPERTY( undefined, i2cDriverInstance );
	config.gpioDriverInstance = ATMO_PROPERTY( undefined, gpioDriverInstance );
	config.wupPin = ATMO_PROPERTY( undefined, wupPin );
	config.rstPin = ATMO_PROPERTY( undefined, rstPin );
	ATMO_TeseoLIV3F_Init( &config );

	return ATMO_Status_Success;
