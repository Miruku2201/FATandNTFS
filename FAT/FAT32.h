#ifndef FAT32_H
#define FAT32_H
#pragma warning(disable:4996)

#include<iostream>
#include<iomanip>
#include<string>
#include<vector>
#include<sstream>
#include<Windows.h>
#include<stack>
#include<bitset>
using namespace std;

class FAT32
{
public:
	LPCWSTR driver = NULL;  //lưu tên đường dẫn
	string FATiden =""; //offset 52 - 8 bytes (định danh của FAT)
	string volumeType=""; //offset 15 - 1 byte, Loại volume, nhận biết bằng chuỗi hexa
	
	int BPB_BytesPerSec = 0; // Số bytes/sector (off set 0 - 3 bytes)  
	int BPB_SecPerClus = 0; // Số sector/cluster (off set 3 - 8 bytes)
	int BPB_RsvdSecCnt = 0; // Số sector để dành (khác 0)(Số sector trước bảng FAT) (off set B - 2 bytes)
	int BPB_NumFATs = 0; // Số bảng FAT (off set 10 - 1 bytes)
	int BPB_RootEntCnt = 0; // FAT12, FAT16: Số entry trong bảng RDET - FAT32: Có giá trị là 0 (off set 11 - 2 bytes)
	int BPB_TotSec32 = 0; // Số sector trong volume (off set 20 - 4 bytes)
	int BPB_FATSz32 = 0; // Số sector trong 1 bảng FAT (off set 24 - 4 bytes)
	int BPB_RootClus = 0;//Rootcluster location in the fat32 image
	int BS_DrvNum = 0; // Chỉ số cluster đầu tiên trong RDET (off set 2C - 4 bytes)
	int Data_Position = 0;
public:
	FAT32(LPCWSTR disk) {
		this->driver = disk;
	}
	std::string read(uint8_t* sector, int len); // hàm để đọc bootsector - viết hàm bên file function.cpp
	void print(uint8_t* sector, int len); // hàm để in thông tin về bootsector - như trên
	int getRootClus() {
		return BPB_RootClus;
	}

	// Tìm vị trí sector logic.
	int findTheLogicSector(int cluster) {
		return (cluster - 2) * BPB_SecPerClus + BPB_RsvdSecCnt + BPB_NumFATs * BPB_FATSz32;
	}

	std::vector<int> getAllClustersOfFile(int firstCluster);
	std::vector<int> getAllSectorsOfFile(std::vector<int> fileClusters);
	void ReadData(std::string fileExtension, int firstCluster);
	void GetFileInfo(int firstCluster);
	std::vector<std::pair<std::vector<string>, std::vector<std::vector<std::string>>>> splitEntries(int readPoint);
	std::string readNameEntry(std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>> sub_and_main_entry);
	void printEntry(std::vector<string>entry);
	void printEntryInfomation(std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>>);
	void GetDirectory(int cluster);
	void readFAT32(int readPoint, int len); 
	std::string FileAttribute(std::vector<std::string> mainEntry);
	bool isDeleteFile(std::vector<std::string>mainEntry);
	void readRDECT();
};

void ReadSector(LPCWSTR drive, int readPoint, BYTE* sector);
wstring strToWstr(std::string s);
int hexadecimalToDecimal(std::string hexa);// Chuyển từ hệ 16 -> hệ 10
char decimalToAscii(int dec);// Chuyển từ hệ 10 -> Ascii
char hexdecimalToAscii(std::string hexa);//Chuyển từ hệ 16 -> Ascii
int asciiToDecimal(char ch);// Chuyển từ bảng mã Ascii -> hệ 10
std::string decimalToHexdecimal(int dec);// Chuyển từ hệ 10 -> hệ 16
std::string asciiToHexdecimal(char ch);// Chuyển hệ Ascii-> 16
std::vector<string> convertToVector(const uint8_t* data, int len);
string LittleEndian(std::vector<string> temp, std::string offset_hex, int nBytes);
std::string BigEndian(std::vector<string> sector, std::string offset_hex, int nBytes);
std::vector<string> convertToEntry(std::vector<std::string> sector, int n); // Convert 512 byte -> 16 entry
bool isSubEntry(std::vector<std::string>entry);
bool isMainEntry(std::vector<std::string>entry);

void printTab();


#endif //!FAT32