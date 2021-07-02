#include "sd_io_logging.h"
#include "main.h" // TODO think about removing this to make it more general
#include "fatfs.h"
#include "utility_umweltspaeher.h"

void createNewLogFile(char* name){

	
}

// If file exist it return true otherwise false
// TODO // if (fr == FR_NO_FILE)
uint8_t does_file_exist (
    FIL* fp,
    const char* path // Filename. If file exist it return true
)
{
    FRESULT fr;
    // If file exist it will be oppened. If not it will be created
    fr = f_open(fp, path, FA_OPEN_EXISTING);
    if (fr == FR_OK) {
			f_close(fp);
			return 1;
    }
		f_close(fp);
		return 0;
}


FRESULT open_append (
    FIL* fp,
    const char* path // Filename. If file exist it will be oppened. If not it will be created
)
{
    FRESULT fr;

    // If file exist it will be oppened. If not it will be created
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        // Go to the end of the file and append the data
        fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}

void testSdFat(void){
	FATFS myFAT;
	FIL myFile;
	uint8_t myData[150] = "HelloWorld1234_HelloWorld1234_HelloWorld1234_HelloWorld1234_HelloWorld1234_HelloWorld1234_HelloWorld1234_HelloWorld1234_HelloWorld1234_HelloWorld1234_";
	UINT byteCount;
  char dataToPrint[30];
	print_uart2("BEGIN - SD FAT TEST \r\n");
	
	int mount_id = f_mount(&myFAT, SD_Path, 1);
	sprintf(dataToPrint, "mount_id: %d\r\n", mount_id);
	print_uart2(dataToPrint);
	
	if(f_mount(&myFAT, SD_Path, 1) == FR_OK){		
		print_uart2("Mount SD OK\r\n");		
		// For time mesaurement
	  //unsigned long t1 = DWT->CYCCNT;
	  //unsigned long t2 = 0;
		__disable_irq(); // Otherwise There will be writing errors randomly!!!
		f_open(&myFile, "test1.txt\0", FA_WRITE|FA_CREATE_ALWAYS);
		//for (int i=0; i<10000; i++){
			//sprintf(dataToPrint, "Write NOW\r\n");
	    //HAL_UART_Transmit(&huart2, (uint8_t *) dataToPrint, 15, 50);
		f_write(&myFile, myData, 150, &byteCount);
			//}
		}
		f_close(&myFile);
		//t2 = DWT->CYCCNT;	
	  //unsigned long diff = t2 - t1;
	  //sprintf(dataToPrint, "%lu Takte\r\n", diff);
	  //float spiTime = diff/168000000.0;
	  //printf("Time = %f\r\n", spiTime);
		//HAL_UART_Transmit(&huart2, (uint8_t *) dataToPrint, 100, 50);
		__enable_irq();
		print_uart2("DONE \r\n");
	}
