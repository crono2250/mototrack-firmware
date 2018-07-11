/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */

DWORD get_fattime(void)
{
	DWORD tmr;


//	_DI();
	/* Pack date and time into a DWORD variable */
	tmr =	  (((DWORD)2010 - 1980) << 25)
			| ((DWORD)11 << 21)
			| ((DWORD)1 << 16)
			| (WORD)(0 << 11)
			| (WORD)(0 << 5)
			| (WORD)(0 >> 1);
//	_EI();

	return tmr;
}

static FRESULT scan_files(
	char* path		/* Pointer to the path name working buffer */
) CYREENTRANT
{
	DIR dirs;
	FRESULT res;
	BYTE i;
	char *fn;

	if ((res = f_opendir(&dirs, path)) == FR_OK) {
		i = strlen(path);
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
			if (_FS_RPATH && Finfo.fname[0] == '.') continue;
#if _USE_LFN
			fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR) {
				acc_dirs++;
				*(path+i) = '/'; strcpy(path+i+1, fn);
//				res = scan_files(path);
				*(path+i) = '\0';
				if (res != FR_OK) break;
			} else {
/*				xprintf("%s/%s\n", path, fn);	*/
				acc_files++;
				acc_size += Finfo.fsize;
			}
		}
	}

	return res;
}
