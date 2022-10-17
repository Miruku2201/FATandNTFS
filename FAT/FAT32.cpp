#include"FAT32.h"
int countTab = 0;
// Đọc thông tin từ "drive (ổ đĩa)" từ vị trí byte "readPoint" đến khi đủ 512 bytes
// Sau đó lưu vào mảng sector dưới dạng mã Ascii
// 1 sector => 512 bytes
void ReadSector(LPCWSTR drive, int readPoint, BYTE* sector)
{
	DWORD bytesRead; //trong thư viện windows.h, dùng để lưu khoảng lớn hơn
	HANDLE device = CreateFile(
		drive,    // Drive to open
		GENERIC_READ,           // Access mode
		FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
		NULL,                   // Security Descriptor
		OPEN_EXISTING,          // How to create
		0,                      // File attributes
		NULL);                  // Handle to template

	if (device == INVALID_HANDLE_VALUE) // hello 
	{
		std::cout << "Error, get error code: " << GetLastError() << std::endl;
		return;
	}

	SetFilePointer(device, readPoint, NULL, FILE_BEGIN);

	if (!ReadFile(device, sector, 512, &bytesRead, NULL))
	{
		std::cout << "Read file error: " << GetLastError() << std::endl;
		return;
	}
}

wstring strToWstr(std::string s)
{
	int slength = (int)s.length() + 1;																			// Lấy độ dài của chuỗi
	int wslen = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);				//	=> buffer = 0 ( giá trị kế cuối ) => độ dài của chuỗi s
	wchar_t* buf = new wchar_t[wslen];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, wslen);						// Copy dữ liệu từ string vào mảng wchar
	wstring r(buf);
	delete[] buf;
	return r;
}


// Chuyển từ hệ 16 -> hệ 10
int hexadecimalToDecimal(std::string hexa)
{
	int len = hexa.size();
	int base = 1;
	int dec_val = 0;
	for (int i = len - 1; i >= 0; i--) {
		if (hexa[i] >= '0' && hexa[i] <= '9') {
			dec_val += (int(hexa[i]) - 48) * base;
			base = base * 16;
		}
		else if (hexa[i] >= 'A' && hexa[i] <= 'F') {
			dec_val += (int(hexa[i]) - 55) * base;
			base = base * 16;
		}
		else {
			dec_val += (int(hexa[i] - 87) * base);
			base = base * 16;
		}
	}
	return dec_val;
}

// Chuyển từ hệ 10 -> Ascii
char decimalToAscii(int dec)
{
	return static_cast<char>(dec);
}

//Chuyển từ hệ 16 -> Ascii
char hexdecimalToAscii(std::string hexa)
{
	return decimalToAscii(hexadecimalToDecimal(hexa));
}

string hexToASCII(string hex)
{
	// initialize the ASCII code string as empty.
	string ascii = "";
	for (size_t i = 0; i < hex.length(); i += 2)
	{
		// extract two characters from hex string
		string part = hex.substr(i, 2);

		// change it into base 16 and
		// typecast as the character
		char ch = stoul(part, nullptr, 16);

		// add this char to final ASCII string
		ascii += ch;
	}
	return ascii;
}

// Chuyển từ bảng mã Ascii -> hệ 10
int asciiToDecimal(char ch) {
	return static_cast<int>(ch);
}

// Chuyển từ hệ 10 -> hệ 16
std::string decimalToHexdecimal(int dec) {
	stringstream hex;
	int i = 0;
	while (dec != 0) {
		int temp = 0;
		temp = dec % 16;
		if (temp < 10) {
			hex << static_cast<char>(temp + 48);
			i++;
		}
		else {
			hex << static_cast<char>(temp + 55);
			i++;
		}
		dec = dec / 16;
	}

	std::string _hex = hex.str();
	int n = _hex.length();
	for (int i = 0; i < n / 2; i++)
		swap(_hex[i], _hex[n - i - 1]);

	return _hex;
}

// Chuyển hệ Ascii-> 16
std::string asciiToHexdecimal(char ch) {
	return decimalToHexdecimal(asciiToDecimal(ch));
}

// Đọc ngược Byte
std::string LittleEndian(std::vector<string> sector, std::string offset_hex, int nBytes)
{
	int offset_dec = hexadecimalToDecimal(offset_hex);
	int offset_start = offset_dec + nBytes - 1;
	string str = "";
	std::vector<string>::iterator itr = sector.begin();
	advance(itr, offset_start);
	for (; nBytes != 0; --itr, --nBytes)
	{
		str += *itr;
	}
	return str;
}

std::string BigEndian(std::vector<string> sector, std::string offset_hex, int nBytes)
{
	int offset_dec = hexadecimalToDecimal(offset_hex);
	string str = "";
	std::vector<string>::iterator itr = sector.begin();
	advance(itr, offset_dec);
	for (int i(0); i < nBytes; ++itr, ++i)
	{
		str += *itr;
	}
	return str;
}

void printTab() {
	for (int i(0); i < countTab; i++) {
		std::cout << "\t";
	}
}


// Chuyển string các offsets -> vector với mỗi phần từ là 1 byte offset
// => có 512 phần tử
std::vector<string> convertToVector(const uint8_t* data, int len) {
	std::vector<string> offset;
	for (int i(0); i < len; ++i) {
		stringstream ss;
		ss << hex << uppercase;
		ss << setw(2) << setfill('0') << static_cast<int>(data[i]);
		offset.push_back(ss.str());
	}
	int j = 0;
	return offset;
}

std::string FAT32::read(uint8_t* sector, int len) {
	stringstream ss;
	ss << hex;
	if (ES_UPPERCASE)
		ss << uppercase;
	for (int i(0); i < len; ++i)
	{
		if (i % 16 == 0)
		{
			ss << "\n";
		}
		ss << setw(2) << setfill('0') << static_cast<int>(sector[i]);
		ss << " ";
	}

	return ss.str();
}

void FAT32::print(uint8_t* sector, int len) {
	std::cout << read(sector, len);
	std::cout << std::endl;

	BPB_BytesPerSec = hexadecimalToDecimal(LittleEndian(convertToVector(sector, len), "B", 2));
	cout << "So bytes/sector: " << BPB_BytesPerSec << " bytes" << endl;

	BPB_SecPerClus = hexadecimalToDecimal(LittleEndian(convertToVector(sector, len), "D", 1));
	cout << "So sectors/cluster: " << BPB_SecPerClus << " sector" << endl;

	BPB_RsvdSecCnt = hexadecimalToDecimal(LittleEndian(convertToVector(sector, len), "E", 2));
	cout << "So sectors truoc bang FAT: " << BPB_RsvdSecCnt << " sector" << endl;

	BPB_NumFATs = hexadecimalToDecimal(LittleEndian(convertToVector(sector, len), "10", 1));
	cout << "So bang FAT: " << BPB_NumFATs << endl;

	BPB_RootEntCnt = hexadecimalToDecimal(LittleEndian(convertToVector(sector, len), "11", 2));
	cout << "So entry trong bang RDET: " << BPB_RootEntCnt << endl;

	BPB_TotSec32 = hexadecimalToDecimal(LittleEndian(convertToVector(sector, len), "20", 4));
	cout << "So sector trong volume: " << BPB_TotSec32 << " sector" << endl;

	BPB_FATSz32 = hexadecimalToDecimal(LittleEndian(convertToVector(sector, len), "24", 4)); // Số sector trong 1 bảng FAT (off set 24 - 4 bytes)
	cout << "So sector trong 1 bang FAT: " << BPB_FATSz32 << " sector => Kich thuoc cua bang FAT: " << BPB_BytesPerSec * BPB_FATSz32 << " bytes" << endl;

	BPB_RootClus = hexadecimalToDecimal(LittleEndian(convertToVector(sector, len), "2C", 4));//Rootcluster location in the fat32 image
	cout << "Chi so cluster dau tien cua RDET: " << BPB_RootClus << endl;

	BS_DrvNum = hexadecimalToDecimal(LittleEndian(convertToVector(sector, len), "40", 1));
	cout << "Ki hieu vat ly dia: " << BS_DrvNum << endl;
	cout << "Vi tri cua bang FAT1: " << BPB_RsvdSecCnt * BPB_BytesPerSec << " bytes" << endl;
	cout << "Vi tri cua bang FAT2: " << (BPB_RsvdSecCnt + BPB_FATSz32) * BPB_BytesPerSec << " bytes" << endl;

	Data_Position = BPB_RsvdSecCnt + BPB_NumFATs * BPB_FATSz32;
	std::cout << Data_Position << std::endl;
	
	std::string FatName = LittleEndian(convertToVector(sector, len), "52", 8);
	std::cout << hexToASCII(FatName) << std::endl;
}

std::vector<int> FAT32::getAllClustersOfFile(int firstCluster) {
	std::vector<int> fileClusters;

	BYTE sector[512];

	// Chuyển đến vị trí bản FAT1 => Lấy các thông tin dữ liệu từ bản Cluster
	int readPoint = BPB_RsvdSecCnt * BPB_BytesPerSec;

	ReadSector(driver, readPoint, sector);
	//Chuyển đến sector tiếp theo (flag)
	readPoint += 512;

	int iCluster = firstCluster;

	do {
		if (iCluster == 0) {
			break;
		}
		fileClusters.push_back(iCluster);

		//FAT32 => bảng Fat1 có 4 byte để lữu trữ vị trí 1 cluster trong vùng DATA
		iCluster = hexadecimalToDecimal(LittleEndian(convertToVector(sector, 512), decimalToHexdecimal((4 * iCluster) % 512), 4));
		
		// Nếu iCluster lớn hơn 512 bytes hiện tại -> chuyển sang Sector kế tiếp.
		if (iCluster > readPoint / 4) {
			ReadSector(driver, readPoint, sector);
			readPoint += 512;
		}
	} while (iCluster != 268435455); // 0FFFFFFF

	return fileClusters;
}



// Tìm sector tương ứng với Cluster
vector<int> FAT32::getAllSectorsOfFile(std::vector<int> fileClusters) {
	vector<int> fileSectors;

	for (unsigned int i = 0; i < fileClusters.size(); i++) {
		int firstSector = findTheLogicSector(fileClusters[i]);

		for (int i = 0; i < BPB_SecPerClus; i++)
			fileSectors.push_back(firstSector + i);
	}

	return fileSectors;
}

//Doc noi dung tap tin
void FAT32::ReadData(std::string fileExtension, int firstCluster) {
	for (auto& c : fileExtension) c = toupper(c);
	
	if (strcmp(fileExtension.c_str(), "TXT") == 0) {
		if (firstCluster != 0) {
			printTab();
			std::cout << "Content:" << std::endl;
			vector<int> fileClusters = getAllClustersOfFile(firstCluster);
			vector<int> fileSectors = getAllSectorsOfFile(fileClusters);
			for (unsigned int i = 0; i < fileSectors.size(); i++) {
				BYTE sectorFile[512];

				int readPointFile = fileSectors[i] * BPB_BytesPerSec;
				
				ReadSector(driver,readPointFile, sectorFile);
				
				printTab();
				for (int j = 0; j < 512; j += 1) {
					std::cout << (int)sectorFile[j];
					if (sectorFile[j] == '\0') {
						std::cout << std::endl;
						return;
					}
				}
			}
		}
		std::cout << "\n";
	}
	else
		printTab();
		std::cout << "Can phan mem khac de doc file khac .txt\n";
}

//Lay thong tin tap tin
void FAT32::GetFileInfo(int firstCluster) {
	vector<int> fileClusters = getAllClustersOfFile(firstCluster);
	vector<int> fileSectors = getAllSectorsOfFile(fileClusters);

	printTab();
	std::cout << "Cluster bat dau: " << firstCluster << std::endl;

	printTab();
	std::cout << "Chiem cac cluster: ";
	for (unsigned int i = 0; i < fileClusters.size(); i++)
		std::cout << fileClusters[i] << "\t";
	std::cout << "\n";

	printTab();
	std::cout << "Chiem cac sector: ";
	for (unsigned int i = 0; i < fileSectors.size(); i++)
		std::cout << fileSectors[i] << "\t";
	std::cout << "\n";
}


// sector : 512 bytes
// n is stt cua entry. Tối đa 16 entry (0 -> 15)
std::vector<string> convertToEntry(std::vector<std::string> sector, int n) {
	std::vector<string> entry;
	for (int i(0); i < 32; i++) {
		entry.push_back(sector[n * 32 + i]);
	}
	return entry;
}

// Kiểm tra hàm có phải là Entry phụ không
bool isSubEntry(std::vector<std::string> entry) {
	if (entry[hexadecimalToDecimal("B")] == "0F") {
		return true;
	}
	return false;
}


// Kiểm tra hàm có phải là Entry chính không
bool isMainEntry(std::vector<std::string> entry) {
	if ((isSubEntry(entry)) || (entry[hexadecimalToDecimal("B")] == "00"))
		return false;
	return true;
}


// Hàm phân các vùng Entry 
// Hàm phân tách 16 entry thành các entry phụ của 1 entry chính sau đó đóng lại thành 1 vector các bộ entry
// Parameter: 1 vector gồm 512bytes ( 1 phân tử < = > 1 byte ) -- sector
// Return: 1 Vector gồm 1 pair với:
//									- first:	Là 32 bytes của entry chính
//									- second:	Là 1 vector dùng để chứa các entry phụ (vector<string>) 
std::vector<
	std::pair<
		std::vector<string>, std::vector<std::vector<std::string>>>>FAT32::splitEntries(int readPoint) {


	std::vector<
		std::pair<
			std::vector<string>, std::vector<std::vector<std::string>>>> entries;		// Vector để chứa toàn bộ các bộ entry chính và phụ

	std::pair<
		std::vector<string>, std::vector<std::vector<std::string>> > sub_and_main_entry; // Pair để chứa 1 entry chính và các entry phụ của nó
	BYTE sector[512];
	Miku:
	ReadSector(driver, readPoint, sector);
	std::vector<std::string> vector_sector = convertToVector(sector, 512);
	readPoint += 512;

	
	for (int i(0);; i++) {														// Vòng for chạy tới 16 vì chỉ có 16 entry trong 1 sector (16x32 = 512)
		std::vector<string> entry = convertToEntry(vector_sector, i);						// Lấy giá trị của entry thứ i / 16 entry
		if (entry[0] == "00") {
			break;
		}
		else {
			if (isSubEntry(entry)) {														// Kiểm tra xem có phải là Entry phụ không
				sub_and_main_entry.second.push_back(entry);								// Đúng thì lưu vào trong list của các entry phụ
			}
			else if (isMainEntry(entry)) {													// Nếu là Entry chính 
				sub_and_main_entry.first = entry;											// Lưu vào giá trị của Entry chính. Vì 1 bộ chỉ có 1 entry chính
				entries.push_back(sub_and_main_entry);									// Lưu bộ đó vào bên trong vector các bộ
				sub_and_main_entry.second.clear();											// Xóa tất cả dữ liệu của Entry phụ để bắt đầu 1 bộ Entry chính phụ mới
			}
			if (i == 15) {
				goto Miku;
			}
		}
	}
	return entries;
}

// Hàm Đọc tên Entry
// Parameter: 1 Bộ Entry
// Mô tả:	Nếu Entry chính không có Entry phụ thì :
//		- 8 byte đầu của entry sẽ là Tên File
//		- 3 byte sau của entry sẽ là Phần mở rộng
//
//
//			Nếu Entry chính có Entry phụ thì:
//		- Đọc từ byte 1 10 bytes
//		- Đọc từ byte E 12 bytes
//		- Đọc từ byte 1C 4 bytes

std::string FAT32::readNameEntry(std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>> sub_and_main_entry) {
	std::string name = "";
	stringstream ext;
	std::string extname = "";
	if (sub_and_main_entry.second.size() == 0) { // trường hợp nếu Entry chính không có Entry phụ
		std::string filename = hexToASCII(BigEndian(sub_and_main_entry.first, "0", 8));
		extname = hexToASCII(BigEndian(sub_and_main_entry.first, "8", 3));
		name = filename + "." + extname;
	}
	else {
		for (int i = sub_and_main_entry.second.size() - 1; i > -1; --i) { // Trường hợp Entry chính có Entry phụ

			name += hexToASCII(BigEndian(sub_and_main_entry.second[i], "1", 10)) +
				hexToASCII(BigEndian(sub_and_main_entry.second[i], "E", 12)) +
				hexToASCII(BigEndian(sub_and_main_entry.second[i], "1C", 4));
		}

		for (int i(0); i < name.length(); i++) {
			if (name[i] == '.') {
				ext << name[i + 2] << name[i + 4] << name[i + 6];
				break;
			}
		}
		extname = ext.str();
	}

	printTab();
	std::cout << "Ten: " << name << std::endl;
	
	int mainEntry_StartCluster = hexadecimalToDecimal(LittleEndian(sub_and_main_entry.first, "1A", 2));
	//printTab();
	//std::cout << "Cluster bat dau: " << mainEntry_StartCluster << std::endl;
	GetFileInfo(mainEntry_StartCluster);

	return extname;
}

bool FAT32::isDeleteFile(std::vector<std::string>mainEntry) {
	std::string firstbyte = BigEndian(mainEntry, "0", 1);
	if (firstbyte == "E5") {
		return true;
	}
	return false;
}

std::string FAT32::FileAttribute(std::vector<std::string> mainEntry) {
	std::string hexa = LittleEndian(mainEntry, "B", 1);
	return hexa;
}

void FAT32::GetDirectory(int cluster) {
	BYTE sector[512];
	int readPoint = findTheLogicSector(cluster) * BPB_BytesPerSec;
	ReadSector(driver, readPoint, sector);

	std::vector<
		std::pair<
		std::vector<string>, std::vector<std::vector<std::string>>>> entries = splitEntries(readPoint);
	for (int j(0); j < entries.size(); j++) {
		if (!isDeleteFile(entries[j].first)) {
			if (hexdecimalToAscii(entries[j].first[0]) != '.') {
				std::string fileatr = FileAttribute(entries[j].first);
				if (fileatr == "10" || fileatr == "16") {
					printTab();
					std::cout << "Loai: Thu Muc" << std::endl;
					readNameEntry(entries[j]);
					int mainEntry_StartCluster = hexadecimalToDecimal(LittleEndian(entries[j].first, "1A", 2));
					countTab++;
					GetDirectory(mainEntry_StartCluster);
					countTab--;
				}
				else if (LittleEndian(entries[j].first, "B", 1) == "20") {
					printTab();
					std::cout << "Loai: Tep Tin" << std::endl;
					std::string ext = readNameEntry(entries[j]);
					int mainEntry_StartCluster = hexadecimalToDecimal(LittleEndian(entries[j].first, "1A", 2));
					ReadData(ext, mainEntry_StartCluster);
					printTab();
					int mainEntry_SizeFile = hexadecimalToDecimal(LittleEndian(entries[j].first, "1C", 4));
					std::cout << "Kich thuoc cua File: " << mainEntry_SizeFile << " bytes" << std::endl;
				}
			}
		}
		std::cout << std::endl;
	}
}


// In kiểm tra thôi
void FAT32::printEntry(std::vector<std::string> entry) {
	for (int i(0); i < 32; i++) {
		if (i != 0 && i % 16 == 0) {
			std::cout << std::endl;
		}
		std::cout << entry[i] << " ";
	}
}

void FAT32::readFAT32(int readPoint, int len) {
	std::vector<
		std::pair<
			std::vector<string>, std::vector<std::vector<std::string>>>> entries = splitEntries(readPoint);
	for (int j(0); j < entries.size(); j++) {
		if (!isDeleteFile(entries[j].first)) {
			std::string fileatr = FileAttribute(entries[j].first);
			if (fileatr == "10" || fileatr == "16") {
				printTab();
				std::cout << "Loai: Thu Muc" << std::endl;
				readNameEntry(entries[j]);
				int mainEntry_StartCluster = hexadecimalToDecimal(LittleEndian(entries[j].first, "1A", 2));
				countTab++;
				GetDirectory(mainEntry_StartCluster);
				countTab--;
			}
			else if (fileatr == "20") {
				printTab();
				std::cout << "Loai: Tep Tin" << std::endl;
				std::string ext = readNameEntry(entries[j]);
				int mainEntry_StartCluster = hexadecimalToDecimal(LittleEndian(entries[j].first, "1A", 2));
				ReadData(ext, mainEntry_StartCluster);
				printTab();
				int mainEntry_SizeFile = hexadecimalToDecimal(LittleEndian(entries[j].first, "1C", 4));
				std::cout << "Kich thuoc cua File: " << mainEntry_SizeFile << " bytes" << std::endl;
			}
		}
	}
}

void FAT32::readRDECT() {
	BYTE RDECT[512];
	int FAT2_pos = (BPB_RsvdSecCnt + BPB_FATSz32) * BPB_BytesPerSec;
	int readPoint = FAT2_pos + BPB_FATSz32 * 512;
	while (true) {
		ReadSector(driver, readPoint, RDECT);
		std::vector<std::string> sector = convertToVector(RDECT, 512);
		if (sector[0] == "00") {
			break;
		}
		std::cout << read(RDECT, 512);
		std::cout << std::endl;

		readFAT32(readPoint, 512);
		readPoint += 512;
	}
}


