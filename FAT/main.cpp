#include"FAT32.h"

int main() {
	BYTE BootSector[512];
	BYTE FAT1[512];
	BYTE RDECT[512];
	BYTE DATA[512];
	
	string namePath;
	cout << "Nhap ten dau dia: ";
	cin >> namePath;
	string tom = "\\\\.\\" + namePath + ":";
	wstring tmp = strToWstr(tom);
	LPCWSTR disk = tmp.c_str();
	FAT32 Miku(disk);
	ReadSector(Miku.driver, (long)0, BootSector);
	Miku.print(BootSector, 512);

	int FAT1_pos = Miku.BPB_RsvdSecCnt * Miku.BPB_BytesPerSec;
	int FAT2_pos = (Miku.BPB_RsvdSecCnt + Miku.BPB_FATSz32) * Miku.BPB_BytesPerSec;
	
	ReadSector(Miku.driver, FAT1_pos, FAT1);
	std::cout << Miku.read(FAT1, 512) << std::endl;
	ReadSector(Miku.driver, FAT2_pos+Miku.BPB_FATSz32*512, RDECT);
	std::cout << Miku.read(RDECT, 512);
	std::cout << std::endl;
	std::vector<string> entry = convertToEntry(convertToVector(RDECT, 512), 0);
	std::cout << std::endl;
	std::vector<
		std::pair<
		std::vector<string>, std::vector<std::vector<std::string>>>> entries = Miku.splitEntries(convertToVector(RDECT, 512));
	for (int j(0); j < entries.size(); j++) {
		for (int i(0); i < entries[j].second.size(); i++) {
			Miku.printEntry(entries[j].second[i]);
			std::cout << std::endl;
		}
		Miku.printEntry(entries[j].first);
		std::cout << std::endl;
		Miku.readNameEntry(entries[j]);
	}
	return 0;
}

//#include <windows.h>
//#include <iostream>
//#include <iomanip>
//#include <sstream>
//#include <list>
//#include <stack>
//#include <string>
//using namespace std;
//int ReadSector(LPCWSTR  drive, int readPoint, BYTE* sector)
//{
//	int retCode = 0;
//	DWORD bytesRead;
//	HANDLE device = NULL;
//
//	device = CreateFile(drive,    // Drive to open
//		GENERIC_READ,           // Access mode
//		FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
//		NULL,                   // Security Descriptor
//		OPEN_EXISTING,          // How to create
//		0,                      // File attributes
//		NULL);                  // Handle to template
//
//	if (device == INVALID_HANDLE_VALUE) // Open Error
//	{
//		printf("CreateFile: %u\n", GetLastError());
//		return 1;
//	}
//	/*const int MAX_USB_GB = 64;
//	VOLUME_DISK_EXTENTS vde = { 0 };
//	DeviceIoControl(
//		device,
//		IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
//		NULL,
//		0,
//		(LPVOID)&vde,
//		(DWORD)sizeof(VOLUME_DISK_EXTENTS),
//		&bytesRead, NULL
//	);
//	if (vde.Extents[0].ExtentLength.QuadPart > LONGLONG(MAX_USB_GB * 1000000000LL))
//	{
//
//		printf("USB too large\n");
//
//		printf("Use < %d GB", MAX_USB_GB);
//
//		CloseHandle(device);
//
//		return -2;
//
//	}*/
//
//	SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read
//
//	if (!ReadFile(device, sector, 512, &bytesRead, NULL))
//	{
//		printf("ReadFile: %u\n", GetLastError());
//	}
//	else
//	{
//		printf("Success!\n");
//	}
//}
//
//
////int asciiToHex(uint8_t* dest, char* str) {
////
////	int destLength = strlen(str) / 2; //Get length of dest
////	for (int i = 0; i < destLength; i++) {
////		char digit[3];
////		digit[0] = *(str++);
////		digit[1] = *(str++);
////		digit[2] = 0x00; //Null terminate
////		dest[i] = (uint8_t)(strtol(digit, NULL, 16));
////	}
////
////	return destLength;
////}
//
//string hexStr(const uint8_t* data, int len)
//{
//	stringstream ss;
//	ss << hex;
//	if (ES_UPPERCASE)
//		ss << uppercase;
//	for (int i(0); i < len; ++i)
//	{
//		if (i % 16 == 0)
//		{
//			ss << "\n";
//		}
//		ss << setw(2) << setfill('0') << static_cast<int>(data[i]);
//		ss << " ";
//	}
//
//	return ss.str();
//}
//
//
//
//class FAT32
//{
//private:
//	HANDLE device;
//	int BPB_BytesPerSec; // Số bytes/sector (off set 0 - 3 bytes)  
//	int BPB_SecPerClus; // Số sector/cluster (off set 3 - 8 bytes)
//	int BPB_RsvdSecCnt; // Số sector để dành (khác 0)(Số sector trước bảng FAT) (off set B - 2 bytes)
//	int BPB_NumFATs; // Số bảng FAT (off set 10 - 1 bytes)
//	int BPB_RootEntCnt; // FAT12, FAT16: Số entry trong bảng RDET - FAT32: Có giá trị là 0 (off set 11 - 2 bytes)
//	int BPB_TotSec32; // Số sector trong volume (off set 20 - 4 bytes)
//	int BPB_FATSz32; // Số sector trong 1 bảng FAT (off set 24 - 4 bytes)
//	int BPB_RootClus;//Rootcluster location in the fat32 image
//	int BS_DrvNum; // Chỉ số cluster đầu tiên trong RDET (off set 2C - 4 bytes)
//
//	int32_t RootDirSectors = 0; // 
//	int32_t FirstDataSector = 0;//Where the first data sector exists in the fat32 file image.
//	int32_t FirstSectorofCluster = 0;//First sector of the data cluster exists at point 0 in the fat32 file image.
//
//	int32_t currentDirectory;//Current working directory
//public:
//	void getInformation(BYTE* sector, int len);
//	int hexaToDecimal(string hex_)
//	{
//		int len = hex_.size();
//		int base = 1;
//		int dec_ = 0;
//		for (int i = len - 1; i >= 0; i--) {
//			if (hex_[i] >= '0' && hex_[i] <= '9') {
//				dec_ += (int(hex_[i]) - 48) * base;
//
//				// incrementing base by power
//				base = base * 16;
//			}
//
//			// if character lies in 'A'-'F' , converting
//			// it to integral 10 - 15 by subtracting 55
//			// from ASCII value
//			else if (hex_[i] >= 'A' && hex_[i] <= 'F') {
//				dec_ += (int(hex_[i]) - 55) * base;
//
//				// incrementing base by power
//				base = base * 16;
//			}
//		}
//		return dec_;
//	}
//	list<string> convertToList(const uint8_t* data, int len)
//	{
//		list<string> offset;
//		for (int i(0); i < len; ++i)
//		{
//			stringstream ss;
//			ss << hex << uppercase;
//			ss << setw(2) << setfill('0') << static_cast<int>(data[i]);
//			offset.push_back(ss.str());
//		}
//		int j = 0;
//		/*for (const auto& i : offset)
//		{
//			if (j % 16 == 0)
//			{
//				cout << "\n";
//			}
//			cout << i << " ";
//			j++;
//		}*/
//		return offset;
//	}
//	string reverseTrav(list<string> temp, int i, int nBytes);
//};
//
//string FAT32::reverseTrav(list<string> temp, int i, int nBytes)
//{
//	string str = "";
//	list<string>::iterator itr = temp.begin();
//	advance(itr, i);
//	for (; nBytes != 0; --itr, --nBytes)
//	{
//		str += *itr;
//	}
//	return str;
//}
//
//
//void FAT32::getInformation(BYTE* sector, int len)
//{
//	BPB_BytesPerSec = hexaToDecimal(reverseTrav(convertToList(sector, len), 12, 2));
//	cout << "So bytes/sector: " << BPB_BytesPerSec << endl;
//	BPB_SecPerClus = hexaToDecimal(reverseTrav(convertToList(sector, len), 13, 1));
//	cout << "So sectors/cluster: " << BPB_SecPerClus << endl;
//	BPB_RsvdSecCnt = hexaToDecimal(reverseTrav(convertToList(sector, len), 15, 2));
//	cout << "So sectors truoc bang FAT: " << BPB_RsvdSecCnt << endl;
//	BPB_NumFATs = hexaToDecimal(reverseTrav(convertToList(sector, len), 16, 1));
//	cout << "So bang FAT: " << BPB_NumFATs << endl;
//	BPB_RootEntCnt = hexaToDecimal(reverseTrav(convertToList(sector, len), 17, 1));
//	cout << "So entry trong bang RDET: " << BPB_RootEntCnt << endl;
//	BPB_TotSec32 = hexaToDecimal(reverseTrav(convertToList(sector, len), 23, 4));
//	cout << "So sector trong volume: " << BPB_TotSec32 << endl;
//
//	int BPB_FATSz32; // Số sector trong 1 bảng FAT (off set 24 - 4 bytes)
//	int BPB_RootClus;//Rootcluster location in the fat32 image
//	int BS_DrvNum; // Chỉ số cluster đầu tiên trong RDET (off set 2C - 4 bytes)
//}
//
//
//int main()
//{
//	uint8_t* result;
//	BYTE* sector = new BYTE[512];
//	FAT32 MBR;
//	ReadSector(L"\\\\.\\E:", 0, sector);
//	MBR.getInformation(sector, 512);
//	cout << hexStr(sector, 512);
//	//test(sector, 512);
//	//cout << hexaToDecimal("2F");
//	cout << endl;
//	for (int i = 0; i < 512; i++)
//	{
//
//		// create a row after every 16 
//		// columns so that display 
//		// looks good 
//		if (0 == i % 16)
//		{
//
//			cout << setw(6) << "\n";
//
//		}
//
//		BYTE b = sector[i];
//		//cout << isascii(b) ? hexStr(sector, i) : ".";
//		//cout << hexStr(sector, i) << " ";
//		cout << b << " ";
//	}
//
//
//
//	return 0;
//}